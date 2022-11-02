/* Automatically generated by wasm2c */
#ifndef BASIC_WASI_PROG_H_GENERATED_
#define BASIC_WASI_PROG_H_GENERATED_

#include <stdint.h>

#include "wasm-rt.h"

/* TODO(binji): only use stdint.h types in header */
#ifndef WASM_RT_CORE_TYPES_DEFINED
#define WASM_RT_CORE_TYPES_DEFINED
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
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct Z_wasi_snapshot_preview1_instance_t;

typedef struct Z_basic_wasi_prog_instance_t {
  struct Z_wasi_snapshot_preview1_instance_t* Z_wasi_snapshot_preview1_instance;
  u32 w2c___stack_pointer;
  wasm_rt_memory_t w2c_memory;
  wasm_rt_funcref_table_t w2c_T0;
} Z_basic_wasi_prog_instance_t;

void Z_basic_wasi_prog_init_module(void);
void Z_basic_wasi_prog_instantiate(Z_basic_wasi_prog_instance_t*, struct Z_wasi_snapshot_preview1_instance_t*);
void Z_basic_wasi_prog_free(Z_basic_wasi_prog_instance_t*);

/* import: 'wasi_snapshot_preview1' 'args_get' */
u32 Z_wasi_snapshot_preview1Z_args_get(struct Z_wasi_snapshot_preview1_instance_t*, u32, u32);
/* import: 'wasi_snapshot_preview1' 'args_sizes_get' */
u32 Z_wasi_snapshot_preview1Z_args_sizes_get(struct Z_wasi_snapshot_preview1_instance_t*, u32, u32);
/* import: 'wasi_snapshot_preview1' 'fd_close' */
u32 Z_wasi_snapshot_preview1Z_fd_close(struct Z_wasi_snapshot_preview1_instance_t*, u32);
/* import: 'wasi_snapshot_preview1' 'fd_fdstat_get' */
u32 Z_wasi_snapshot_preview1Z_fd_fdstat_get(struct Z_wasi_snapshot_preview1_instance_t*, u32, u32);
/* import: 'wasi_snapshot_preview1' 'fd_seek' */
u32 Z_wasi_snapshot_preview1Z_fd_seek(struct Z_wasi_snapshot_preview1_instance_t*, u32, u64, u32, u32);
/* import: 'wasi_snapshot_preview1' 'fd_write' */
u32 Z_wasi_snapshot_preview1Z_fd_write(struct Z_wasi_snapshot_preview1_instance_t*, u32, u32, u32, u32);
/* import: 'wasi_snapshot_preview1' 'proc_exit' */
void Z_wasi_snapshot_preview1Z_proc_exit(struct Z_wasi_snapshot_preview1_instance_t*, u32);

/* export: 'memory' */
wasm_rt_memory_t* Z_basic_wasi_progZ_memory(Z_basic_wasi_prog_instance_t* instance);

/* export: '_start' */
void Z_basic_wasi_progZ__start(Z_basic_wasi_prog_instance_t*);

#ifdef __cplusplus
}
#endif

#endif  /* BASIC_WASI_PROG_H_GENERATED_ */
