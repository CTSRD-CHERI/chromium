// Copyright 2026 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Compatibility shim for std::atomic_ref<T> (C++20) for compilers/libc++
// that lack it. Uses std::atomic<T> under the hood.

#ifndef BASE_ATOMIC_REF_H_
#define BASE_ATOMIC_REF_H_

#include <atomic>
#include <cstddef>
#include <type_traits>

namespace base {

template <typename T>
class atomic_ref {
  static_assert(std::is_trivially_copyable_v<T>,
                "atomic_ref requires a trivially copyable type");
  static_assert(sizeof(T) > 0, "atomic_ref requires a non-empty type");

 public:
  static constexpr size_t required_alignment = alignof(std::atomic<T>);

  explicit atomic_ref(T& obj) noexcept
      : ptr_(reinterpret_cast<std::atomic<T>*>(&obj)) {}

  atomic_ref(const atomic_ref&) noexcept = default;
  atomic_ref& operator=(const atomic_ref&) = delete;

  T operator=(T desired) const noexcept {
    store(desired);
    return desired;
  }

  void store(T desired, std::memory_order order =
                            std::memory_order_seq_cst) const noexcept {
    ptr_->store(desired, order);
  }

  T load(std::memory_order order = std::memory_order_seq_cst) const noexcept {
    return ptr_->load(order);
  }

  T exchange(T desired, std::memory_order order =
                            std::memory_order_seq_cst) const noexcept {
    return ptr_->exchange(desired, order);
  }

  bool compare_exchange_strong(T& expected, T desired,
                               std::memory_order success,
                               std::memory_order failure) const noexcept {
    return ptr_->compare_exchange_strong(expected, desired, success, failure);
  }

  bool compare_exchange_strong(
      T& expected, T desired,
      std::memory_order order = std::memory_order_seq_cst) const noexcept {
    return ptr_->compare_exchange_strong(expected, desired, order);
  }

  bool compare_exchange_weak(T& expected, T desired, std::memory_order success,
                             std::memory_order failure) const noexcept {
    return ptr_->compare_exchange_weak(expected, desired, success, failure);
  }

  bool compare_exchange_weak(
      T& expected, T desired,
      std::memory_order order = std::memory_order_seq_cst) const noexcept {
    return ptr_->compare_exchange_weak(expected, desired, order);
  }

  T fetch_add(T arg, std::memory_order order =
                         std::memory_order_seq_cst) const noexcept {
    return ptr_->fetch_add(arg, order);
  }

  T fetch_sub(T arg, std::memory_order order =
                         std::memory_order_seq_cst) const noexcept {
    return ptr_->fetch_sub(arg, order);
  }

  T fetch_and(T arg, std::memory_order order =
                         std::memory_order_seq_cst) const noexcept {
    return ptr_->fetch_and(arg, order);
  }

  T fetch_or(T arg, std::memory_order order =
                        std::memory_order_seq_cst) const noexcept {
    return ptr_->fetch_or(arg, order);
  }

  T fetch_xor(T arg, std::memory_order order =
                         std::memory_order_seq_cst) const noexcept {
    return ptr_->fetch_xor(arg, order);
  }

  bool is_lock_free() const noexcept { return ptr_->is_lock_free(); }

 private:
  std::atomic<T>* ptr_;
};

}  // namespace base

#endif  // BASE_ATOMIC_REF_H_
