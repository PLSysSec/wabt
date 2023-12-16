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

// This file is used as a template to generate code for both regular memories
// and shared memories.

#include "wasm-rt-impl.h"

// "_TEMPLATE_ REPLACE" (without the space in between) will be replaced with ""
// or "_shared" throughout this file by the build to generate two files --- one
// for regular memory, one for shared memories.
#define WASM_RT_MEM_OPS_shared

// Shared memory operations are defined only if we have C11
#if defined(WASM_RT_MEM_OPS) || \
    (defined(WASM_RT_MEM_OPS_shared) && defined(WASM_RT_C11_AVAILABLE))

#include <assert.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#endif

#ifdef WASM_RT_MEM_OPS
// Memory operations on wasm_rt_memory_t
#define MEMORY_CELL_TYPE uint8_t*
#define MEMORY_LOCK_VAR_INIT(name)
#define MEMORY_LOCK_AQUIRE(name)
#define MEMORY_LOCK_RELEASE(name)
#else
// Memory operations on wasm_rt_shared_memory_t
#define MEMORY_CELL_TYPE _Atomic volatile uint8_t*

#if WASM_RT_USE_C11THREADS
#define MEMORY_LOCK_VAR_INIT(name)                    \
  if (mtx_init(&(name), mtx_plain) != thrd_success) { \
    fprintf(stderr, "Lock init failed\n");            \
    abort();                                          \
  }
#define MEMORY_LOCK_AQUIRE(name)              \
  if (mtx_lock(&(name)) != thrd_success) {    \
    fprintf(stderr, "Lock acquire failed\n"); \
    abort();                                  \
  }
#define MEMORY_LOCK_RELEASE(name)             \
  if (mtx_unlock(&(name)) != thrd_success) {  \
    fprintf(stderr, "Lock release failed\n"); \
    abort();                                  \
  }
#elif WASM_RT_USE_PTHREADS
#define MEMORY_LOCK_VAR_INIT(name)              \
  if (pthread_mutex_init(&(name), NULL) != 0) { \
    fprintf(stderr, "Lock init failed\n");      \
    abort();                                    \
  }
#define MEMORY_LOCK_AQUIRE(name)              \
  if (pthread_mutex_lock(&(name)) != 0) {     \
    fprintf(stderr, "Lock acquire failed\n"); \
    abort();                                  \
  }
#define MEMORY_LOCK_RELEASE(name)             \
  if (pthread_mutex_unlock(&(name)) != 0) {   \
    fprintf(stderr, "Lock release failed\n"); \
    abort();                                  \
  }
#elif WASM_RT_USE_CRITICALSECTION
#define MEMORY_LOCK_VAR_INIT(name) InitializeCriticalSection(&(name))
#define MEMORY_LOCK_AQUIRE(name) EnterCriticalSection(&(name))
#define MEMORY_LOCK_RELEASE(name) LeaveCriticalSection(&(name))
#endif

#endif

#define PAGE_SIZE 65536

#ifdef WASM_RT_GROW_FAILED_HANDLER
extern void WASM_RT_GROW_FAILED_HANDLER();
#endif

#ifdef _WIN32
static void* os_mmap(size_t size) {
  void* ret = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
  return ret;
}

static int os_munmap(void* addr, size_t size) {
  // Windows can only unmap the whole mapping
  (void)size; /* unused */
  BOOL succeeded = VirtualFree(addr, 0, MEM_RELEASE);
  return succeeded ? 0 : -1;
}

static int os_mprotect(void* addr, size_t size) {
  if (size == 0) {
    return 0;
  }
  void* ret = VirtualAlloc(addr, size, MEM_COMMIT, PAGE_READWRITE);
  if (ret == addr) {
    return 0;
  }
  VirtualFree(addr, 0, MEM_RELEASE);
  return -1;
}

