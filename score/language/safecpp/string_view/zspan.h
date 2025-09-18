/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
#ifndef SCORE_LANGUAGE_SAFECPP_STRING_VIEW_ZSPAN_H
#define SCORE_LANGUAGE_SAFECPP_STRING_VIEW_ZSPAN_H

#include "score/language/safecpp/string_view/null_termination_violation_policies.h"

#if defined(__has_include) && __has_include(<span>)
#include <span>
#endif
#include <cstddef>
#include <functional>
#include <type_traits>

// NOLINTBEGIN(readability-identifier-naming): STL-style notation is intended here
// in order to facilitate interoperability with other STL-like containters/algorithms

namespace score::safecpp
{

namespace details
{
constexpr inline auto kNullByte = '\0';
}

///
/// @brief view type over contiguous sequence of objects which is guaranteed to be null-terminated
/// @details The null-termination guarantee for the underlying sequence gets preserved by every
///          member function offered by `zspan`. That’s why the underlying sequence can only
///          get accessed via a `const_pointer` as returned by `data()`.
///
template <typename T>
class zspan
{
    static_assert(!std::is_array_v<T>, "`safecpp::zspan` cannot be instantiated for array types");
    static_assert(!std::is_pointer_v<T>, "`safecpp::zspan` cannot be instantiated for pointer types");
    static_assert(!std::is_reference_v<T>, "`safecpp::zspan` cannot be instantiated for reference types");

    /// @brief type trait for checking whether type \p U; is a `safecpp::zspan`
    template <typename U>
    class is_zspan : public std::false_type
    {
    };
    template <typename U>
    class is_zspan<zspan<U>> : public std::true_type
    {
    };

    /// @brief type trait for checking whether type \p R; is a range over elements which can get compared to kNullByte
    template <typename R, typename = void>
    class is_compatible_range : public std::false_type
    {
    };
    template <typename R>
    class is_compatible_range<
        R,
        std::void_t<decltype(std::data(std::declval<R>())), decltype(std::size(std::declval<R>()))>>
        // NOLINTNEXTLINE(score-banned-function): using `std::data()` is valid here, `is_range_of` is just a type trait
        : public std::is_convertible<decltype(*std::data(std::declval<R>())), decltype(details::kNullByte)>
    {
      public:
        using value_type = std::remove_reference_t<decltype(*std::data(std::declval<R>()))>;
    };

    /// @brief type trait for checking whether type \p R; is a range over a guaranteed null-terminated sequence
    template <typename R, typename = void>
    class is_guaranteed_null_terminated : public std::false_type
    {
    };
    template <typename R>
    class is_guaranteed_null_terminated<
        R,
        std::void_t<decltype(std::declval<R>().c_str()), decltype(std::declval<R>().size())>> : public std::true_type
    {
      public:
        using value_type = std::remove_reference_t<decltype(*std::declval<R>().c_str())>;
    };

    /// @brief type trait for checking whether a ptr to \p RangeValueType;'s is convertible to one of \p ZSpanValueType;
    template <typename RangeValueType, typename ZSpanValueType>
    class is_pointer_convertible
        : public std::is_convertible<std::add_pointer_t<RangeValueType>, std::add_pointer_t<ZSpanValueType>>
    {
    };

  public:
    using value_type = T;
    using size_type = std::size_t;

    using pointer = std::add_pointer_t<value_type>;
    using const_pointer = std::add_pointer_t<std::add_const_t<value_type>>;
    using const_reference = std::add_lvalue_reference_t<std::add_const_t<value_type>>;

    using violation_policies = null_termination_violation_policies;

    /// @brief Constructs `zspan` as view over a guaranteed null-terminated \p range;.
    template <typename RangeType,
              typename ZSpanValueType = T,
              typename RangeValueType = std::enable_if_t<is_guaranteed_null_terminated<RangeType>::value,
                                                         typename is_guaranteed_null_terminated<RangeType>::value_type>,
              std::enable_if_t<is_pointer_convertible<RangeValueType, ZSpanValueType>::value, bool> = true>
    // NOLINTNEXTLINE(google-explicit-constructor) allow implicit conversions from guaranteed null-terminated ranges
    constexpr zspan(RangeType&& range) noexcept(noexcept(range.c_str()) && noexcept(range.size()))
        : data_{range.c_str()}, size_{range.size()}
    {
    }

