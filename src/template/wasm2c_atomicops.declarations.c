#include <errno.h>
#include <limits.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#endif

#if defined(_MSC_VER)

#include <intrin.h>

// Use MSVC intrinsics

// For loads and stores, its not clear if we can rely on register width loads
// and stores to be atomic as reported here
// https://learn.microsoft.com/en-us/windows/win32/sync/interlocked-variable-access?redirectedfrom=MSDN
// or if we have to reuse other instrinsics
// https://stackoverflow.com/questions/42660091/atomic-load-in-c-with-msvc
// We reuse other intrinsics to be cautious
#define atomic_load_u8(a) _InterlockedOr8(a, 0)
#define atomic_load_u16(a) _InterlockedOr16(a, 0)
#define atomic_load_u32(a) _InterlockedOr(a, 0)
#define atomic_load_u64(a) _InterlockedOr64(a, 0)

#define atomic_store_u8(a, v) _InterlockedExchange8(a, v)
#define atomic_store_u16(a, v) _InterlockedExchange16(a, v)
#define atomic_store_u32(a, v) _InterlockedExchange(a, v)
#define atomic_store_u64(a, v) _InterlockedExchange64(a, v)

#define atomic_add_u8(a, v) _InterlockedExchangeAdd8(a, v)
#define atomic_add_u16(a, v) _InterlockedExchangeAdd16(a, v)
#define atomic_add_u32(a, v) _InterlockedExchangeAdd(a, v)
#define atomic_add_u64(a, v) _InterlockedExchangeAdd64(a, v)

#define atomic_sub_u8(a, v) _InterlockedExchangeAdd8(a, -(v))
#define atomic_sub_u16(a, v) _InterlockedExchangeAdd16(a, -(v))
#define atomic_sub_u32(a, v) _InterlockedExchangeAdd(a, -(v))
#define atomic_sub_u64(a, v) _InterlockedExchangeAdd64(a, -(v))

#define atomic_and_u8(a, v) _InterlockedAnd8(a, v)
#define atomic_and_u16(a, v) _InterlockedAnd16(a, v)
#define atomic_and_u32(a, v) _InterlockedAnd(a, v)
#define atomic_and_u64(a, v) _InterlockedAnd64(a, v)

#define atomic_or_u8(a, v) _InterlockedOr8(a, v)
#define atomic_or_u16(a, v) _InterlockedOr16(a, v)
#define atomic_or_u32(a, v) _InterlockedOr(a, v)
#define atomic_or_u64(a, v) _InterlockedOr64(a, v)

#define atomic_xor_u8(a, v) _InterlockedXor8(a, v)
#define atomic_xor_u16(a, v) _InterlockedXor16(a, v)
#define atomic_xor_u32(a, v) _InterlockedXor(a, v)
#define atomic_xor_u64(a, v) _InterlockedXor64(a, v)

#define atomic_exchange_u8(a, v) _InterlockedExchange8(a, v)
#define atomic_exchange_u16(a, v) _InterlockedExchange16(a, v)
#define atomic_exchange_u32(a, v) _InterlockedExchange(a, v)
#define atomic_exchange_u64(a, v) _InterlockedExchange64(a, v)

// clang-format off
#define atomic_compare_exchange_u8(a, expected_ptr, desired) _InterlockedCompareExchange8(a, desired, *(expected_ptr))
#define atomic_compare_exchange_u16(a, expected_ptr, desired) _InterlockedCompareExchange16(a, desired, *(expected_ptr))
#define atomic_compare_exchange_u32(a, expected_ptr, desired) _InterlockedCompareExchange(a, desired, *(expected_ptr))
#define atomic_compare_exchange_u64(a, expected_ptr, desired) _InterlockedCompareExchange64(a, desired, *(expected_ptr))
// clang-format on

#define atomic_fence() _ReadWriteBarrier()

#else

