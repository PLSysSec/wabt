#include <map>

#include <stdint.h>

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;
typedef float f32;
typedef double f64;

#define atomic_load_u8(a) __atomic_load_n((u8*)(a), __ATOMIC_SEQ_CST)
#define atomic_load_u16(a) __atomic_load_n((u16*)(a), __ATOMIC_SEQ_CST)
#define atomic_load_u32(a) __atomic_load_n((u32*)(a), __ATOMIC_SEQ_CST)
#define atomic_load_u64(a) __atomic_load_n((u64*)(a), __ATOMIC_SEQ_CST)

#define atomic_store_u8(a, v) __atomic_store_n((u8*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_store_u16(a, v) __atomic_store_n((u16*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_store_u32(a, v) __atomic_store_n((u32*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_store_u64(a, v) __atomic_store_n((u64*)(a), v, __ATOMIC_SEQ_CST)

#define __atomic_compare_exchange_helper(a, expected_ptr, desired)        \
  (__atomic_compare_exchange_n(a, expected_ptr, desired, 0 /* is_weak */, \
                               __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST),       \
   *(expected_ptr))

#define atomic_compare_exchange_u8(a, expected_ptr, desired) __atomic_compare_exchange_helper((u8*)(a), expected_ptr, desired)
#define atomic_compare_exchange_u16(a, expected_ptr, desired) __atomic_compare_exchange_helper((u16*)(a), expected_ptr, desired)
#define atomic_compare_exchange_u32(a, expected_ptr, desired) __atomic_compare_exchange_helper((u32*)(a), expected_ptr, desired)
#define atomic_compare_exchange_u64(a, expected_ptr, desired) __atomic_compare_exchange_helper((u64*)(a), expected_ptr, desired)

#include <time.h>

#define wasm2c_timer_t struct timespec
#define wasm2c_timer_start(ptr_timer_info) clock_gettime(CLOCK_MONOTONIC, ptr_timer_info)
#define wasm2c_timer_end(ptr_start_info, ptr_diff_nano) do {                                                               \
  wasm2c_timer_t end_info;                                                                                              \
  wasm2c_timer_start(&end_info);                                                                                           \
  uint64_t nano = 1000000000;                                                                                              \
  *(ptr_diff_nano) = (end_info.tv_sec - (ptr_start_info)->tv_sec) * nano + (end_info.tv_nsec - (ptr_start_info)->tv_nsec); \
} while (0)

#include <pthread.h>

#define wasm2c_mutex_t pthread_mutex_t
#define wasm2c_mutex_lock(ptr_mutex) pthread_mutex_lock(ptr_mutex);
#define wasm2c_mutex_unlock(ptr_mutex) pthread_mutex_unlock(ptr_mutex);

typedef struct {
  uint32_t flag;
  waiter_info* prev;
  waiter_info* next;
} waiter_info;

typedef struct waiters {
  waiter_info dummy_head;
  waiter_info* instances;

  waiters() : dummy_head({0}), instances (&dummy_head){}

} waiters;

wasm2c_mutex_t full_waiters_mutex;
std::map<uintptr_t, waiters> full_waiters_map;

void add_waiter_info_helper(waiters* waiters_list, waiter_info* element) {
  waiter_info* curr = waiters_list->instances;
  while(curr->next) {
    curr = curr->next;
  };
  element->prev = curr;
  curr->next = element;
}

void remove_waiter_info(waiter_info* element) {
  element->prev->next = element->next;
}

void wasm2c_add_waiter_info(waiter_info* info, uintptr_t address) {
  wasm2c_mutex_lock(&full_waiters_mutex);
  add_waiter_info_helper(&(full_waiters_map[address]), info);
  wasm2c_mutex_unlock(&full_waiters_mutex);
}

void wasm2c_remove_waiter_info(waiter_info* info, uintptr_t address) {
  wasm2c_mutex_lock(&full_waiters_mutex);
  remove_waiter_info(info);

  if (full_waiters_map[address].instances->next == NULL) {
    // only dummy head found
    full_waiters_map.erase(address);
  }
  wasm2c_mutex_unlock(&full_waiters_mutex);
}

#define WASM_WAIT_WOKEN_UP 0
#define WASM_WAIT_NOT_EQUAL 1
#define WASM_WAIT_TIMED_OUT 2

int wait_helper(waiter_info* info, uintptr_t address, int64_t timeout) {
  if (timeout == 0) {
    // Not sure what timeout = 0 means. It seems like we can timeout
    // immediately.
    return WASM_WAIT_TIMED_OUT;
  } else if (timeout < 0) {
    while(atomic_load_u32(&(info->flag)) == 0) {
    };
  } else {

    wasm2c_timer_t start_info;
    wasm2c_timer_start(&start_info);

    while(atomic_load_u32(&(info->flag)) == 0) {
      uint64_t nano_elapsed = 0;
      wasm2c_timer_end(&start_info, &nano_elapsed);

      if (nano_elapsed > timeout) {
        return WASM_WAIT_TIMED_OUT;
      }
    };
  }

  return WASM_WAIT_WOKEN_UP;
}

int wait(uintptr_t address, int64_t timeout) {
  waiter_info info {0};

  // register info with global waiters
  wasm2c_add_waiter_info(&info, address);

  int ret = wait_helper(&info, address, timeout);

  // remove info from global waiters
  wasm2c_remove_waiter_info(&info, address);

  return ret;
}

uint32_t notify(uintptr_t address) {
  uint32_t ret = 0;

  wasm2c_mutex_lock(&full_waiters_mutex);
  waiter_info* waiter = full_waiters_map[address].instances->next;
  while (waiter) {
    uint32_t expected = 0;
    uint32_t old = atomic_compare_exchange_u32(&(waiter->flag), &expected, 1);
    if (old == expected) {
      ret++;
    }
    waiter = waiter->next;
  }
  wasm2c_mutex_unlock(&full_waiters_mutex);

  return ret;
}


int main(int argc, char const *argv[])
{
  /* code */
  return 0;
}

