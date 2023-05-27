const char* s_atomicops_source_declarations = R"w2c_template(#include <errno.h>
)w2c_template"
R"w2c_template(#include <limits.h>
)w2c_template"
R"w2c_template(
#ifndef _WIN32
)w2c_template"
R"w2c_template(// POSIX system
)w2c_template"
R"w2c_template(#include <linux/futex.h>
)w2c_template"
R"w2c_template(#include <sys/syscall.h>
)w2c_template"
R"w2c_template(#include <unistd.h>
)w2c_template"
R"w2c_template(#endif
)w2c_template"
R"w2c_template(
#if defined(_MSC_VER)
)w2c_template"
R"w2c_template(
#include <intrin.h>
)w2c_template"
R"w2c_template(
// Use MSVC intrinsics
)w2c_template"
R"w2c_template(
// For loads and stores, its not clear if we can rely on register width loads
)w2c_template"
R"w2c_template(// and stores to be atomic as reported here
)w2c_template"
R"w2c_template(// https://learn.microsoft.com/en-us/windows/win32/sync/interlocked-variable-access?redirectedfrom=MSDN
)w2c_template"
R"w2c_template(// or if we have to reuse other instrinsics
)w2c_template"
R"w2c_template(// https://stackoverflow.com/questions/42660091/atomic-load-in-c-with-msvc
)w2c_template"
R"w2c_template(// We reuse other intrinsics to be cautious
)w2c_template"
R"w2c_template(#define atomic_load_u8(a, v) _InterlockedOr8(a, 0)
)w2c_template"
R"w2c_template(#define atomic_load_u16(a, v) _InterlockedOr16(a, 0)
)w2c_template"
R"w2c_template(#define atomic_load_u32(a, v) _InterlockedOr(a, 0)
)w2c_template"
R"w2c_template(#define atomic_load_u64(a, v) _InterlockedOr64(a, 0)
)w2c_template"
R"w2c_template(
#define atomic_store_u8(a, v) _InterlockedExchange8(a, v)
)w2c_template"
R"w2c_template(#define atomic_store_u16(a, v) _InterlockedExchange16(a, v)
)w2c_template"
R"w2c_template(#define atomic_store_u32(a, v) _InterlockedExchange(a, v)
)w2c_template"
R"w2c_template(#define atomic_store_u64(a, v) _InterlockedExchange64(a, v)
)w2c_template"
R"w2c_template(
#define atomic_add_u8(a, v) _InterlockedExchangeAdd8(a, v)
)w2c_template"
R"w2c_template(#define atomic_add_u16(a, v) _InterlockedExchangeAdd16(a, v)
)w2c_template"
R"w2c_template(#define atomic_add_u32(a, v) _InterlockedExchangeAdd(a, v)
)w2c_template"
R"w2c_template(#define atomic_add_u64(a, v) _InterlockedExchangeAdd64(a, v)
)w2c_template"
R"w2c_template(
#define atomic_sub_u8(a, v) _InterlockedExchangeAdd8(a, -(v))
)w2c_template"
R"w2c_template(#define atomic_sub_u16(a, v) _InterlockedExchangeAdd16(a, -(v))
)w2c_template"
R"w2c_template(#define atomic_sub_u32(a, v) _InterlockedExchangeAdd(a, -(v))
)w2c_template"
R"w2c_template(#define atomic_sub_u64(a, v) _InterlockedExchangeAdd64(a, -(v))
)w2c_template"
R"w2c_template(
#define atomic_and_u8(a, v) _InterlockedAnd8(a, v)
)w2c_template"
R"w2c_template(#define atomic_and_u16(a, v) _InterlockedAnd16(a, v)
)w2c_template"
R"w2c_template(#define atomic_and_u32(a, v) _InterlockedAnd(a, v)
)w2c_template"
R"w2c_template(#define atomic_and_u64(a, v) _InterlockedAnd64(a, v)
)w2c_template"
R"w2c_template(
#define atomic_or_u8(a, v) _InterlockedOr8(a, v)
)w2c_template"
R"w2c_template(#define atomic_or_u16(a, v) _InterlockedOr16(a, v)
)w2c_template"
R"w2c_template(#define atomic_or_u32(a, v) _InterlockedOr(a, v)
)w2c_template"
R"w2c_template(#define atomic_or_u64(a, v) _InterlockedOr64(a, v)
)w2c_template"
R"w2c_template(
#define atomic_xor_u8(a, v) _InterlockedXor8(a, v)
)w2c_template"
R"w2c_template(#define atomic_xor_u16(a, v) _InterlockedXor16(a, v)
)w2c_template"
R"w2c_template(#define atomic_xor_u32(a, v) _InterlockedXor(a, v)
)w2c_template"
R"w2c_template(#define atomic_xor_u64(a, v) _InterlockedXor64(a, v)
)w2c_template"
R"w2c_template(
#define atomic_exchange_u8(a, v) _InterlockedExchange8(a, v)
)w2c_template"
R"w2c_template(#define atomic_exchange_u16(a, v) _InterlockedExchange16(a, v)
)w2c_template"
R"w2c_template(#define atomic_exchange_u32(a, v) _InterlockedExchange(a, v)
)w2c_template"
R"w2c_template(#define atomic_exchange_u64(a, v) _InterlockedExchange64(a, v)
)w2c_template"
R"w2c_template(
// clang-format off
)w2c_template"
R"w2c_template(#define atomic_compare_exchange_u8(a, expected_ptr, desired) _InterlockedCompareExchange8(a, desired, *(expected_ptr))
)w2c_template"
R"w2c_template(#define atomic_compare_exchange_u16(a, expected_ptr, desired) _InterlockedCompareExchange16(a, desired, *(expected_ptr))
)w2c_template"
R"w2c_template(#define atomic_compare_exchange_u32(a, expected_ptr, desired) _InterlockedCompareExchange(a, desired, *(expected_ptr))
)w2c_template"
R"w2c_template(#define atomic_compare_exchange_u64(a, expected_ptr, desired) _InterlockedCompareExchange64(a, desired, *(expected_ptr))
)w2c_template"
R"w2c_template(// clang-format on
)w2c_template"
R"w2c_template(
#define atomic_fence() _ReadWriteBarrier()
)w2c_template"
R"w2c_template(
#else
)w2c_template"
R"w2c_template(
// Use gcc/clang/icc intrinsics
)w2c_template"
R"w2c_template(#define atomic_load_u8(a) __atomic_load_n((u8*)(a), __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_load_u16(a) __atomic_load_n((u16*)(a), __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_load_u32(a) __atomic_load_n((u32*)(a), __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_load_u64(a) __atomic_load_n((u64*)(a), __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(
#define atomic_store_u8(a, v) __atomic_store_n((u8*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_store_u16(a, v) __atomic_store_n((u16*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_store_u32(a, v) __atomic_store_n((u32*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_store_u64(a, v) __atomic_store_n((u64*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(
#define atomic_add_u8(a, v) __atomic_fetch_add((u8*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_add_u16(a, v) __atomic_fetch_add((u16*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_add_u32(a, v) __atomic_fetch_add((u32*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_add_u64(a, v) __atomic_fetch_add((u64*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(
#define atomic_sub_u8(a, v) __atomic_fetch_sub((u8*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_sub_u16(a, v) __atomic_fetch_sub((u16*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_sub_u32(a, v) __atomic_fetch_sub((u32*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_sub_u64(a, v) __atomic_fetch_sub((u64*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(
#define atomic_and_u8(a, v) __atomic_fetch_and((u8*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_and_u16(a, v) __atomic_fetch_and((u16*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_and_u32(a, v) __atomic_fetch_and((u32*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_and_u64(a, v) __atomic_fetch_and((u64*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(
#define atomic_or_u8(a, v) __atomic_fetch_or((u8*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_or_u16(a, v) __atomic_fetch_or((u16*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_or_u32(a, v) __atomic_fetch_or((u32*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_or_u64(a, v) __atomic_fetch_or((u64*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(
#define atomic_xor_u8(a, v) __atomic_fetch_xor((u8*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_xor_u16(a, v) __atomic_fetch_xor((u16*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_xor_u32(a, v) __atomic_fetch_xor((u32*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_xor_u64(a, v) __atomic_fetch_xor((u64*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(
// clang-format off
)w2c_template"
R"w2c_template(#define atomic_exchange_u8(a, v) __atomic_exchange_n((u8*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_exchange_u16(a, v) __atomic_exchange_n((u16*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_exchange_u32(a, v) __atomic_exchange_n((u32*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_exchange_u64(a, v) __atomic_exchange_n((u64*)(a), v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(// clang-format on
)w2c_template"
R"w2c_template(
#define __atomic_compare_exchange_helper(a, expected_ptr, desired)        \
)w2c_template"
R"w2c_template(  (__atomic_compare_exchange_n(a, expected_ptr, desired, 0 /* is_weak */, \
)w2c_template"
R"w2c_template(                               __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST),       \
)w2c_template"
R"w2c_template(   *(expected_ptr))
)w2c_template"
R"w2c_template(
// clang-format off
)w2c_template"
R"w2c_template(#define atomic_compare_exchange_u8(a, expected_ptr, desired) __atomic_compare_exchange_helper((u8*)(a), expected_ptr, desired)
)w2c_template"
R"w2c_template(#define atomic_compare_exchange_u16(a, expected_ptr, desired) __atomic_compare_exchange_helper((u16*)(a), expected_ptr, desired)
)w2c_template"
R"w2c_template(#define atomic_compare_exchange_u32(a, expected_ptr, desired) __atomic_compare_exchange_helper((u32*)(a), expected_ptr, desired)
)w2c_template"
R"w2c_template(#define atomic_compare_exchange_u64(a, expected_ptr, desired) __atomic_compare_exchange_helper((u64*)(a), expected_ptr, desired)
)w2c_template"
R"w2c_template(// clang-format on
)w2c_template"
R"w2c_template(
#define atomic_fence() __atomic_thread_fence(__ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(
#endif
)w2c_template"
R"w2c_template(
#define ATOMIC_ALIGNMENT_CHECK(addr, t1) \
)w2c_template"
R"w2c_template(  if (UNLIKELY(addr % sizeof(t1))) {     \
)w2c_template"
R"w2c_template(    TRAP(UNALIGNED);                     \
)w2c_template"
R"w2c_template(  }
)w2c_template"
R"w2c_template(
#define DEFINE_ATOMIC_LOAD(name, t1, t2, t3)               \
)w2c_template"
R"w2c_template(  static inline t3 name(wasm_rt_memory_t* mem, u64 addr) { \
)w2c_template"
R"w2c_template(    MEMCHECK(mem, addr, t1);                               \
)w2c_template"
R"w2c_template(    ATOMIC_ALIGNMENT_CHECK(addr, t1);                      \
)w2c_template"
R"w2c_template(    t1 result;                                             \
)w2c_template"
R"w2c_template(    wasm_rt_memcpy(&result, &mem->data[addr], sizeof(t1)); \
)w2c_template"
R"w2c_template(    result = atomic_load_##t1(&mem->data[addr]);           \
)w2c_template"
R"w2c_template(    wasm_asm("" ::"r"(result));                            \
)w2c_template"
R"w2c_template(    return (t3)(t2)result;                                 \
)w2c_template"
R"w2c_template(  }
)w2c_template"
R"w2c_template(
DEFINE_ATOMIC_LOAD(i32_atomic_load, u32, u32, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_LOAD(i64_atomic_load, u64, u64, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_LOAD(i32_atomic_load8_u, u8, u32, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_LOAD(i64_atomic_load8_u, u8, u64, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_LOAD(i32_atomic_load16_u, u16, u32, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_LOAD(i64_atomic_load16_u, u16, u64, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_LOAD(i64_atomic_load32_u, u32, u64, u64)
)w2c_template"
R"w2c_template(
#define DEFINE_ATOMIC_STORE(name, t1, t2)                              \
)w2c_template"
R"w2c_template(  static inline void name(wasm_rt_memory_t* mem, u64 addr, t2 value) { \
)w2c_template"
R"w2c_template(    MEMCHECK(mem, addr, t1);                                           \
)w2c_template"
R"w2c_template(    ATOMIC_ALIGNMENT_CHECK(addr, t1);                                  \
)w2c_template"
R"w2c_template(    t1 wrapped = (t1)value;                                            \
)w2c_template"
R"w2c_template(    atomic_store_##t1(&mem->data[addr], wrapped);                      \
)w2c_template"
R"w2c_template(  }
)w2c_template"
R"w2c_template(
DEFINE_ATOMIC_STORE(i32_atomic_store, u32, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_STORE(i64_atomic_store, u64, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_STORE(i32_atomic_store8, u8, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_STORE(i32_atomic_store16, u16, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_STORE(i64_atomic_store8, u8, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_STORE(i64_atomic_store16, u16, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_STORE(i64_atomic_store32, u32, u64)
)w2c_template"
R"w2c_template(
#define DEFINE_ATOMIC_RMW(name, op, t1, t2)                          \
)w2c_template"
R"w2c_template(  static inline t2 name(wasm_rt_memory_t* mem, u64 addr, t2 value) { \
)w2c_template"
R"w2c_template(    MEMCHECK(mem, addr, t1);                                         \
)w2c_template"
R"w2c_template(    ATOMIC_ALIGNMENT_CHECK(addr, t1);                                \
)w2c_template"
R"w2c_template(    t1 wrapped = (t1)value;                                          \
)w2c_template"
R"w2c_template(    t1 ret = atomic_##op##_##t1(&mem->data[addr], wrapped);          \
)w2c_template"
R"w2c_template(    return (t2)ret;                                                  \
)w2c_template"
R"w2c_template(  }
)w2c_template"
R"w2c_template(
DEFINE_ATOMIC_RMW(i32_atomic_rmw8_add_u, add, u8, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i32_atomic_rmw16_add_u, add, u16, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i32_atomic_rmw_add, add, u32, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw8_add_u, add, u8, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw16_add_u, add, u16, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw32_add_u, add, u32, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw_add, add, u64, u64)
)w2c_template"
R"w2c_template(
DEFINE_ATOMIC_RMW(i32_atomic_rmw8_sub_u, sub, u8, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i32_atomic_rmw16_sub_u, sub, u16, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i32_atomic_rmw_sub, sub, u32, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw8_sub_u, sub, u8, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw16_sub_u, sub, u16, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw32_sub_u, sub, u32, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw_sub, sub, u64, u64)
)w2c_template"
R"w2c_template(
DEFINE_ATOMIC_RMW(i32_atomic_rmw8_and_u, and, u8, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i32_atomic_rmw16_and_u, and, u16, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i32_atomic_rmw_and, and, u32, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw8_and_u, and, u8, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw16_and_u, and, u16, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw32_and_u, and, u32, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw_and, and, u64, u64)
)w2c_template"
R"w2c_template(
DEFINE_ATOMIC_RMW(i32_atomic_rmw8_or_u, or, u8, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i32_atomic_rmw16_or_u, or, u16, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i32_atomic_rmw_or, or, u32, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw8_or_u, or, u8, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw16_or_u, or, u16, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw32_or_u, or, u32, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw_or, or, u64, u64)
)w2c_template"
R"w2c_template(
DEFINE_ATOMIC_RMW(i32_atomic_rmw8_xor_u, xor, u8, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i32_atomic_rmw16_xor_u, xor, u16, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i32_atomic_rmw_xor, xor, u32, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw8_xor_u, xor, u8, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw16_xor_u, xor, u16, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw32_xor_u, xor, u32, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw_xor, xor, u64, u64)
)w2c_template"
R"w2c_template(
DEFINE_ATOMIC_RMW(i32_atomic_rmw8_xchg_u, exchange, u8, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i32_atomic_rmw16_xchg_u, exchange, u16, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i32_atomic_rmw_xchg, exchange, u32, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw8_xchg_u, exchange, u8, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw16_xchg_u, exchange, u16, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw32_xchg_u, exchange, u32, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW(i64_atomic_rmw_xchg, exchange, u64, u64)
)w2c_template"
R"w2c_template(
#define DEFINE_ATOMIC_CMP_XCHG(name, t1, t2)                                   \
)w2c_template"
R"w2c_template(  static inline t1 name(wasm_rt_memory_t* mem, u64 addr, t1 expected,          \
)w2c_template"
R"w2c_template(                        t1 replacement) {                                      \
)w2c_template"
R"w2c_template(    MEMCHECK(mem, addr, t2);                                                   \
)w2c_template"
R"w2c_template(    ATOMIC_ALIGNMENT_CHECK(addr, t2);                                          \
)w2c_template"
R"w2c_template(    t2 expected_wrapped = (t2)expected;                                        \
)w2c_template"
R"w2c_template(    t2 replacement_wrapped = (t2)replacement;                                  \
)w2c_template"
R"w2c_template(    t2 old = atomic_compare_exchange_##t2(&mem->data[addr], &expected_wrapped, \
)w2c_template"
R"w2c_template(                                          replacement_wrapped);                \
)w2c_template"
R"w2c_template(    return (t1)old;                                                            \
)w2c_template"
R"w2c_template(  }
)w2c_template"
R"w2c_template(
DEFINE_ATOMIC_CMP_XCHG(i32_atomic_rmw8_cmpxchg_u, u32, u8);
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_CMP_XCHG(i32_atomic_rmw16_cmpxchg_u, u32, u16);
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_CMP_XCHG(i32_atomic_rmw_cmpxchg, u32, u32);
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_CMP_XCHG(i64_atomic_rmw8_cmpxchg_u, u64, u8);
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_CMP_XCHG(i64_atomic_rmw16_cmpxchg_u, u64, u16);
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_CMP_XCHG(i64_atomic_rmw32_cmpxchg_u, u64, u32);
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_CMP_XCHG(i64_atomic_rmw_cmpxchg, u64, u64);
)w2c_template"
R"w2c_template(
static int futex(uint32_t* uaddr,
)w2c_template"
R"w2c_template(                 int futex_op,
)w2c_template"
R"w2c_template(                 uint32_t val,
)w2c_template"
R"w2c_template(                 const struct timespec* timeout,
)w2c_template"
R"w2c_template(                 uint32_t* uaddr2,
)w2c_template"
R"w2c_template(                 uint32_t val3) {
)w2c_template"
R"w2c_template(  return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
)w2c_template"
R"w2c_template(}
)w2c_template"
R"w2c_template(
#ifndef _WIN32
)w2c_template"
R"w2c_template(
// POSIX system
)w2c_template"
R"w2c_template(
// Helper for atomic wait. This implements an atomic wait
)w2c_template"
R"w2c_template(// - operating on a 32-bit target (Linux kernel only supports this)
)w2c_template"
R"w2c_template(// - has spurious wake ups
)w2c_template"
R"w2c_template(// - returns 0 if valid or spurious wakeup, 2 if timedout
)w2c_template"
R"w2c_template(static u32 memory_atomic_wait_helper(wasm_rt_memory_t* mem,
)w2c_template"
R"w2c_template(                                     u64 addr,
)w2c_template"
R"w2c_template(                                     u32 initial,
)w2c_template"
R"w2c_template(                                     s64 timeout) {
)w2c_template"
R"w2c_template(  // Linux futex only supports 32-bit
)w2c_template"
R"w2c_template(  u32* futexp = (u32*)&mem->data[addr];
)w2c_template"
R"w2c_template(
  struct timespec futex_timeout;
)w2c_template"
R"w2c_template(  struct timespec* chosen_futex_timeout = NULL;
)w2c_template"
R"w2c_template(  if (timeout == 0) {
)w2c_template"
R"w2c_template(    // Not sure what timeout = 0 means. It seems like we can timeout
)w2c_template"
R"w2c_template(    // immediately.
)w2c_template"
R"w2c_template(    return 2;  // timed out
)w2c_template"
R"w2c_template(  } else if (timeout > 0) {
)w2c_template"
R"w2c_template(    const u64 nano = (u64)1000000000;
)w2c_template"
R"w2c_template(    futex_timeout.tv_sec = ((u64)timeout) / nano;
)w2c_template"
R"w2c_template(    futex_timeout.tv_nsec = ((u64)timeout) % nano;
)w2c_template"
R"w2c_template(    chosen_futex_timeout = &futex_timeout;
)w2c_template"
R"w2c_template(  }
)w2c_template"
R"w2c_template(
  long s =
)w2c_template"
R"w2c_template(      futex(futexp, FUTEX_WAIT_PRIVATE, initial, chosen_futex_timeout, NULL, 0);
)w2c_template"
R"w2c_template(
  if (s == -1 && errno == ETIMEDOUT) {
)w2c_template"
R"w2c_template(    return 2;  // timed out
)w2c_template"
R"w2c_template(  }
)w2c_template"
R"w2c_template(
  // clang-format off
)w2c_template"
R"w2c_template(  int is_valid_or_spurious_wakeup =
)w2c_template"
R"w2c_template(      (s == 0)                     ||  // regular path or spurious wake up
)w2c_template"
R"w2c_template(      (s == -1 && errno == EAGAIN) ||  // we already checked the initial value,
)w2c_template"
R"w2c_template(                                       // any subsequent matching failure could
)w2c_template"
R"w2c_template(                                       // be due to a notify
)w2c_template"
R"w2c_template(      (s == -1 && errno == EINTR)  ;   // According to the man page, old linux
)w2c_template"
R"w2c_template(                                       // kernels could have spurious interrupts
)w2c_template"
R"w2c_template(                                       // with EINTR
)w2c_template"
R"w2c_template(  // clang-format on
)w2c_template"
R"w2c_template(
  if (!is_valid_or_spurious_wakeup) {
)w2c_template"
R"w2c_template(    abort();
)w2c_template"
R"w2c_template(  }
)w2c_template"
R"w2c_template(
  return 0;
)w2c_template"
R"w2c_template(}
)w2c_template"
R"w2c_template(
static u32 memory_atomic_wait32(wasm_rt_memory_t* mem,
)w2c_template"
R"w2c_template(                                u64 addr,
)w2c_template"
R"w2c_template(                                u32 initial,
)w2c_template"
R"w2c_template(                                s64 timeout) {
)w2c_template"
R"w2c_template(  ATOMIC_ALIGNMENT_CHECK(addr, u32);
)w2c_template"
R"w2c_template(  MEMCHECK(mem, addr, u32);
)w2c_template"
R"w2c_template(
  if (i32_atomic_load(mem, addr) != initial) {
)w2c_template"
R"w2c_template(    return 1;  // initial value did not match
)w2c_template"
R"w2c_template(  }
)w2c_template"
R"w2c_template(
  do {
)w2c_template"
R"w2c_template(    u32 ret = memory_atomic_wait_helper(mem, addr, initial, timeout);
)w2c_template"
R"w2c_template(
    if (ret != 0) {
)w2c_template"
R"w2c_template(      return ret;
)w2c_template"
R"w2c_template(    }
)w2c_template"
R"w2c_template(
    // check for spurious
)w2c_template"
R"w2c_template(    if (i32_atomic_load(mem, addr) == initial) {
)w2c_template"
R"w2c_template(      continue;
)w2c_template"
R"w2c_template(    }
)w2c_template"
R"w2c_template(
    break;
)w2c_template"
R"w2c_template(  } while (1);
)w2c_template"
R"w2c_template(
  return 0;
)w2c_template"
R"w2c_template(}
)w2c_template"
R"w2c_template(
static u32 memory_atomic_wait64(wasm_rt_memory_t* mem,
)w2c_template"
R"w2c_template(                                u64 addr,
)w2c_template"
R"w2c_template(                                u64 initial,
)w2c_template"
R"w2c_template(                                s64 timeout) {
)w2c_template"
R"w2c_template(  ATOMIC_ALIGNMENT_CHECK(addr, u64);
)w2c_template"
R"w2c_template(  MEMCHECK(mem, addr, u64);
)w2c_template"
R"w2c_template(
  if (i64_atomic_load(mem, addr) != initial) {
)w2c_template"
R"w2c_template(    return 1;  // initial value did not match
)w2c_template"
R"w2c_template(  }
)w2c_template"
R"w2c_template(
  do {
)w2c_template"
R"w2c_template(    // memory_atomic_wait_helper only supports 32-bits target
)w2c_template"
R"w2c_template(    const u32 initial32 = (u32)initial;
)w2c_template"
R"w2c_template(    u64 ret = memory_atomic_wait_helper(mem, addr, initial32, timeout);
)w2c_template"
R"w2c_template(
    if (ret != 0) {
)w2c_template"
R"w2c_template(      return ret;
)w2c_template"
R"w2c_template(    }
)w2c_template"
R"w2c_template(
    // check for spurious
)w2c_template"
R"w2c_template(    if (i64_atomic_load(mem, addr) == initial) {
)w2c_template"
R"w2c_template(      continue;
)w2c_template"
R"w2c_template(    }
)w2c_template"
R"w2c_template(
    break;
)w2c_template"
R"w2c_template(  } while (1);
)w2c_template"
R"w2c_template(
  return 0;
)w2c_template"
R"w2c_template(}
)w2c_template"
R"w2c_template(
static u32 memory_atomic_notify(wasm_rt_memory_t* mem, u64 addr, u32 count) {
)w2c_template"
R"w2c_template(  ATOMIC_ALIGNMENT_CHECK(addr, u32);
)w2c_template"
R"w2c_template(  MEMCHECK(mem, addr, u32);
)w2c_template"
R"w2c_template(
  u32* futexp = (u32*)&mem->data[addr];
)w2c_template"
R"w2c_template(
  // linux futex can handle at most INT_MAX - 1, while INT_MAX wakes up all
)w2c_template"
R"w2c_template(  // waiters
)w2c_template"
R"w2c_template(  const unsigned int max_notify = ((unsigned int)INT_MAX) - 1;
)w2c_template"
R"w2c_template(  u32 remaining_notify = count;
)w2c_template"
R"w2c_template(  u32 ret = 0;
)w2c_template"
R"w2c_template(
  while (remaining_notify > 0) {
)w2c_template"
R"w2c_template(    const u32 curr_notify =
)w2c_template"
R"w2c_template(        remaining_notify <= max_notify ? remaining_notify : max_notify;
)w2c_template"
R"w2c_template(
    unsigned long woken_up = (unsigned long)futex(
)w2c_template"
R"w2c_template(        futexp, FUTEX_WAKE_PRIVATE, (int)curr_notify, NULL, NULL, 0);
)w2c_template"
R"w2c_template(    // We can only add values whose sum is less than or equal to the u32 count,
)w2c_template"
R"w2c_template(    // so no checks needed
)w2c_template"
R"w2c_template(    ret += (u32)woken_up;
)w2c_template"
R"w2c_template(    remaining_notify -= curr_notify;
)w2c_template"
R"w2c_template(  }
)w2c_template"
R"w2c_template(
  return ret;
)w2c_template"
R"w2c_template(}
)w2c_template"
R"w2c_template(
#else
)w2c_template"
R"w2c_template(
static u32 memory_atomic_wait32(wasm_rt_memory_t* mem,
)w2c_template"
R"w2c_template(                                u64 addr,
)w2c_template"
R"w2c_template(                                u32 expected,
)w2c_template"
R"w2c_template(                                s64 timeout) {
)w2c_template"
R"w2c_template(  return 0;
)w2c_template"
R"w2c_template(}
)w2c_template"
R"w2c_template(
static u32 memory_atomic_wait64(wasm_rt_memory_t* mem,
)w2c_template"
R"w2c_template(                                u64 addr,
)w2c_template"
R"w2c_template(                                u64 expected,
)w2c_template"
R"w2c_template(                                s64 timeout) {
)w2c_template"
R"w2c_template(  return 0;
)w2c_template"
R"w2c_template(}
)w2c_template"
R"w2c_template(
static u32 memory_atomic_notify(wasm_rt_memory_t* mem, u64 addr, u32 count) {
)w2c_template"
R"w2c_template(  return 0;
)w2c_template"
R"w2c_template(}
)w2c_template"
R"w2c_template(
#endif
)w2c_template"
;
