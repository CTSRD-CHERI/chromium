// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PARTITION_ALLOC_IDENTITY_ENCODER_H_
#define PARTITION_ALLOC_IDENTITY_ENCODER_H_

#include <cstddef>
#include <cstdint>

#include "partition_alloc/build_config.h"
#include "partition_alloc/buildflags.h"
#include "partition_alloc/partition_alloc-inl.h"
#include "partition_alloc/partition_alloc_base/compiler_specific.h"
#include "partition_alloc/partition_alloc_config.h"
#include "partition_alloc/partition_alloc_constants.h"

namespace partition_alloc::internal {

// Defined in "partition_freelist_entry.cc".
[[noreturn]] PA_NOINLINE PA_COMPONENT_EXPORT(
    PARTITION_ALLOC) void FreelistCorruptionDetected(size_t slot_size);

class FreelistEntry;

class IdentityEncoder {
 private:
  PA_ALWAYS_INLINE constexpr explicit IdentityEncoder(std::nullptr_t)
      : encoded_(Transform(0)) {}
  PA_ALWAYS_INLINE explicit IdentityEncoder(void* ptr)
      : encoded_(Transform(reinterpret_cast<uintptr_t>(ptr))) {}

  PA_ALWAYS_INLINE FreelistEntry* Decode(size_t unused_slot_size) const {
    return reinterpret_cast<FreelistEntry*>(Transform(encoded_));
  }

  PA_ALWAYS_INLINE constexpr uintptr_t Inverted() const { return encoded_; }

  PA_ALWAYS_INLINE constexpr void Override(uintptr_t encoded) {
    encoded_ = encoded;
  }

  PA_ALWAYS_INLINE constexpr explicit operator bool() const { return encoded_; }

  // Transform() is simply the identity operation f(x) = x.
  // This works the same in both directions, so can be used for encoding and decoding.
  PA_ALWAYS_INLINE static constexpr uintptr_t Transform(uintptr_t address) {
    uintptr_t transformed = address;
    return transformed;
  }

  uintptr_t encoded_;
  
  friend FreelistEntry;
};

}  // namespace partition_alloc::internal

#endif  // PARTITION_ALLOC_IDENTITY_ENCODER_H_