// Use gcc/clang/icc intrinsics
#define atomic_load_u8(a) __atomic_load_n((u8*)(a), __ATOMIC_SEQ_CST)
#define atomic_load_u16(a) __atomic_load_n((u16*)(a), __ATOMIC_SEQ_CST)
#define atomic_load_u32(a) __atomic_load_n((u32*)(a), __ATOMIC_SEQ_CST)
#define atomic_load_u64(a) __atomic_load_n((u64*)(a), __ATOMIC_SEQ_CST)

#define atomic_store_u8(a, v) __atomic_store_n((u8*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_store_u16(a, v) __atomic_store_n((u16*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_store_u32(a, v) __atomic_store_n((u32*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_store_u64(a, v) __atomic_store_n((u64*)(a), v, __ATOMIC_SEQ_CST)

#define atomic_add_u8(a, v) __atomic_fetch_add((u8*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_add_u16(a, v) __atomic_fetch_add((u16*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_add_u32(a, v) __atomic_fetch_add((u32*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_add_u64(a, v) __atomic_fetch_add((u64*)(a), v, __ATOMIC_SEQ_CST)

#define atomic_sub_u8(a, v) __atomic_fetch_sub((u8*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_sub_u16(a, v) __atomic_fetch_sub((u16*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_sub_u32(a, v) __atomic_fetch_sub((u32*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_sub_u64(a, v) __atomic_fetch_sub((u64*)(a), v, __ATOMIC_SEQ_CST)

#define atomic_and_u8(a, v) __atomic_fetch_and((u8*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_and_u16(a, v) __atomic_fetch_and((u16*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_and_u32(a, v) __atomic_fetch_and((u32*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_and_u64(a, v) __atomic_fetch_and((u64*)(a), v, __ATOMIC_SEQ_CST)

#define atomic_or_u8(a, v) __atomic_fetch_or((u8*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_or_u16(a, v) __atomic_fetch_or((u16*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_or_u32(a, v) __atomic_fetch_or((u32*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_or_u64(a, v) __atomic_fetch_or((u64*)(a), v, __ATOMIC_SEQ_CST)

#define atomic_xor_u8(a, v) __atomic_fetch_xor((u8*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_xor_u16(a, v) __atomic_fetch_xor((u16*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_xor_u32(a, v) __atomic_fetch_xor((u32*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_xor_u64(a, v) __atomic_fetch_xor((u64*)(a), v, __ATOMIC_SEQ_CST)

// clang-format off
#define atomic_exchange_u8(a, v) __atomic_exchange_n((u8*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_exchange_u16(a, v) __atomic_exchange_n((u16*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_exchange_u32(a, v) __atomic_exchange_n((u32*)(a), v, __ATOMIC_SEQ_CST)
#define atomic_exchange_u64(a, v) __atomic_exchange_n((u64*)(a), v, __ATOMIC_SEQ_CST)
// clang-format on

#define __atomic_compare_exchange_helper(a, expected_ptr, desired)        \
  (__atomic_compare_exchange_n(a, expected_ptr, desired, 0 /* is_weak */, \
                               __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST),       \
   *(expected_ptr))

// clang-format off
#define atomic_compare_exchange_u8(a, expected_ptr, desired) __atomic_compare_exchange_helper((u8*)(a), expected_ptr, desired)
#define atomic_compare_exchange_u16(a, expected_ptr, desired) __atomic_compare_exchange_helper((u16*)(a), expected_ptr, desired)
#define atomic_compare_exchange_u32(a, expected_ptr, desired) __atomic_compare_exchange_helper((u32*)(a), expected_ptr, desired)
#define atomic_compare_exchange_u64(a, expected_ptr, desired) __atomic_compare_exchange_helper((u64*)(a), expected_ptr, desired)
// clang-format on

#define atomic_fence() __atomic_thread_fence(__ATOMIC_SEQ_CST)

#endif

#define ATOMIC_ALIGNMENT_CHECK(addr, t1) \
  if (UNLIKELY(addr % sizeof(t1))) {     \
    TRAP(UNALIGNED);                     \
  }

