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
#ifndef SCORE_LANGUAGE_SAFECPP_STRING_VIEW_ZSTRING_VIEW_H
#define SCORE_LANGUAGE_SAFECPP_STRING_VIEW_ZSTRING_VIEW_H

#include "score/language/safecpp/string_view/zspan.h"

#include <string_view>
#include <type_traits>

// NOLINTBEGIN(readability-identifier-naming): STL-style notation is intended here
// in order to facilitate interoperability with other STL-like containters/algorithms

namespace score::safecpp
{

///
/// @brief non-modifiable view type over null-terminated character sequence
///
template <typename CharType>
class basic_zstring_view : private safecpp::zspan<std::add_const_t<CharType>>
{
    using base = safecpp::zspan<std::add_const_t<CharType>>;

  public:
    using value_type = typename base::value_type;
    using size_type = typename base::size_type;

    /// @brief Constructs a `basic_zstring_view` from base type (i.e. `zspan<const CharType>`)
    // NOLINTNEXTLINE(google-explicit-constructor) allow implicit conversions from base type `zspan` (are always safe)
    constexpr basic_zstring_view(base other) noexcept : base(other) {}

    /// @brief Prohibits construction of a `basic_zstring_view` from `std::basic_string_view`
    constexpr basic_zstring_view(std::basic_string_view<CharType>) = delete;

    /// @brief Default constructs a `basic_zstring_view`.
    constexpr basic_zstring_view() noexcept = default;

    using base::base;

    using base::data;
    using base::empty;
    using base::size;

    ///
    /// @brief implicit conversion operator from `basic_zstring_view` to `std::string_view`
    ///
    // NOLINTNEXTLINE(google-explicit-constructor) allow implicit conversions to `std::string_view` (const view type)
    [[nodiscard]] constexpr operator std::string_view() const noexcept
    {
        return std::string_view{data(), size()};
    }

    /// @brief Returns a pointer to the null-terminated underlying character sequence.
    [[nodiscard]] constexpr auto c_str() const noexcept
    {
        return data();
    }
};

/// @brief alias definition for `zstring_view`
using zstring_view = basic_zstring_view<char>;

namespace literals
{
///
/// @brief user-defined literal operator for character type `char`
///
[[nodiscard]] constexpr zstring_view operator""_zsv(const char* str, std::size_t len) noexcept
{
    return safecpp::literals::operator""_zsp(str, len);
}
}  // namespace literals

}  // namespace score::safecpp

// NOLINTEND(readability-identifier-naming)

#endif  // SCORE_LANGUAGE_SAFECPP_STRING_VIEW_ZSTRING_VIEW_H