static void os_print_last_error(const char* msg) {
  DWORD errorMessageID = GetLastError();
  if (errorMessageID != 0) {
    LPSTR messageBuffer = 0;
    // The api creates the buffer that holds the message
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer, 0, NULL);
    (void)size;
    printf("%s. %s\n", msg, messageBuffer);
    LocalFree(messageBuffer);
  } else {
    printf("%s. No error code.\n", msg);
  }
}

#else
static void* os_mmap(size_t size) {
  int map_prot = PROT_NONE;
  int map_flags = MAP_ANONYMOUS | MAP_PRIVATE;
  uint8_t* addr = mmap(NULL, size, map_prot, map_flags, -1, 0);
  if (addr == MAP_FAILED)
    return NULL;
  return addr;
}

static int os_munmap(void* addr, size_t size) {
  return munmap(addr, size);
}

static int os_mprotect(void* addr, size_t size) {
  return mprotect(addr, size, PROT_READ | PROT_WRITE);
}

static void os_print_last_error(const char* msg) {
  perror(msg);
}

#endif

#if WASM_RT_USE_MMAP

static uint64_t get_alloc_size_for_mmap(
    wasm_rt_shared_memory_t* memory) {
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

void wasm_rt_allocate_memory_shared(
    wasm_rt_shared_memory_t* memory,
    uint64_t initial_pages,
    uint64_t max_pages,
    bool is64) {
  uint64_t byte_length = initial_pages * PAGE_SIZE;
  memory->size = byte_length;
  memory->pages = initial_pages;
  memory->max_pages = max_pages;
  memory->is64 = is64;
  MEMORY_LOCK_VAR_INIT(memory->mem_lock);

#if WASM_RT_USE_MMAP
  const uint64_t mmap_size = get_alloc_size_for_mmap(memory);
  void* addr = os_mmap(mmap_size);
  if (!addr) {
    os_print_last_error("os_mmap failed.");
    abort();
  }
  int ret = os_mprotect(addr, byte_length);
  if (ret != 0) {
    os_print_last_error("os_mprotect failed.");
    abort();
  }
  memory->data = addr;
#else
  memory->data = calloc(byte_length, 1);
#endif
}

static uint64_t grow_memory_impl(wasm_rt_shared_memory_t* memory,
                                 uint64_t delta) {
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
  MEMORY_CELL_TYPE new_data = memory->data;
  int ret = os_mprotect((void*)(new_data + old_size), delta_size);
  if (ret != 0) {
    return (uint64_t)-1;
  }
#else
  MEMORY_CELL_TYPE new_data = realloc((void*)memory->data, new_size);
  if (new_data == NULL) {
    return (uint64_t)-1;
  }
#if !WABT_BIG_ENDIAN
  memset((void*)(new_data + old_size), 0, delta_size);
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

uint64_t wasm_rt_grow_memory_shared(
    wasm_rt_shared_memory_t* memory,
    uint64_t delta) {
  MEMORY_LOCK_AQUIRE(memory->mem_lock);
  uint64_t ret = grow_memory_impl(memory, delta);
  MEMORY_LOCK_RELEASE(memory->mem_lock);
#ifdef WASM_RT_GROW_FAILED_HANDLER
  if (ret == -1) {
    WASM_RT_GROW_FAILED_HANDLER();
  }
#endif
  return ret;
}

void wasm_rt_free_memory_shared(
    wasm_rt_shared_memory_t* memory) {
#if WASM_RT_USE_MMAP
  const uint64_t mmap_size = get_alloc_size_for_mmap(memory);
  os_munmap((void*)memory->data, mmap_size);  // ignore error
#else
  free((void*)memory->data);
#endif
}

#undef PAGE_SIZE
#undef MEMORY_LOCK_RELEASE
#undef MEMORY_LOCK_AQUIRE
#undef MEMORY_LOCK_VAR_INIT
#undef MEMORY_CELL_TYPE
#undef WASM_RT_MEM_OPS_shared

#endif