#define DEFINE_ATOMIC_LOAD(name, t1, t2, t3)               \
  static inline t3 name(wasm_rt_memory_t* mem, u64 addr) { \
    MEMCHECK(mem, addr, t1);                               \
    ATOMIC_ALIGNMENT_CHECK(addr, t1);                      \
    t1 result;                                             \
    wasm_rt_memcpy(&result, &mem->data[addr], sizeof(t1)); \
    result = atomic_load_##t1(&mem->data[addr]);           \
    wasm_asm("" ::"r"(result));                            \
    return (t3)(t2)result;                                 \
  }

DEFINE_ATOMIC_LOAD(i32_atomic_load, u32, u32, u32)
DEFINE_ATOMIC_LOAD(i64_atomic_load, u64, u64, u64)
DEFINE_ATOMIC_LOAD(i32_atomic_load8_u, u8, u32, u32)
DEFINE_ATOMIC_LOAD(i64_atomic_load8_u, u8, u64, u64)
DEFINE_ATOMIC_LOAD(i32_atomic_load16_u, u16, u32, u32)
DEFINE_ATOMIC_LOAD(i64_atomic_load16_u, u16, u64, u64)
DEFINE_ATOMIC_LOAD(i64_atomic_load32_u, u32, u64, u64)

#define DEFINE_ATOMIC_STORE(name, t1, t2)                              \
  static inline void name(wasm_rt_memory_t* mem, u64 addr, t2 value) { \
    MEMCHECK(mem, addr, t1);                                           \
    ATOMIC_ALIGNMENT_CHECK(addr, t1);                                  \
    t1 wrapped = (t1)value;                                            \
    atomic_store_##t1(&mem->data[addr], wrapped);                      \
  }

DEFINE_ATOMIC_STORE(i32_atomic_store, u32, u32)
DEFINE_ATOMIC_STORE(i64_atomic_store, u64, u64)
DEFINE_ATOMIC_STORE(i32_atomic_store8, u8, u32)
DEFINE_ATOMIC_STORE(i32_atomic_store16, u16, u32)
DEFINE_ATOMIC_STORE(i64_atomic_store8, u8, u64)
DEFINE_ATOMIC_STORE(i64_atomic_store16, u16, u64)
DEFINE_ATOMIC_STORE(i64_atomic_store32, u32, u64)

#define DEFINE_ATOMIC_RMW(name, op, t1, t2)                          \
  static inline t2 name(wasm_rt_memory_t* mem, u64 addr, t2 value) { \
    MEMCHECK(mem, addr, t1);                                         \
    ATOMIC_ALIGNMENT_CHECK(addr, t1);                                \
    t1 wrapped = (t1)value;                                          \
    t1 ret = atomic_##op##_##t1(&mem->data[addr], wrapped);          \
    return (t2)ret;                                                  \
  }

DEFINE_ATOMIC_RMW(i32_atomic_rmw8_add_u, add, u8, u32)
DEFINE_ATOMIC_RMW(i32_atomic_rmw16_add_u, add, u16, u32)
DEFINE_ATOMIC_RMW(i32_atomic_rmw_add, add, u32, u32)
DEFINE_ATOMIC_RMW(i64_atomic_rmw8_add_u, add, u8, u64)
DEFINE_ATOMIC_RMW(i64_atomic_rmw16_add_u, add, u16, u64)
DEFINE_ATOMIC_RMW(i64_atomic_rmw32_add_u, add, u32, u64)
DEFINE_ATOMIC_RMW(i64_atomic_rmw_add, add, u64, u64)

