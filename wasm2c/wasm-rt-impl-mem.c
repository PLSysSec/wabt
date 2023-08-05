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

// This file is not meant to be used directly. Rather is is included by
// wasm-rt-impl.c

#include <wasm-rt-impl.h>

// By default this file implements the memory operations on wasm_rt_memory_t.
// However wasm-rt-impl.c may use this file to implement operations on
// wasm_rt_shared_memory_t as well by setting
//
// #define MEMORY_TYPE_NAME wasm_rt_shared_memory_t
// #define MEMORY_DATA_TYPE volatile uint8_t*
// #define HELPERS_NAME(name) name##_shared
// #define RT_ALLOCATE_MEMORY wasm_rt_allocate_shared_memory
// #define RT_GROW_MEMORY wasm_rt_grow_shared_memory
// #define RT_FREE_MEMORY wasm_rt_free_shared_memory

#ifndef MEMORY_TYPE_NAME

#define MEMORY_TYPE_NAME wasm_rt_memory_t
#define MEMORY_DATA_TYPE uint8_t*
#define HELPERS_NAME(name) name
#define RT_ALLOCATE_MEMORY wasm_rt_allocate_memory
#define RT_GROW_MEMORY wasm_rt_grow_memory
#define RT_FREE_MEMORY wasm_rt_free_memory

#endif

#define PAGE_SIZE 65536

#if WASM_RT_USE_MMAP

static uint64_t HELPERS_NAME(get_memory_allocation_size_for_mmap)(MEMORY_TYPE_NAME* memory) {
  assert(!memory->is64 &&
         "memory64 is not yet compatible with WASM_RT_USE_MMAP");
#if WASM_RT_MEMCHECK_GUARD_PAGES
  /* Reserve 8GiB. */
  const uint64_t max_size = 0x200000000ul;
  return max_size;
#else
  if (memory->max_pages != 0) {
    const uint64_t max_size = memory->max_pages * PAGE_SIZE;
    return max_size;
  }

  /* Reserve 4GiB. */
  const uint64_t max_size = 0x100000000ul;
  return max_size;
#endif
}

#endif

void RT_ALLOCATE_MEMORY(MEMORY_TYPE_NAME* memory,
                             uint64_t initial_pages,
                             uint64_t max_pages,
                             bool is64) {
  uint64_t byte_length = initial_pages * PAGE_SIZE;
  memory->size = byte_length;
  memory->pages = initial_pages;
  memory->max_pages = max_pages;
  memory->is64 = is64;

#if WASM_RT_USE_MMAP
  const uint64_t mmap_size = HELPERS_NAME(get_memory_allocation_size_for_mmap)(memory);
  void* addr = os_mmap(mmap_size);
  if (!addr) {
    os_print_last_error("os_mmap failed.");
    abort();
  }
  int ret = os_mprotect((void*) addr, byte_length);
  if (ret != 0) {
    os_print_last_error("os_mprotect failed.");
    abort();
  }
  memory->data = addr;
#else
  memory->data = calloc(byte_length, 1);
#endif
}

static uint64_t HELPERS_NAME(grow_memory_impl)(MEMORY_TYPE_NAME* memory, uint64_t delta) {
  uint64_t old_pages = memory->pages;
  uint64_t new_pages = memory->pages + delta;
  if (new_pages == 0) {
    return 0;
  }
  if (new_pages < old_pages || new_pages > memory->max_pages) {
    return (uint64_t)-1;
  }
  uint64_t old_size = old_pages * PAGE_SIZE;
  uint64_t new_size = new_pages * PAGE_SIZE;
  uint64_t delta_size = delta * PAGE_SIZE;
#if WASM_RT_USE_MMAP
  MEMORY_DATA_TYPE new_data = memory->data;
  int ret = os_mprotect((void*) (new_data + old_size), delta_size);
  if (ret != 0) {
    return (uint64_t)-1;
  }
#else
  MEMORY_DATA_TYPE new_data = realloc(memory->data, new_size);
  if (new_data == NULL) {
    return (uint64_t)-1;
  }
#if !WABT_BIG_ENDIAN
  memset(new_data + old_size, 0, delta_size);
#endif
#endif
#if WABT_BIG_ENDIAN
  memmove(new_data + new_size - old_size, new_data, old_size);
  memset(new_data, 0, delta_size);
#endif
  memory->pages = new_pages;
  memory->size = new_size;
  memory->data = new_data;
  return old_pages;
}

uint64_t RT_GROW_MEMORY(MEMORY_TYPE_NAME* memory, uint64_t delta) {
  uint64_t ret = HELPERS_NAME(grow_memory_impl)(memory, delta);
#ifdef WASM_RT_GROW_FAILED_HANDLER
  if (ret == -1) {
    WASM_RT_GROW_FAILED_HANDLER();
  }
#endif
  return ret;
}

void RT_FREE_MEMORY(MEMORY_TYPE_NAME* memory) {
#if WASM_RT_USE_MMAP
  const uint64_t mmap_size = HELPERS_NAME(get_memory_allocation_size_for_mmap)(memory);
  os_munmap((void*) memory->data, mmap_size);  // ignore error
#else
  free(memory->data);
#endif
}

#undef PAGE_SIZE
#undef MEMORY_TYPE_NAME
#undef MEMORY_DATA_TYPE
#undef HELPERS_NAME
#undef RT_ALLOCATE_MEMORY
#undef RT_GROW_MEMORY
#undef RT_FREE_MEMORY
