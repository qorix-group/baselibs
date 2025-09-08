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

#if defined(__has_include) && __has_include(<span>)
#include <span>
#endif
#include <cstddef>
#include <type_traits>

// NOLINTBEGIN(readability-identifier-naming): STL-style notation is intended here
// in order to facilitate interoperability with other STL-like containters/algorithms

namespace score::safecpp
{

template <typename T>
class zspan;

namespace literals
{
namespace internal
{
/// @brief private implementation of user-defined literal operator
[[nodiscard]] constexpr zspan<const char> operator""_zsp(const char* str, std::size_t len) noexcept;

/// @brief helper type for user-defined literal operator
class zspan_creator
{
  private:  // since `operator()` is only permitted to get invoked by `operator""_zsp`
    friend constexpr zspan<const char> operator""_zsp(const char* str, std::size_t len) noexcept;
    [[nodiscard]] constexpr zspan<const char> operator()(const char* str, std::size_t len) noexcept;
};
}  // namespace internal

///
/// @brief user-defined literal operator for public use
///
using internal::operator""_zsp;

}  // namespace literals

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

    /// @brief Disallow implicit conversions back to `std::span` since its `data()` method exposes a non-const pointer.
    /// @details Reason: a non-const pointer to the underlying sequence could get used to overwrite the null-terminator.
#if defined(__cpp_lib_span)
    operator std::span<std::remove_const_t<CharType>>() const = delete;
    operator std::span<std::add_const_t<CharType>>() const = delete;
#endif

  private:
    friend class literals::internal::zspan_creator;  // since only `zspan_creator` shall have direct access to fields

    const_pointer data_{};
    size_type size_{};
};

///
/// @brief user-defined literal operator helper for character type `char`
///
[[nodiscard]] constexpr zspan<const char> literals::internal::zspan_creator::operator()(const char* str,
                                                                                        std::size_t len) noexcept
{
    // no further checks are required here since string literals are guaranteed to be null-terminated
    zspan<const char> result{};
    result.data_ = str;
    result.size_ = len;
    return result;
}

///
/// @brief user-defined literal operator for character type `char`
///
[[nodiscard]] constexpr zspan<const char> literals::internal::operator""_zsp(const char* str, std::size_t len) noexcept
{
    return literals::internal::zspan_creator{}(str, len);
}

}  // namespace score::safecpp

// NOLINTEND(readability-identifier-naming)

#endif  // SCORE_LANGUAGE_SAFECPP_STRING_VIEW_ZSPAN_H