DEFINE_ATOMIC_RMW(i32_atomic_rmw8_sub_u, sub, u8, u32)
DEFINE_ATOMIC_RMW(i32_atomic_rmw16_sub_u, sub, u16, u32)
DEFINE_ATOMIC_RMW(i32_atomic_rmw_sub, sub, u32, u32)
DEFINE_ATOMIC_RMW(i64_atomic_rmw8_sub_u, sub, u8, u64)
DEFINE_ATOMIC_RMW(i64_atomic_rmw16_sub_u, sub, u16, u64)
DEFINE_ATOMIC_RMW(i64_atomic_rmw32_sub_u, sub, u32, u64)
DEFINE_ATOMIC_RMW(i64_atomic_rmw_sub, sub, u64, u64)

DEFINE_ATOMIC_RMW(i32_atomic_rmw8_and_u, and, u8, u32)
DEFINE_ATOMIC_RMW(i32_atomic_rmw16_and_u, and, u16, u32)
DEFINE_ATOMIC_RMW(i32_atomic_rmw_and, and, u32, u32)
DEFINE_ATOMIC_RMW(i64_atomic_rmw8_and_u, and, u8, u64)
DEFINE_ATOMIC_RMW(i64_atomic_rmw16_and_u, and, u16, u64)
DEFINE_ATOMIC_RMW(i64_atomic_rmw32_and_u, and, u32, u64)
DEFINE_ATOMIC_RMW(i64_atomic_rmw_and, and, u64, u64)

DEFINE_ATOMIC_RMW(i32_atomic_rmw8_or_u, or, u8, u32)
DEFINE_ATOMIC_RMW(i32_atomic_rmw16_or_u, or, u16, u32)
DEFINE_ATOMIC_RMW(i32_atomic_rmw_or, or, u32, u32)
DEFINE_ATOMIC_RMW(i64_atomic_rmw8_or_u, or, u8, u64)
DEFINE_ATOMIC_RMW(i64_atomic_rmw16_or_u, or, u16, u64)
DEFINE_ATOMIC_RMW(i64_atomic_rmw32_or_u, or, u32, u64)
DEFINE_ATOMIC_RMW(i64_atomic_rmw_or, or, u64, u64)

DEFINE_ATOMIC_RMW(i32_atomic_rmw8_xor_u, xor, u8, u32)
DEFINE_ATOMIC_RMW(i32_atomic_rmw16_xor_u, xor, u16, u32)
DEFINE_ATOMIC_RMW(i32_atomic_rmw_xor, xor, u32, u32)
DEFINE_ATOMIC_RMW(i64_atomic_rmw8_xor_u, xor, u8, u64)
DEFINE_ATOMIC_RMW(i64_atomic_rmw16_xor_u, xor, u16, u64)
DEFINE_ATOMIC_RMW(i64_atomic_rmw32_xor_u, xor, u32, u64)
DEFINE_ATOMIC_RMW(i64_atomic_rmw_xor, xor, u64, u64)

DEFINE_ATOMIC_RMW(i32_atomic_rmw8_xchg_u, exchange, u8, u32)
DEFINE_ATOMIC_RMW(i32_atomic_rmw16_xchg_u, exchange, u16, u32)
DEFINE_ATOMIC_RMW(i32_atomic_rmw_xchg, exchange, u32, u32)
DEFINE_ATOMIC_RMW(i64_atomic_rmw8_xchg_u, exchange, u8, u64)
DEFINE_ATOMIC_RMW(i64_atomic_rmw16_xchg_u, exchange, u16, u64)
DEFINE_ATOMIC_RMW(i64_atomic_rmw32_xchg_u, exchange, u32, u64)
DEFINE_ATOMIC_RMW(i64_atomic_rmw_xchg, exchange, u64, u64)

#define DEFINE_ATOMIC_CMP_XCHG(name, t1, t2)                                   \
  static inline t1 name(wasm_rt_memory_t* mem, u64 addr, t1 expected,          \
                        t1 replacement) {                                      \
    MEMCHECK(mem, addr, t2);                                                   \
    ATOMIC_ALIGNMENT_CHECK(addr, t2);                                          \
    t2 expected_wrapped = (t2)expected;                                        \
    t2 replacement_wrapped = (t2)replacement;                                  \
    t2 old = atomic_compare_exchange_##t2(&mem->data[addr], &expected_wrapped, \
                                          replacement_wrapped);                \
    return (t1)old;                                                            \
  }

