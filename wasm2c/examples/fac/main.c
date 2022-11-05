#include <stdio.h>
#include <stdlib.h>

#include "fac.h"

int main(int argc, char** argv) {
  /* Make sure there is at least one command-line argument. */
  if (argc < 2) {
    printf("Invalid argument. Expected '%s NUMBER'\n", argv[0]);
    return 1;
  }

  /* Convert the argument from a string to an int. We'll implicitly cast the int
  to a `u32`, which is what `fac` expects. */
  u32 x = atoi(argv[1]);

  /* Initialize the Wasm runtime. */
  wasm_rt_init();

  /* Initialize the `fac` module (this registers the module's function types in
   * a global data structure) */
  Z_fac_init_module();

  /* Declare an instance of the `fac` module. */
  Z_fac_instance_t instance;

  wasm_rt_thread_state* thread_state = wasm_rt_thread_init();

  /* Construct the module instance. */
  Z_fac_instantiate(thread_state, &instance);

  /* Call `fac`, using the mangled name. */
  u32 result = Z_facZ_fac(thread_state, &instance, x);

  /* Print the result. */
  printf("fac(%u) -> %u\n", x, result);

  wasm_rt_thread_free(thread_state);

  /* Free the fac module instance. */
  Z_fac_free(&instance);

  /* Free the fac module. */
  Z_fac_free_module();

  /* Free the Wasm runtime state. */
  wasm_rt_free();

  return 0;
}
