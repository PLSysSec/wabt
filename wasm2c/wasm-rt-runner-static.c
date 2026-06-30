// Note the actual header of the generated Wasm should be included here through
// a command line flag. This can be done using for example through the below.
//
// -include hello-wasi-wasm.h

// If the above was successfully done, the macro WASM_RT_CORE_TYPES_DEFINED
// should be available since it is part of a wasm2c generated header. Check for this.

#ifndef WASM_RT_CORE_TYPES_DEFINED
#error "Didn't include the wasm2c generated header during compilation. Add this using the -include command line flag"
#endif

#ifndef CURR_WASM2C_MODULE_NAME
#error "No Wasm Module Name defined during compilation or in header. Specify this with -DCURR_WASM2C_MODULE_NAME=NAME_GOES_HERE"
#endif

#define CONCAT2(a, b, c) a##b##c
#define CONCAT(a, b, c) CONCAT2(a, b, c)

#define WASM_INSTANCE_NAME CONCAT(w2c_, CURR_WASM2C_MODULE_NAME, )
#define WASM_MODULE_INSTANTIATE CONCAT(wasm2c_, CURR_WASM2C_MODULE_NAME, _instantiate)
#define WASM_MODULE_START CONCAT(w2c_, CURR_WASM2C_MODULE_NAME, _0x5Fstart)
#define WASM_MODULE_FREE CONCAT(wasm2c_, CURR_WASM2C_MODULE_NAME, _free)

#include <stdio.h>
#include <stdlib.h>

#include "wasm-rt-uvwasi-adapter.h"
#include "uvwasi.h"

int main(int argc, const char** argv) {
  WASM_INSTANCE_NAME instance = {0};
  struct w2c_wasi__snapshot__preview1 wasi = {0};
  uvwasi_t uvwasi = {0};
  uvwasi_options_t init_options = {0};
  uvwasi_preopen_t preopens[3] = {0};

  init_options.in = 0;
  init_options.out = 1;
  init_options.err = 2;
  init_options.fd_table_size = 10;

  extern const char** environ;
  init_options.argc = argc;
  init_options.argv = argv;
  init_options.envp = (const char**)environ;

  preopens[0].mapped_path = "/";
  preopens[0].real_path = "/";
  preopens[1].mapped_path = "/tmp";
  preopens[1].real_path = "/tmp";
  preopens[2].mapped_path = ".";
  preopens[2].real_path = ".";
  init_options.preopenc = 3;
  init_options.preopens = preopens;
  init_options.allocator = NULL;

  wasm_rt_init();
  uvwasi_errno_t ret = uvwasi_init(&uvwasi, &init_options);
  if (ret != UVWASI_ESUCCESS) {
    fprintf(stderr, "uvwasi_init failed with error %u\n", ret);
    return 1;
  }

  wasm2c_uvwasi_link(&wasi, &instance.w2c_memory, &uvwasi);
  WASM_MODULE_INSTANTIATE(&instance, &wasi);
  WASM_MODULE_START(&instance);
  WASM_MODULE_FREE(&instance);

  uvwasi_destroy(&uvwasi);
  wasm_rt_free();

  return 0;
}