DEFINE_ATOMIC_CMP_XCHG(i32_atomic_rmw8_cmpxchg_u, u32, u8);
DEFINE_ATOMIC_CMP_XCHG(i32_atomic_rmw16_cmpxchg_u, u32, u16);
DEFINE_ATOMIC_CMP_XCHG(i32_atomic_rmw_cmpxchg, u32, u32);
DEFINE_ATOMIC_CMP_XCHG(i64_atomic_rmw8_cmpxchg_u, u64, u8);
DEFINE_ATOMIC_CMP_XCHG(i64_atomic_rmw16_cmpxchg_u, u64, u16);
DEFINE_ATOMIC_CMP_XCHG(i64_atomic_rmw32_cmpxchg_u, u64, u32);
DEFINE_ATOMIC_CMP_XCHG(i64_atomic_rmw_cmpxchg, u64, u64);

#ifdef _WIN32

typedef struct {
  void* futex_addr;
  HANDLE win_event;
  w2c_memory_atomic_winwait_event* next;
} w2c_memory_atomic_winwait_event;

static w2c_memory_atomic_winwait_event* memory_atomic_add_wait_event(wasm_rt_atomics_info_t* atomics_info, void* futex_addr, HANDLE win_event) {
  w2c_memory_atomic_winwait_event* evt = malloc(sizeof(w2c_memory_atomic_winwait_event));
  evt->futex_addr = futex_addr;
  evt->win_event = win_event;

  w2c_memory_atomic_winwait_event* null_expected = NULL;
  w2c_memory_atomic_winwait_event* prev = NULL;
  w2c_memory_atomic_winwait_event* curr = (w2c_memory_atomic_winwait_event*) atomics_info->event_info;

  do {

    if (!curr) {
      curr = atomic_compare_exchange_u64(curr, &null_expected, evt);
      if (curr == NULL) {
        // add succeeded
        break;
      }
      // else there was an concurrent call to memory_atomic_add_wait_event
      // continue iterating to the end of the list using the new curr
    }

    prev = curr;
    curr = (w2c_memory_atomic_winwait_event*) atomic_load_u64(curr->next);
  } while(1);

  return evt;
}

static w2c_memory_atomic_winwait_event* memory_atomic_get_next_wait_event(wasm_rt_atomics_info_t* atomics_info, void* futex_addr, w2c_memory_atomic_winwait_event* prev) {

  w2c_memory_atomic_winwait_event* curr = NULL;

  if (prev == NULL) {
    curr = (w2c_memory_atomic_winwait_event*) atomics_info->event_info;
  } else {
    curr = (w2c_memory_atomic_winwait_event*) atomic_load_u64(curr->next);
  }

  do {

    if (!curr) {
      // reached the end
      break;
    }

    if (curr->futex_addr == futex_addr) {
      // found the value
      break;
    }

    // not found, go to the next element

    curr = (w2c_memory_atomic_winwait_event*) atomic_load_u64(curr->next);
  } while (1);

  return curr;
}

static void memory_atomic_remove_wait_event(wasm_rt_atomics_info_t* atomics_info, w2c_memory_atomic_winwait_event* evt) {
  abort();
}

