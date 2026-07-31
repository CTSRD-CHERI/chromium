// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This header defines cross-platform NetToHostXX() / HostToNextXX() functions
// equivalent to the traditional ntohX() and htonX() functions.
// Use the functions defined here rather than using the platform-specific
// functions directly.

#ifndef BASE_SYS_BYTEORDER_H_
#define BASE_SYS_BYTEORDER_H_

#include <stdint.h>

#include <bit>

#include "build/build_config.h"

#if defined(COMPILER_MSVC)
#include <stdlib.h>
#endif

namespace base {

#if !__cpp_lib_byteswap
#if defined(COMPILER_MSVC) && !defined(__clang__)
// TODO(pkasting): See
// https://developercommunity.visualstudio.com/t/Mark-some-built-in-functions-as-constexp/362558
// https://developercommunity.visualstudio.com/t/constexpr-byte-swapping-optimization/983963
#define BASE_BYTESWAPS_CONSTEXPR
#else
#define BASE_BYTESWAPS_CONSTEXPR constexpr
#endif

// Returns a value with all bytes in |x| swapped, i.e. reverses the endianness.
inline BASE_BYTESWAPS_CONSTEXPR uint16_t ByteSwap(uint16_t x) {
#if defined(COMPILER_MSVC) && !defined(__clang__)
  return _byteswap_ushort(x);
#else
  return __builtin_bswap16(x);
#endif
}

inline BASE_BYTESWAPS_CONSTEXPR uint32_t ByteSwap(uint32_t x) {
#if defined(COMPILER_MSVC) && !defined(__clang__)
  return _byteswap_ulong(x);
#else
  return __builtin_bswap32(x);
#endif
}

inline BASE_BYTESWAPS_CONSTEXPR uint64_t ByteSwap(uint64_t x) {
  // Per build/build_config.h, clang masquerades as MSVC on Windows. If we are
  // actually using clang, we can rely on the builtin.
  //
  // This matters in practice, because on x86(_64), this is a single "bswap"
  // instruction. MSVC correctly replaces the call with an inlined bswap at /O2
  // as of 2021, but clang as we use it in Chromium doesn't, keeping a function
  // call for a single instruction.
#if defined(COMPILER_MSVC) && !defined(__clang__)
  return _byteswap_uint64(x);
#else
  return __builtin_bswap64(x);
#endif
}
#endif // !__cpp_lib_byteswap

// Converts the bytes in |x| from network to host order (endianness), and
// returns the result.
inline constexpr uint16_t NetToHost16(uint16_t x) {
#if defined(ARCH_CPU_LITTLE_ENDIAN)
#if __cpp_lib_byteswap
  return std::byteswap(x);
#else
  return ByteSwap(x);
#endif
#else
  return x;
#endif
}
inline constexpr uint32_t NetToHost32(uint32_t x) {
#if defined(ARCH_CPU_LITTLE_ENDIAN)
#if __cpp_lib_byteswap
  return std::byteswap(x);
#else
  return ByteSwap(x);
#endif
#else
  return x;
#endif
}
inline constexpr uint64_t NetToHost64(uint64_t x) {
#if defined(ARCH_CPU_LITTLE_ENDIAN)
#if __cpp_lib_byteswap
  return std::byteswap(x);
#else
  return ByteSwap(x);
#endif
#else
  return x;
#endif
}

// Converts the bytes in |x| from host to network order (endianness), and
// returns the result.
inline constexpr uint16_t HostToNet16(uint16_t x) {
#if defined(ARCH_CPU_LITTLE_ENDIAN)
#if __cpp_lib_byteswap
  return std::byteswap(x);
#else
  return ByteSwap(x);
#endif
#else
  return x;
#endif
}
inline constexpr uint32_t HostToNet32(uint32_t x) {
#if defined(ARCH_CPU_LITTLE_ENDIAN)
#if __cpp_lib_byteswap
  return std::byteswap(x);
#else
  return ByteSwap(x);
#endif
#else
  return x;
#endif
}
inline constexpr uint64_t HostToNet64(uint64_t x) {
#if defined(ARCH_CPU_LITTLE_ENDIAN)
#if __cpp_lib_byteswap
  return std::byteswap(x);
#else
  return ByteSwap(x);
#endif
#else
  return x;
#endif
}

}  // namespace base

#endif  // BASE_SYS_BYTEORDER_H_
