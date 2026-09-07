// Copyright 2026 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Compatibility shim for std::to_underlying<T> (C++23) for compilers/libc++
// that lack it.

#ifndef BASE_TO_UNDERLYING_H_
#define BASE_TO_UNDERLYING_H_

#include <type_traits>

namespace base {

template <typename E>
constexpr auto to_underlying(E e) noexcept {
#if __cpp_lib_to_underlying
    return std::to_underlying<E>(e);
#else
    return static_cast<std::underlying_type_t<E>>(e);
#endif
}

}  // namespace base

#endif  // BASE_TO_UNDERLYING_H_