static u32 memory_atomic_wait_helper(wasm_rt_atomics_info_t* atomics_info, wasm_rt_memory_t* mem,
                                u64 addr,
                                s64 timeout) {
  void* futexp = (void*)&mem->data[addr];
  DWORD win_timeout = INFINITE;
  if (timeout == 0) {
    // Not sure what timeout = 0 means. It seems like we can timeout
    // immediately.
    return 2;  // timed out
  } else if (timeout > 0) {
#define div_roundup(x, y)     q = x/y + (x % y != 0);
    win_timeout = div_roundup(timeout, 1000000);
#undef div_roundup
  }

  uint32_t prev_wait_instances = atomic_add_u32(atomics_info->wait_instances, 1);
  if (prev_wait_instances == UINT32_MAX - 1) {
    TRAP(MAX_WAITERS);
  }

  HANDLE win_evt = CreateEvent(NULL, TRUE /* reset after each signal */, FALSE /* is signalled initially */, NULL);
  if (win_evt == NULL) {
    abort();
  }
  w2c_memory_atomic_winwait_event* evt = memory_atomic_add_wait_event(atomics_info, futexp, &win_evt);

  DWORD ret = WaitForSingleObject(win_evt, win_timeout);

  memory_atomic_remove_wait_event(atomics_info, evt);
  atomic_sub_u32(atomics_info->wait_instances, 1);

  if (ret == WAIT_TIMEOUT) {
    return 2;  // timed out
  } else if (ret != WAIT_OBJECT_0) {
    abort(); // some error occurred
  }

  return 0;
}

static u32 memory_atomic_wait32(wasm_rt_atomics_info_t* atomics_info, wasm_rt_memory_t* mem,
                                u64 addr,
                                u32 initial,
                                s64 timeout) {
  ATOMIC_ALIGNMENT_CHECK(addr, u32);
  MEMCHECK(mem, addr, u32);

  if (i32_atomic_load(mem, addr) != initial) {
    return 1;  // initial value did not match
  }

  u32 ret = memory_atomic_wait_helper(atomics_info, mem, addr, timeout);
  return ret;
}

static u64 memory_atomic_wait64(wasm_rt_atomics_info_t* atomics_info, wasm_rt_memory_t* mem,
                                u64 addr,
                                u64 initial,
                                s64 timeout) {
  ATOMIC_ALIGNMENT_CHECK(addr, u64);
  MEMCHECK(mem, addr, u64);

  if (i64_atomic_load(mem, addr) != initial) {
    return 1;  // initial value did not match
  }

  u32 ret = memory_atomic_wait_helper(atomics_info, mem, addr, timeout);
  return ret;
}


static u32 memory_atomic_notify(wasm_rt_atomics_info_t* atomics_info, wasm_rt_memory_t* mem, u64 addr, u32 count) {
  ATOMIC_ALIGNMENT_CHECK(addr, u32);
  MEMCHECK(mem, addr, u32);

  u32* futexp = (u32*)&mem->data[addr];

  u32 count = 0;
  w2c_memory_atomic_winwait_event* curr = NULL;

  do {
    curr = memory_atomic_get_next_wait_event(atomics_info, futexp, curr);
    if (!curr) {
      break;
    }
    count++;

    BOOL succeeded = SetEvent(curr->win_event);
    if (!succeeded) {
      abort();
    }
  } while(1);

  return count;
}

#else

// POSIX system

