// Its purpose is to preempt the BSD Libc symbols for malloc/new so they call the
// shim layer entry points.

#ifdef BASE_ALLOCATOR_PARTITION_ALLOCATOR_SHIM_ALLOCATOR_SHIM_OVERRIDE_BSD_LIBC_SYMBOLS_H_
#error This header is meant to be included only once by allocator_shim.cc
#endif

#ifndef BASE_ALLOCATOR_PARTITION_ALLOCATOR_SHIM_ALLOCATOR_SHIM_OVERRIDE_BSD_LIBC_SYMBOLS_H_
#define BASE_ALLOCATOR_PARTITION_ALLOCATOR_SHIM_ALLOCATOR_SHIM_OVERRIDE_BSD_LIBC_SYMBOLS_H_

#include "build/build_config.h"

#include <malloc.h>

#include "base/allocator/partition_allocator/shim/allocator_shim_internals.h"

extern "C" {

SHIM_ALWAYS_EXPORT void* malloc(size_t size) {
  return ShimMalloc(size, nullptr);
}

SHIM_ALWAYS_EXPORT void free(void* ptr) {
  ShimFree(ptr, nullptr);
}

SHIM_ALWAYS_EXPORT void* realloc(void* ptr, size_t size) {
  return ShimRealloc(ptr, size, nullptr);
}

SHIM_ALWAYS_EXPORT void* calloc(size_t n, size_t size) {
  return ShimCalloc(n, size, nullptr);
}

SHIM_ALWAYS_EXPORT void cfree(void* ptr) {
  ShimFree(ptr, nullptr);
}

SHIM_ALWAYS_EXPORT void* memalign(size_t align, size_t s) {
  return ShimMemalign(align, s, nullptr);
}

SHIM_ALWAYS_EXPORT void* aligned_alloc(size_t align, size_t s) {
  return ShimMemalign(align, s, nullptr);
}

SHIM_ALWAYS_EXPORT void* valloc(size_t size) {
  return ShimValloc(size, nullptr);
}

SHIM_ALWAYS_EXPORT void* pvalloc(size_t size) {
  return ShimPvalloc(size);
}

SHIM_ALWAYS_EXPORT int posix_memalign(void** r, size_t a, size_t s) {
  return ShimPosixMemalign(r, a, s);
}

SHIM_ALWAYS_EXPORT size_t malloc_size(const void* address) {
  return ShimGetSizeEstimate(address, nullptr);
}

SHIM_ALWAYS_EXPORT size_t malloc_usable_size(const void* address) {
  return ShimGetSizeEstimate(address, nullptr);
}

// The default dispatch translation unit has to define also the following
// symbols (unless they are ultimately routed to the system symbols):
//   void malloc_stats(void);
//   int mallopt(int, int);
//   struct mallinfo mallinfo(void);

}  // extern "C"

#endif  // BASE_ALLOCATOR_PARTITION_ALLOCATOR_SHIM_ALLOCATOR_SHIM_OVERRIDE_BSD_LIBC_SYMBOLS_H_