    /// @brief Constructs `zspan` as view over arbitrary \p range;.
    /// @details The provided \p violation_policy; will get invoked in case the range is not null-terminated.
    template <typename RangeType,
              typename ZSpanValueType = T,
              typename RangeValueType = std::enable_if_t<is_compatible_range<RangeType>::value,
                                                         typename is_compatible_range<RangeType>::value_type>,
              typename RT = std::remove_cv_t<std::remove_reference_t<RangeType>>,
              typename ViolationPolicy = violation_policies::default_policy,
              std::enable_if_t<violation_policies::is_valid_one<ViolationPolicy>(), bool> = true,
              std::enable_if_t<is_pointer_convertible<RangeValueType, ZSpanValueType>::value, bool> = true,
              std::enable_if_t<
                  std::conjunction_v<std::negation<is_zspan<RT>>, std::negation<is_guaranteed_null_terminated<RT>>>,
                  bool> = true>
    // NOLINTBEGIN(cppcoreguidelines-pro-type-member-init) false positive, this constructor delegates to another one
    constexpr explicit zspan(RangeType&& range, ViolationPolicy violation_policy = {}) noexcept(
        // NOLINTNEXTLINE(score-banned-function) using `std::data()` is valid here since used in conj. with `std::size()`
        noexcept(std::data(range)) && noexcept(std::size(range)) && noexcept(std::invoke(violation_policy, "reason")))
        // NOLINTNEXTLINE(score-banned-function) using `std::data()` is valid here since used in conj. with `std::size()`
        : zspan(std::data(range), std::size(range), std::move(violation_policy))
    // NOLINTEND(cppcoreguidelines-pro-type-member-init)
    {
    }

    /// @brief Constructs a `zspan` as view over null-terminated sequence \p data; with provided \p size;.
    /// @details The provided \p violation_policy; will get invoked in case the sequence is not null-terminated.
    template <typename ViolationPolicy = violation_policies::default_policy,
              std::enable_if_t<violation_policies::is_valid_one<ViolationPolicy>(), bool> = true>
    constexpr explicit zspan(pointer data, size_type size, ViolationPolicy violation_policy = {}) noexcept(
        noexcept(std::invoke(violation_policy, "reason")))
        : data_{data}, size_{size}
    {
        if ((data_ == nullptr) || (size_ == 0U) || (back() != details::kNullByte))
        {
            data_ = nullptr;
            size_ = 0U;
            std::invoke(violation_policy, "score::safecpp::zspan: provided range is not null-terminated");
        }
        if (size_ > 0U)
        {
            size_ -= 1U;
        }
    }

    /// @brief Constructs a `zspan` from another `zspan` of compatible type.
    template <typename U,
              std::enable_if_t<std::conjunction_v<std::negation<std::is_same<U, T>>,
                                                  std::is_convertible<typename zspan<U>::pointer, pointer>>,
                               bool> = true>
    // NOLINTNEXTLINE(google-explicit-constructor) allow implicit conversions from compatible other `zspan`
    constexpr zspan(zspan<U> other) noexcept : data_{other.data()}, size_{other.size()}
    {
    }

    /// @brief Default constructs a `zspan`.
    constexpr zspan() noexcept = default;

    /// @brief Obtains pointer to the null-terminated underlying sequence.
    [[nodiscard]] constexpr const_pointer data() const noexcept
    {
        return data_;
    }

    /// @brief Obtains number of `value_type` elements in the view (not accounting the trailing null-terminator).
    [[nodiscard]] constexpr size_type size() const noexcept
    {
        return size_;
    }

    /// @brief Returns whether the span's range is empty or not.
    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return (data_ == nullptr || size_ == 0U);
    }

    /// @brief Returns a const reference to the first element in the span.
    /// @note Calling this function on an empty span results in undefined behavior!
    [[nodiscard]] constexpr const_reference front() const
    {
        return *data_;
    }

    /// @brief Returns a const reference to the last element in the span.
    /// @note Calling this function on an empty span results in undefined behavior!
    [[nodiscard]] constexpr const_reference back() const
    {
        return data_[size_ - 1U];
    }

    /// @brief Disallow implicit conversions back to `std::span` since its `data()` method exposes a non-const pointer.
    /// @details Reason: a non-const pointer to the underlying sequence could get used to overwrite the null-terminator.
#if defined(__cpp_lib_span)
    operator std::span<std::remove_const_t<CharType>>() const = delete;
    operator std::span<std::add_const_t<CharType>>() const = delete;
#endif

  private:
    const_pointer data_{};
    size_type size_{};
};

}  // namespace score::safecpp

// NOLINTEND(readability-identifier-naming)

#endif  // SCORE_LANGUAGE_SAFECPP_STRING_VIEW_ZSPAN_H
