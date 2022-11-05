/*
 * Copyright 2018 WebAssembly Community Group participants
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef WASM_RT_IMPL_H_
#define WASM_RT_IMPL_H_

#include "wasm-rt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WASM_RT_MAX_EXCEPTION_SIZE 65536

/** An object that holds the per-thread Wasm runtime state for this wasm
 * runtime. */
struct wasm_rt_thread_state {
  uint32_t active_exception_tag;
  uint8_t active_exception[WASM_RT_MAX_EXCEPTION_SIZE];
  uint32_t active_exception_size;
  jmp_buf* unwind_target;
  /** A setjmp buffer used for handling traps. */
  jmp_buf trap_jmp_buf;
#if !WASM_RT_MEMCHECK_SIGNAL_HANDLER_POSIX
  /** Saved call stack depth that will be restored in case a trap occurs. */
  extern uint32_t saved_call_stack_depth;
#endif
};

extern _Thread_local wasm_rt_thread_state* wasm_rt_curr_thread_state;

#if WASM_RT_MEMCHECK_SIGNAL_HANDLER_POSIX
#define WASM_RT_LONGJMP(buf, val) siglongjmp(buf, val)
#else
#define WASM_RT_LONGJMP(buf, val) longjmp(buf, val)
#endif

#define WASM_RT_SET_CURRENT_THREAD_STATE(thread_state) \
  wasm_rt_curr_thread_state = thread_state

/**
 * Convenience macro to use before calling a wasm function. On first execution
 * it will return `WASM_RT_TRAP_NONE` (i.e. 0). If the function traps, it will
 * jump back and return the trap that occurred.
 *
 * ```
 *   wasm_rt_trap_t code = wasm_rt_impl_try();
 *   if (code != 0) {
 *     printf("A trap occurred with code: %d\n", code);
 *     ...
 *   }
 *
 *   // Call the potentially-trapping function.
 *   my_wasm_func();
 * ```
 */
#if WASM_RT_MEMCHECK_SIGNAL_HANDLER_POSIX
#define wasm_rt_impl_try(thread_state)                       \
  (WASM_RT_SET_CURRENT_THREAD_STATE(thread_state),           \
   wasm_rt_set_unwind_target(&(thread_state->trap_jmp_buf)), \
   WASM_RT_SETJMP(thread_state->trap_jmp_buf))
#else
#define wasm_rt_impl_try(thread_state)                              \
  (WASM_RT_SET_CURRENT_THREAD_STATE(thread_state),                  \
   thread_state->saved_call_stack_depth = wasm_rt_call_stack_depth, \
   wasm_rt_set_unwind_target(&(thread_state->trap_jmp_buf)),        \
   WASM_RT_SETJMP(thread_state->trap_jmp_buf))
#endif

#ifdef __cplusplus
}
#endif

#endif /* WASM_RT_IMPL_H_ */
