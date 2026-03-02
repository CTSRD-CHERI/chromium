#include <limits>

#include "partition_alloc/partition_alloc_base/compiler_specific.h"
#include "partition_alloc/partition_alloc_base/numerics/checked_math.h"
#include "partition_alloc/shim/allocator_shim.h"
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

void* LibcMalloc(size_t size, void* context) {
  // Cannot force glibc's malloc() to crash when a large size is requested, do
  // it in the shim instead.
  if (size >= kMaxAllowedSize) [[unlikely]] {
    base::TerminateBecauseOutOfMemory(size);
  }

  return __malloc(size);
}

void* LibcUncheckedMalloc(size_t size, void* context) {
  if (size >= kMaxAllowedSize) [[unlikely]] {
    return nullptr;
  }

  return __malloc(size);
}

void* LibcCalloc(size_t n, size_t size, void* context) {
  const auto total = partition_alloc::internal::base::CheckMul(n, size);
  if (!total.IsValid() || total.ValueOrDie() >= kMaxAllowedSize) [[unlikely]] {
    base::TerminateBecauseOutOfMemory(size * n);
  }

  return __calloc(n, size);
}

void* LibcRealloc(void* address, size_t size, void* context) {
  if (size >= kMaxAllowedSize) [[unlikely]] {
    base::TerminateBecauseOutOfMemory(size);
  }

  return __realloc(address, size);
}

void* LibcMemalign(size_t alignment, size_t size, void* context) {
  if (size >= kMaxAllowedSize) [[unlikely]] {
    base::TerminateBecauseOutOfMemory(size);
  }

  return __aligned_alloc(alignment, size);
}

void LibcFree(void* address, void* context) {
  __free(address);
}

size_t LibcGetSizeEstimate(void* address, void* context) {
  return __malloc_usable_size(address);
}

}  // namespace

const AllocatorDispatch AllocatorDispatch::default_dispatch = {
    &LibcMalloc,          /* alloc_function */
    &LibcUncheckedMalloc, /* alloc_unchecked_function */
    &LibcCalloc,          /* alloc_zero_initialized_function */
    nullptr,              /* alloc_zero_initialized_unchecked_function */
    &LibcMemalign,        /* alloc_aligned_function */
    &LibcRealloc,         /* realloc_function */
    nullptr,              /* realloc_unchecked_function */
    &LibcFree,            /* free_function */
    nullptr,              /* free_with_size_function */
    nullptr,              /* free_with_alignment_function */
    nullptr,              /* free_with_size_and_alignment_function */
    &LibcGetSizeEstimate, /* get_size_estimate_function */
    nullptr,               /* good_size_function */
    nullptr,               /* claimed_address */
    nullptr,               /* batch_malloc_function */
    nullptr,               /* batch_free_function */
    nullptr,               /* try_free_default_function */
    nullptr,               /* aligned_malloc_function */
    nullptr,               /* aligned_malloc_unchecked_function */
    nullptr,               /* aligned_realloc_function */
    nullptr,               /* aligned_realloc_unchecked_function */
    nullptr,               /* aligned_free_function */
    nullptr,               /* next */
};
