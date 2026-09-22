// Copyright 2026 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_IS_INTCAP_H_
#define BASE_IS_INTCAP_H_

namespace base {

template <typename T>
inline constexpr bool IsIntcap = false;

#if defined(__CHERI_PURE_CAPABILITY__)
template <>
inline constexpr bool IsIntcap<__intcap> = true;
template <>
inline constexpr bool IsIntcap<unsigned __intcap> = true;
#endif

}  // namespace base

#endif  // BASE_IS_INTCAP_H_