static int futex(uint32_t* uaddr,
                 int futex_op,
                 uint32_t val,
                 const struct timespec* timeout,
                 uint32_t* uaddr2,
                 uint32_t val3) {
  return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

// Helper for atomic wait. This implements an atomic wait
// - operating on a 32-bit target (Linux kernel only supports this)
// - has spurious wake ups
// - returns 0 if valid or spurious wakeup, 2 if timedout
static u32 memory_atomic_wait_helper(wasm_rt_atomics_info_t* atomics_info, 
                                     wasm_rt_memory_t* mem,
                                     u64 addr,
                                     u32 initial,
                                     s64 timeout) {
  // Linux futex only supports 32-bit
  u32* futexp = (u32*)&mem->data[addr];

  struct timespec futex_timeout;
  struct timespec* chosen_futex_timeout = NULL;
  if (timeout == 0) {
    // Not sure what timeout = 0 means. It seems like we can timeout
    // immediately.
    return 2;  // timed out
  } else if (timeout > 0) {
    const u64 nano = (u64)1000000000;
    futex_timeout.tv_sec = ((u64)timeout) / nano;
    futex_timeout.tv_nsec = ((u64)timeout) % nano;
    chosen_futex_timeout = &futex_timeout;
  }

  uint32_t prev_wait_instances = atomic_add_u32(atomics_info->wait_instances, 1);
  if (prev_wait_instances == UINT32_MAX - 1) {
    TRAP(MAX_WAITERS);
  }

  long s =
      futex(futexp, FUTEX_WAIT_PRIVATE, initial, chosen_futex_timeout, NULL, 0);
  atomic_sub_u32(atomics_info->wait_instances, 1);

  if (s == -1 && errno == ETIMEDOUT) {
    return 2;  // timed out
  }

  // clang-format off
  int is_valid_or_spurious_wakeup =
      (s == 0)                     ||  // regular path or spurious wake up
      (s == -1 && errno == EAGAIN) ||  // we already checked the initial value,
                                       // any subsequent matching failure could
                                       // be due to a notify
      (s == -1 && errno == EINTR)  ;   // According to the man page, old linux
                                       // kernels could have spurious interrupts
                                       // with EINTR
  // clang-format on

  if (!is_valid_or_spurious_wakeup) {
    abort();
  }

  return 0;
}

static u32 memory_atomic_wait32(wasm_rt_atomics_info_t* atomics_info, wasm_rt_memory_t* mem,
                                u64 addr,
                                u32 initial,
                                s64 timeout) {
  ATOMIC_ALIGNMENT_CHECK(addr, u32);
  MEMCHECK(mem, addr, u32);

  if (i32_atomic_load(mem, addr) != initial) {
    return 1;  // initial value did not match
  }

  do {
    u32 ret = memory_atomic_wait_helper(atomics_info, mem, addr, initial, timeout);

    if (ret != 0) {
      return ret;
    }

    // check for spurious
    if (i32_atomic_load(mem, addr) == initial) {
      continue;
    }

    break;
  } while (1);

  return 0;
}

static u32 memory_atomic_wait64(wasm_rt_atomics_info_t* atomics_info, wasm_rt_memory_t* mem,
                                u64 addr,
                                u64 initial,
                                s64 timeout) {
  ATOMIC_ALIGNMENT_CHECK(addr, u64);
  MEMCHECK(mem, addr, u64);

  if (i64_atomic_load(mem, addr) != initial) {
    return 1;  // initial value did not match
  }

  do {
    // memory_atomic_wait_helper only supports 32-bits target
    const u32 initial32 = (u32)initial;
    u64 ret = memory_atomic_wait_helper(atomics_info, mem, addr, initial32, timeout);

    if (ret != 0) {
      return ret;
    }

    // check for spurious
    if (i64_atomic_load(mem, addr) == initial) {
      continue;
    }

    break;
  } while (1);

  return 0;
}

static u32 memory_atomic_notify(wasm_rt_atomics_info_t* atomics_info, wasm_rt_memory_t* mem, u64 addr, u32 count) {
  ATOMIC_ALIGNMENT_CHECK(addr, u32);
  MEMCHECK(mem, addr, u32);

  u32* futexp = (u32*)&mem->data[addr];

  // linux futex can handle at most INT_MAX - 1, while INT_MAX wakes up all
  // waiters
  const unsigned int max_notify = ((unsigned int)INT_MAX) - 1;
  u32 remaining_notify = count;
  u32 ret = 0;

  while (remaining_notify > 0) {
    const u32 curr_notify =
        remaining_notify <= max_notify ? remaining_notify : max_notify;

    unsigned long woken_up = (unsigned long)futex(
        futexp, FUTEX_WAKE_PRIVATE, (int)curr_notify, NULL, NULL, 0);
    // We can only add values whose sum is less than or equal to the u32 count,
    // so no checks needed
    ret += (u32)woken_up;
    remaining_notify -= curr_notify;
  }

  return ret;
}

#endif
