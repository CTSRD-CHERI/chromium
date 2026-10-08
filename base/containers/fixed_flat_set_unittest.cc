// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/containers/fixed_flat_set.h"

#include <algorithm>
#include <cassert>
#include <string_view>

#include "base/test/gtest_util.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace base {

TEST(FixedFlatSetTest, MakeFixedFlatSet_SortedInput) {
#if defined(_LIBCPP_VERSION) && _LIBCPP_VERSION < 180000
  const auto kSet = MakeFixedFlatSet<int>({1, 2, 3, 4});
  assert(std::ranges::is_sorted(kSet) && "Error: Set is not sorted.");
  assert(std::ranges::adjacent_find(kSet) == kSet.end() &&
#else
  constexpr auto kSet = MakeFixedFlatSet<int>({1, 2, 3, 4});
  static_assert(std::ranges::is_sorted(kSet), "Error: Set is not sorted.");
  static_assert(std::ranges::adjacent_find(kSet) == kSet.end(),
#endif
                "Error: Set contains repeated elements.");
  EXPECT_THAT(kSet, ::testing::ElementsAre(1, 2, 3, 4));
}

TEST(FixedFlatSetTest, MakeFixedFlatSet_UnsortedInput) {
#if defined(_LIBCPP_VERSION) && _LIBCPP_VERSION < 180000
  const auto kSet =
      MakeFixedFlatSet<std::string_view>({"foo", "bar", "baz"});
  assert(std::ranges::is_sorted(kSet) && "Error: Set is not sorted.");
  assert(std::ranges::adjacent_find(kSet) == kSet.end() &&
#else
  constexpr auto kSet =
      MakeFixedFlatSet<std::string_view>({"foo", "bar", "baz"});
  static_assert(std::ranges::is_sorted(kSet), "Error: Set is not sorted.");
  static_assert(std::ranges::adjacent_find(kSet) == kSet.end(),
#endif
                "Error: Set contains repeated elements.");
  EXPECT_THAT(kSet, ::testing::ElementsAre("bar", "baz", "foo"));
}

}  // namespace base
