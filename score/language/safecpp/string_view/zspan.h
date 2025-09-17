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

  public:
    using value_type = T;
    using size_type = std::size_t;

    using pointer = std::add_pointer_t<value_type>;
    using const_pointer = std::add_pointer_t<std::add_const_t<value_type>>;
    using const_reference = std::add_lvalue_reference_t<std::add_const_t<value_type>>;

    using violation_policies = null_termination_violation_policies;

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

namespace literals
{
///
/// @brief user-defined literal operator for character type `const char`
/// @note invoking this literal operator manually is strictly prohibited and will result in undefined behavior!
///
[[nodiscard]] constexpr zspan<const char> operator""_zsp(const char* str, std::size_t len) noexcept
{
    // since string literals are guaranteed to be null-terminated, we can safely apply `len + 1U`
    return zspan<const char>{str, len + 1U, null_termination_violation_policies::set_empty{}};
}
}  // namespace literals

}  // namespace score::safecpp

// NOLINTEND(readability-identifier-naming)

#endif  // SCORE_LANGUAGE_SAFECPP_STRING_VIEW_ZSPAN_H
