const char* s_sharedmem_source_declarations = R"w2c_template(#if defined(_MSC_VER)
)w2c_template"
R"w2c_template(
#include <intrin.h>
)w2c_template"
R"w2c_template(
// For loads and stores, its not clear how we can memory_order_relaxed in MSVC
)w2c_template"
R"w2c_template(// https://learn.microsoft.com/en-us/windows/win32/sync/interlocked-variable-access
)w2c_template"
R"w2c_template(// So we fallback to acquire release which is a stronger memory model for now
)w2c_template"
R"w2c_template(#define atomic_load_relaxed_u8(a) _InterlockedOr8Acquire(a, 0)
)w2c_template"
R"w2c_template(#define atomic_load_relaxed_u16(a) _InterlockedOr16Acquire(a, 0)
)w2c_template"
R"w2c_template(#define atomic_load_relaxed_u32(a) _InterlockedOrAcquire(a, 0)
)w2c_template"
R"w2c_template(#define atomic_load_relaxed_u64(a) _InterlockedOr64Acquire(a, 0)
)w2c_template"
R"w2c_template(#define atomic_load_relaxed_s8(a) _InterlockedOr8Acquire(a, 0)
)w2c_template"
R"w2c_template(#define atomic_load_relaxed_s16(a) _InterlockedOr16Acquire(a, 0)
)w2c_template"
R"w2c_template(#define atomic_load_relaxed_s32(a) _InterlockedOrAcquire(a, 0)
)w2c_template"
R"w2c_template(#define atomic_load_relaxed_s64(a) _InterlockedOr64Acquire(a, 0)
)w2c_template"
R"w2c_template(#define atomic_load_relaxed_f32(a) _InterlockedOrAcquire(a, 0)
)w2c_template"
R"w2c_template(#define atomic_load_relaxed_f64(a) _InterlockedOr64Acquire(a, 0)
)w2c_template"
R"w2c_template(
#define atomic_store_relaxed_u8(a, v) _InterlockedExchange8Release(a, v)
)w2c_template"
R"w2c_template(#define atomic_store_relaxed_u16(a, v) _InterlockedExchange16Release(a, v)
)w2c_template"
R"w2c_template(#define atomic_store_relaxed_u32(a, v) _InterlockedExchangeRelease(a, v)
)w2c_template"
R"w2c_template(#define atomic_store_relaxed_u64(a, v) _InterlockedExchange64Release(a, v)
)w2c_template"
R"w2c_template(#define atomic_store_relaxed_s8(a, v) _InterlockedExchange8Release(a, v)
)w2c_template"
R"w2c_template(#define atomic_store_relaxed_s16(a, v) _InterlockedExchange16Release(a, v)
)w2c_template"
R"w2c_template(#define atomic_store_relaxed_s32(a, v) _InterlockedExchangeRelease(a, v)
)w2c_template"
R"w2c_template(#define atomic_store_relaxed_s64(a, v) _InterlockedExchange64Release(a, v)
)w2c_template"
R"w2c_template(#define atomic_store_relaxed_f32(a, v) _InterlockedExchangeRelease(a, v)
)w2c_template"
R"w2c_template(#define atomic_store_relaxed_f64(a, v) _InterlockedExchange64Release(a, v)
)w2c_template"
R"w2c_template(
#else
)w2c_template"
R"w2c_template(
// Use gcc/clang/icc intrinsics
)w2c_template"
R"w2c_template(#define atomic_load_relaxed_u8(a) __atomic_load_n((u8*)(a), __ATOMIC_RELAXED)
)w2c_template"
R"w2c_template(#define atomic_load_relaxed_u16(a) __atomic_load_n((u16*)(a), __ATOMIC_RELAXED)
)w2c_template"
R"w2c_template(#define atomic_load_relaxed_u32(a) __atomic_load_n((u32*)(a), __ATOMIC_RELAXED)
)w2c_template"
R"w2c_template(#define atomic_load_relaxed_u64(a) __atomic_load_n((u64*)(a), __ATOMIC_RELAXED)
)w2c_template"
R"w2c_template(#define atomic_load_relaxed_s8(a) __atomic_load_n((s8*)(a), __ATOMIC_RELAXED)
)w2c_template"
R"w2c_template(#define atomic_load_relaxed_s16(a) __atomic_load_n((s16*)(a), __ATOMIC_RELAXED)
)w2c_template"
R"w2c_template(#define atomic_load_relaxed_s32(a) __atomic_load_n((s32*)(a), __ATOMIC_RELAXED)
)w2c_template"
R"w2c_template(#define atomic_load_relaxed_s64(a) __atomic_load_n((s64*)(a), __ATOMIC_RELAXED)
)w2c_template"
R"w2c_template(#define atomic_load_relaxed_f32(a) (f32) __atomic_load_n((u32*)(a), __ATOMIC_RELAXED)
)w2c_template"
R"w2c_template(#define atomic_load_relaxed_f64(a) (f64) __atomic_load_n((u64*)(a), __ATOMIC_RELAXED)
)w2c_template"
R"w2c_template(
#define atomic_store_relaxed_u8(a, v) __atomic_store_n((u8*)(a), v, __ATOMIC_RELAXED)
)w2c_template"
R"w2c_template(#define atomic_store_relaxed_u16(a, v) __atomic_store_n((u16*)(a), v, __ATOMIC_RELAXED)
)w2c_template"
R"w2c_template(#define atomic_store_relaxed_u32(a, v) __atomic_store_n((u32*)(a), v, __ATOMIC_RELAXED)
)w2c_template"
R"w2c_template(#define atomic_store_relaxed_u64(a, v) __atomic_store_n((u64*)(a), v, __ATOMIC_RELAXED)
)w2c_template"
R"w2c_template(#define atomic_store_relaxed_s8(a, v) __atomic_store_n((s8*)(a), v, __ATOMIC_RELAXED)
)w2c_template"
R"w2c_template(#define atomic_store_relaxed_s16(a, v) __atomic_store_n((s16*)(a), v, __ATOMIC_RELAXED)
)w2c_template"
R"w2c_template(#define atomic_store_relaxed_s32(a, v) __atomic_store_n((s32*)(a), v, __ATOMIC_RELAXED)
)w2c_template"
R"w2c_template(#define atomic_store_relaxed_s64(a, v) __atomic_store_n((s64*)(a), v, __ATOMIC_RELAXED)
)w2c_template"
R"w2c_template(#define atomic_store_relaxed_f32(a, v) __atomic_store_n((u32*)(a), (u32) v, __ATOMIC_RELAXED)
)w2c_template"
R"w2c_template(#define atomic_store_relaxed_f64(a, v) __atomic_store_n((u64*)(a), (u64) v, __ATOMIC_RELAXED)
)w2c_template"
R"w2c_template(
#endif
)w2c_template"
R"w2c_template(
#define DEFINE_SHARED_LOAD(name, t1, t2, t3, force_read)          \
)w2c_template"
R"w2c_template(  static inline t3 name(wasm_rt_shared_memory_t* mem, u64 addr) { \
)w2c_template"
R"w2c_template(    MEMCHECK(mem, addr, t1);                                      \
)w2c_template"
R"w2c_template(    t1 result;                                                    \
)w2c_template"
R"w2c_template(    result = atomic_load_relaxed_##t1(&mem->data[addr]);          \
)w2c_template"
R"w2c_template(    force_read(result);                                           \
)w2c_template"
R"w2c_template(    return (t3)(t2)result;                                        \
)w2c_template"
R"w2c_template(  }
)w2c_template"
R"w2c_template(
DEFINE_SHARED_LOAD(i32_load_shared, u32, u32, u32, FORCE_READ_INT)
)w2c_template"
R"w2c_template(DEFINE_SHARED_LOAD(i64_load_shared, u64, u64, u64, FORCE_READ_INT)
)w2c_template"
R"w2c_template(DEFINE_SHARED_LOAD(f32_load_shared, f32, f32, f32, FORCE_READ_FLOAT)
)w2c_template"
R"w2c_template(DEFINE_SHARED_LOAD(f64_load_shared, f64, f64, f64, FORCE_READ_FLOAT)
)w2c_template"
R"w2c_template(DEFINE_SHARED_LOAD(i32_load8_s_shared, s8, s32, u32, FORCE_READ_INT)
)w2c_template"
R"w2c_template(DEFINE_SHARED_LOAD(i64_load8_s_shared, s8, s64, u64, FORCE_READ_INT)
)w2c_template"
R"w2c_template(DEFINE_SHARED_LOAD(i32_load8_u_shared, u8, u32, u32, FORCE_READ_INT)
)w2c_template"
R"w2c_template(DEFINE_SHARED_LOAD(i64_load8_u_shared, u8, u64, u64, FORCE_READ_INT)
)w2c_template"
R"w2c_template(DEFINE_SHARED_LOAD(i32_load16_s_shared, s16, s32, u32, FORCE_READ_INT)
)w2c_template"
R"w2c_template(DEFINE_SHARED_LOAD(i64_load16_s_shared, s16, s64, u64, FORCE_READ_INT)
)w2c_template"
R"w2c_template(DEFINE_SHARED_LOAD(i32_load16_u_shared, u16, u32, u32, FORCE_READ_INT)
)w2c_template"
R"w2c_template(DEFINE_SHARED_LOAD(i64_load16_u_shared, u16, u64, u64, FORCE_READ_INT)
)w2c_template"
R"w2c_template(DEFINE_SHARED_LOAD(i64_load32_s_shared, s32, s64, u64, FORCE_READ_INT)
)w2c_template"
R"w2c_template(DEFINE_SHARED_LOAD(i64_load32_u_shared, u32, u64, u64, FORCE_READ_INT)
)w2c_template"
R"w2c_template(
#define DEFINE_SHARED_STORE(name, t1, t2)                                     \
)w2c_template"
R"w2c_template(  static inline void name(wasm_rt_shared_memory_t* mem, u64 addr, t2 value) { \
)w2c_template"
R"w2c_template(    MEMCHECK(mem, addr, t1);                                                  \
)w2c_template"
R"w2c_template(    t1 wrapped = (t1)value;                                                   \
)w2c_template"
R"w2c_template(    atomic_store_relaxed_##t1(&mem->data[addr], wrapped);                     \
)w2c_template"
R"w2c_template(  }
)w2c_template"
R"w2c_template(
DEFINE_SHARED_STORE(i32_store_shared, u32, u32)
)w2c_template"
R"w2c_template(DEFINE_SHARED_STORE(i64_store_shared, u64, u64)
)w2c_template"
R"w2c_template(DEFINE_SHARED_STORE(f32_store_shared, f32, f32)
)w2c_template"
R"w2c_template(DEFINE_SHARED_STORE(f64_store_shared, f64, f64)
)w2c_template"
R"w2c_template(DEFINE_SHARED_STORE(i32_store8_shared, u8, u32)
)w2c_template"
R"w2c_template(DEFINE_SHARED_STORE(i32_store16_shared, u16, u32)
)w2c_template"
R"w2c_template(DEFINE_SHARED_STORE(i64_store8_shared, u8, u64)
)w2c_template"
R"w2c_template(DEFINE_SHARED_STORE(i64_store16_shared, u16, u64)
)w2c_template"
R"w2c_template(DEFINE_SHARED_STORE(i64_store32_shared, u32, u64)
)w2c_template"
;
