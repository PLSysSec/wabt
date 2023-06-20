#include "wasm-rt-threads.h"

#include <assert.h>
#include <atomic>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <thread>

#define WAITER_ATOMIC_STATE_NOTIFIED 0
#define WAITER_ATOMIC_STATE_WAITING 1
#define WAITER_ATOMIC_STATE_TIMEDOUT 2

typedef struct {
  std::atomic<uint8_t> state;
} waiter_info_t;

typedef struct {
  std::list<std::shared_ptr<waiter_info_t>> waiter_list;
} waiters_info_t;

typedef struct {
  // Mutex to guard all accesses to waiters_info_map
  std::mutex map_guard;
  std::map<uintptr_t, waiters_info_t> waiters_info_map;
} wait_notify_info_t;

void* wasm_rt_initialize_wait_notify() {
  auto ret = new wait_notify_info_t;
  if (!ret) {
    abort();
  }
  return ret;
}

void wasm_rt_cleanup_wait_notify(void* wait_notify_info_void) {
  auto wait_notify_info =
      reinterpret_cast<wait_notify_info_t*>(wait_notify_info_void);
  delete wait_notify_info;
}

static void create_timeout(int64_t timeout,
                           std::weak_ptr<waiter_info_t> waiter_info_weak) {
  if (timeout < 0) {
    return;
  }

  std::thread([timeout, waiter_info_weak]() {
    const int64_t max_timeout_chunk = 1000000;  // 1 ms
    int64_t remaining = timeout;

    while (remaining > 0) {
      const int64_t chosen_time = std::min(remaining, max_timeout_chunk);

      std::this_thread::sleep_for(std::chrono::nanoseconds(chosen_time));
      remaining -= chosen_time;

      std::shared_ptr<waiter_info_t> waiter_info = waiter_info_weak.lock();

      // object was notified and the wait state no longer exists
      if (!waiter_info) {
        return;
      }
    }

    // timeout complete. Set the state to timed out if the object still exists
    // in the waiting state
    std::shared_ptr<waiter_info_t> waiter_info = waiter_info_weak.lock();
    if (waiter_info) {
      uint8_t expected = WAITER_ATOMIC_STATE_WAITING;
      if (waiter_info->state.compare_exchange_strong(
              expected, WAITER_ATOMIC_STATE_TIMEDOUT)) {
        waiter_info->state.notify_one();
      }
    }
  }).detach();
}

uint32_t wasm_rt_wait_on_address(void* wait_notify_info_void,
                                 uintptr_t address,
                                 int64_t timeout) {
  auto wait_notify_info =
      reinterpret_cast<wait_notify_info_t*>(wait_notify_info_void);

  auto waiter = std::make_shared<waiter_info_t>();
  waiter->state = WAITER_ATOMIC_STATE_WAITING;

  {
    std::lock_guard<std::mutex> lock(wait_notify_info->map_guard);
    wait_notify_info->waiters_info_map[address].waiter_list.push_back(waiter);
  }

  std::weak_ptr<waiter_info_t> waiter_weak = waiter;
  create_timeout(timeout, waiter_weak);

  waiter->state.wait(WAITER_ATOMIC_STATE_WAITING);

  {
    std::lock_guard<std::mutex> lock(wait_notify_info->map_guard);
    wait_notify_info->waiters_info_map[address].waiter_list.remove(waiter);
  }

  return waiter->state;
}

uint32_t wasm_rt_notify_at_address(void* wait_notify_info_void,
                                   uintptr_t address,
                                   uint32_t count) {
  auto wait_notify_info =
      reinterpret_cast<wait_notify_info_t*>(wait_notify_info_void);
  uint32_t curr_notified = 0;

  std::lock_guard<std::mutex> lock(wait_notify_info->map_guard);
  auto p_waiter_list =
      &(wait_notify_info->waiters_info_map[address].waiter_list);

  for (auto p_waiter : *p_waiter_list) {
    if (curr_notified == count) {
      break;
    }

    uint8_t expected = WAITER_ATOMIC_STATE_WAITING;
    if (p_waiter->state.compare_exchange_strong(expected,
                                                WAITER_ATOMIC_STATE_NOTIFIED)) {
      p_waiter->state.notify_one();
      curr_notified++;
    }
  }

  return curr_notified;
}
