// Copyright 2025 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_PAINT_DISPLAY_ITEM_CLIENT_TYPES_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_PAINT_DISPLAY_ITEM_CLIENT_TYPES_H_

#include <stdint.h>

namespace blink {

#if defined(__CHERI_PURE_CAPABILITY__)
#include <stddef.h>

typedef size_t DisplayItemClientId;
#else
typedef uintptr_t DisplayItemClientId;
#endif
inline constexpr DisplayItemClientId kInvalidDisplayItemClientId = 0u;

enum class RasterEffectOutset : uint8_t {
  kNone,
  kHalfPixel,
  kWholePixel,
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_PAINT_DISPLAY_ITEM_CLIENT_TYPES_H_
