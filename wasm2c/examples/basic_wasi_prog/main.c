#include <stdio.h>
#include <stdlib.h>

#include "basic_wasi_prog.h"

#define RANGE_CHECK(mem, offset, len)               \
  if (UNLIKELY(offset + (uint64_t)len > mem->size)) \
    abort();

#define MEMCHECK(mem, a, t) RANGE_CHECK(mem, a, sizeof(t))

#define DEFINE_LOAD(name, t1, t2, t3)                      \
  static inline t3 name(wasm_rt_memory_t* mem, u64 addr) { \
    MEMCHECK(mem, addr, t1);                               \
    t1 result;                                             \
    memcpy(&result, &mem->data[addr], sizeof(t1));         \
    asm("" ::"r"(result));                                 \
    return (t3)(t2)result;                                 \
  }

#define DEFINE_STORE(name, t1, t2)                                     \
  static inline void name(wasm_rt_memory_t* mem, u64 addr, t2 value) { \
    MEMCHECK(mem, addr, t1);                                           \
    t1 wrapped = (t1)value;                                            \
    memcpy(&mem->data[addr], &wrapped, sizeof(t1));                    \
  }

DEFINE_LOAD(i32_load, u32, u32, u32)
DEFINE_STORE(i32_store, u32, u32)

typedef struct Z_wasi_snapshot_preview1_instance_t
{
  Z_basic_wasi_prog_instance_t* instance;
} Z_wasi_snapshot_preview1_instance_t;

#define WASI_PERM_ERROR 63

/* import: 'wasi_snapshot_preview1' 'args_get' */
u32 Z_wasi_snapshot_preview1Z_args_get(struct Z_wasi_snapshot_preview1_instance_t* wasi_data, u32 argv, u32 argv_buf) {
  return 0;
}
/* import: 'wasi_snapshot_preview1' 'args_sizes_get' */
u32 Z_wasi_snapshot_preview1Z_args_sizes_get(struct Z_wasi_snapshot_preview1_instance_t* wasi_data, u32 pargc, u32 pargv_buf_size) {
  wasm_rt_memory_t* mem = &(wasi_data->instance->w2c_memory);
  i32_store(mem, pargc, 0 /* argc */);
  i32_store(mem, pargv_buf_size, 0 /* argv_buf_size*/);
  return 0;
}
/* import: 'wasi_snapshot_preview1' 'fd_close' */
u32 Z_wasi_snapshot_preview1Z_fd_close(struct Z_wasi_snapshot_preview1_instance_t* wasi_data, u32 a) {
  return WASI_PERM_ERROR;
}
/* import: 'wasi_snapshot_preview1' 'fd_fdstat_get' */
u32 Z_wasi_snapshot_preview1Z_fd_fdstat_get(struct Z_wasi_snapshot_preview1_instance_t* wasi_data, u32 a, u32 b) {
  return WASI_PERM_ERROR;
}
/* import: 'wasi_snapshot_preview1' 'fd_seek' */
u32 Z_wasi_snapshot_preview1Z_fd_seek(struct Z_wasi_snapshot_preview1_instance_t* wasi_data, u32 a, u64 b, u32 c, u32 d) {
  return WASI_PERM_ERROR;
}
/* import: 'wasi_snapshot_preview1' 'fd_write' */
u32 Z_wasi_snapshot_preview1Z_fd_write(struct Z_wasi_snapshot_preview1_instance_t* wasi_data, u32 fd, u32 iov, u32 iovcnt, u32 pnum) {
  wasm_rt_memory_t* mem = &(wasi_data->instance->w2c_memory);
  u32 num = 0;
  for (u32 i = 0; i < iovcnt; i++) {

    u32 ptr = i32_load(mem, iov + i * 8);
    u32 len = i32_load(mem, iov + i * 8 + 4);

    RANGE_CHECK(mem, ptr, len);

    size_t result = -1;
    if (fd == 1 || fd == 2) {
      result = fwrite(&(mem->data[ptr]), 1, len, fd == 1? stdout : stderr);
    }

    if (result < 0 || result != len) {
      return WASI_PERM_ERROR;
    }
    num += len;
  }
  i32_store(mem, pnum, num);
  return 0;
}
/* import: 'wasi_snapshot_preview1' 'proc_exit' */
void Z_wasi_snapshot_preview1Z_proc_exit(struct Z_wasi_snapshot_preview1_instance_t* wasi_data, u32 a) {
  /* Free the basic_wasi_prog module. */
  Z_basic_wasi_prog_free(wasi_data->instance);

  /* Free the Wasm runtime state. */
  wasm_rt_free();

  exit(0);
}

int main(int argc, char** argv) {

  /* Initialize the Wasm runtime. */
  wasm_rt_init();

  /* Initialize the `basic_wasi_prog` module (this registers the module's function types in
   * a global data structure) */
  Z_basic_wasi_prog_init_module();

  /* Declare an instance of the `basic_wasi_prog` module. */
  Z_basic_wasi_prog_instance_t instance;

  Z_wasi_snapshot_preview1_instance_t wasi;
  wasi.instance = &instance;

  /* Construct the module instance. */
  Z_basic_wasi_prog_instantiate(&instance, &wasi);

  /* Call `basic_wasi_prog`, using the mangled name. */
  Z_basic_wasi_progZ__start(&instance);

  return 0;
}
