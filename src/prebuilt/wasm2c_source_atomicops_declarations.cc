const char* s_source_atomicops_declarations = R"w2c_template(#if __STDC_VERSION__ >= 201112L
)w2c_template"
R"w2c_template(
#include <stdatomic.h>
)w2c_template"
R"w2c_template(
#else
)w2c_template"
R"w2c_template(
// Use gcc/clang/icc intrinsics
)w2c_template"
R"w2c_template(#define atomic_load(a) __atomic_load_n(a, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_store(a, v) __atomic_store_n(a, v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_fetch_add(a, v) __atomic_fetch_add(a, v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_fetch_sub(a, v) __atomic_fetch_sub(a, v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_fetch_and(a, v) __atomic_fetch_and(a, v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_fetch_or(a, v) __atomic_fetch_or(a, v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_fetch_xor(a, v) __atomic_fetch_xor(a, v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_exchange(a, v) __atomic_exchange_n(a, v, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#define atomic_compare_exchange_strong(a, expected_ptr, desired)         \
)w2c_template"
R"w2c_template(  __atomic_compare_exchange_n(a, expected_ptr, desired, 0 /* is_weak */, \
)w2c_template"
R"w2c_template(                              __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
)w2c_template"
R"w2c_template(#endif
)w2c_template"
R"w2c_template(
#define ATOMIC_ALIGNMENT_CHECK(addr, t1) \
)w2c_template"
R"w2c_template(  if ((addr & (sizeof(t1) - 1)) != 0) {  \
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
R"w2c_template(    result = atomic_load((t1*)&mem->data[addr]);           \
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
R"w2c_template(    atomic_store((t1*)&mem->data[addr], wrapped);                      \
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
R"w2c_template(    t1 ret = atomic_fetch_##op((t1*)&mem->data[addr], wrapped);      \
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
#define DEFINE_ATOMIC_RMW_XCHG(name, t1, t2)                         \
)w2c_template"
R"w2c_template(  static inline t2 name(wasm_rt_memory_t* mem, u64 addr, t2 value) { \
)w2c_template"
R"w2c_template(    MEMCHECK(mem, addr, t1);                                         \
)w2c_template"
R"w2c_template(    ATOMIC_ALIGNMENT_CHECK(addr, t1);                                \
)w2c_template"
R"w2c_template(    t1 wrapped = (t1)value;                                          \
)w2c_template"
R"w2c_template(    t1 ret = atomic_exchange((t1*)&mem->data[addr], wrapped);        \
)w2c_template"
R"w2c_template(    return (t2)ret;                                                  \
)w2c_template"
R"w2c_template(  }
)w2c_template"
R"w2c_template(
DEFINE_ATOMIC_RMW_XCHG(i32_atomic_rmw8_xchg_u, u8, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW_XCHG(i32_atomic_rmw16_xchg_u, u16, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW_XCHG(i32_atomic_rmw_xchg, u32, u32)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW_XCHG(i64_atomic_rmw8_xchg_u, u8, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW_XCHG(i64_atomic_rmw16_xchg_u, u16, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW_XCHG(i64_atomic_rmw32_xchg_u, u32, u64)
)w2c_template"
R"w2c_template(DEFINE_ATOMIC_RMW_XCHG(i64_atomic_rmw_xchg, u64, u64)
)w2c_template"
R"w2c_template(
#define DEFINE_ATOMIC_CMP_XCHG(name, t1, t2)                                 \
)w2c_template"
R"w2c_template(  static inline t1 name(wasm_rt_memory_t* mem, u64 addr, t1 expected,        \
)w2c_template"
R"w2c_template(                        t1 replacement) {                                    \
)w2c_template"
R"w2c_template(    MEMCHECK(mem, addr, t2);                                                 \
)w2c_template"
R"w2c_template(    ATOMIC_ALIGNMENT_CHECK(addr, t2);                                        \
)w2c_template"
R"w2c_template(    t2 expected_wrapped = (t2)expected;                                      \
)w2c_template"
R"w2c_template(    t2 replacement_wrapped = (t2)replacement;                                \
)w2c_template"
R"w2c_template(    atomic_compare_exchange_strong((t2*)&mem->data[addr], &expected_wrapped, \
)w2c_template"
R"w2c_template(                                   replacement_wrapped);                     \
)w2c_template"
R"w2c_template(    t2 ret = expected_wrapped /* expected_wrapped now has the old value */;  \
)w2c_template"
R"w2c_template(    return (t1)ret;                                                          \
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
static inline void atomic_fence() {
)w2c_template"
R"w2c_template(  __atomic_thread_fence(__ATOMIC_SEQ_CST);
)w2c_template"
R"w2c_template(}
)w2c_template"
;
