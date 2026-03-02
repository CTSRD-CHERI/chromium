#include <limits>

#include "base/allocator/partition_allocator/partition_alloc_base/compiler_specific.h"
#include "base/allocator/partition_allocator/partition_alloc_base/numerics/checked_math.h"
#include "base/allocator/partition_allocator/shim/allocator_shim.h"
#include "base/process/memory.h"

#include <dlfcn.h>
#include <malloc.h>

// This translation unit defines a default dispatch for the allocator shim which
// routes allocations to libc functions.
// The code here is strongly inspired from tcmalloc's libc_override_glibc.h.

extern "C" {
void* __malloc(size_t size);
void* __calloc(size_t n, size_t size);
void* __realloc(void* address, size_t size);
void __free(void* ptr);
size_t __malloc_usable_size(const void* ptr);
void* __aligned_alloc(size_t alignment, size_t size);
}  // extern "C"

namespace {

using allocator_shim::AllocatorDispatch;

// Strictly speaking, it would make more sense to not subtract amything, but
// other shims limit to something lower than INT_MAX (which is 0x7FFFFFFF on
// most platforms), and tests expect that.
constexpr size_t kMaxAllowedSize = std::numeric_limits<int>::max() - (1 << 12);

void* LibcMalloc(const AllocatorDispatch*, size_t size, void* context) {
  // Cannot force glibc's malloc() to crash when a large size is requested, do
  // it in the shim instead.
  if (PA_UNLIKELY(size >= kMaxAllowedSize)) {
    base::TerminateBecauseOutOfMemory(size);
  }

  return __malloc(size);
}

void* LibcUncheckedMalloc(const AllocatorDispatch*,
                           size_t size,
                           void* context) {
  if (PA_UNLIKELY(size >= kMaxAllowedSize)) {
    return nullptr;
  }

  return __malloc(size);
}

void* LibcCalloc(const AllocatorDispatch*,
                  size_t n,
                  size_t size,
                  void* context) {
  const auto total = partition_alloc::internal::base::CheckMul(n, size);
  if (PA_UNLIKELY(!total.IsValid() || total.ValueOrDie() >= kMaxAllowedSize)) {
    base::TerminateBecauseOutOfMemory(size * n);
  }

  return __calloc(n, size);
}

void* LibcRealloc(const AllocatorDispatch*,
                   void* address,
                   size_t size,
                   void* context) {
  if (PA_UNLIKELY(size >= kMaxAllowedSize)) {
    base::TerminateBecauseOutOfMemory(size);
  }

  return __realloc(address, size);
}

void* LibcMemalign(const AllocatorDispatch*,
                    size_t alignment,
                    size_t size,
                    void* context) {
  if (PA_UNLIKELY(size >= kMaxAllowedSize)) {
    base::TerminateBecauseOutOfMemory(size);
  }

  return __aligned_alloc(alignment, size);
}

void LibcFree(const AllocatorDispatch*, void* address, void* context) {
  __free(address);
}

size_t LibcGetSizeEstimate(const AllocatorDispatch*,
                            void* address,
                            void* context) {
  return __malloc_usable_size(address);
}

}  // namespace

const AllocatorDispatch AllocatorDispatch::default_dispatch = {
    &LibcMalloc,          /* alloc_function */
    &LibcUncheckedMalloc, /* alloc_unchecked_function */
    &LibcCalloc,          /* alloc_zero_initialized_function */
    &LibcMemalign,        /* alloc_aligned_function */
    &LibcRealloc,         /* realloc_function */
    &LibcFree,            /* free_function */
    &LibcGetSizeEstimate, /* get_size_estimate_function */
    nullptr,               /* claimed_address */
    nullptr,               /* batch_malloc_function */
    nullptr,               /* batch_free_function */
    nullptr,               /* free_definite_size_function */
    nullptr,               /* try_free_default_function */
    nullptr,               /* aligned_malloc_function */
    nullptr,               /* aligned_realloc_function */
    nullptr,               /* aligned_free_function */
    nullptr,               /* next */
};
