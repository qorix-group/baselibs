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
#include "score/language/safecpp/string_view/zstring_view.h"

#include "score/language/safecpp/string_view/zspan.h"

#include "score/mw/log/logging.h"

#include <gtest/gtest.h>

#include <string_view>
#include <type_traits>
#include <utility>

namespace score::safecpp
{
namespace
{

using safecpp::literals::operator""_zsv;
using safecpp::literals::operator""_zsp;

TEST(ZStringView, CanConstructFromLiteral)
{
    // Given a `zstring_view` constructed from a string literal
    constexpr auto view = "hello"_zsv;

    // Then it must not be empty
    EXPECT_FALSE(view.empty());

    // And its size must be the length of the string literal minus the null-terminator
    EXPECT_EQ(view.size(), 5U);

    // And its data must point to the start of the string literal
    EXPECT_STREQ(view.c_str(), "hello");
    EXPECT_STREQ(view.data(), "hello");

    // And its underlying character buffer must be null-terminted
    EXPECT_EQ(view.data()[view.size()], '\0');
}

TEST(ZStringView, CanConstructFromZSpan)
{
    // Given a `zspan` constructed from a null-terminated character buffer
    auto span = "hello"_zsp;

    // When constructing a `zstring_view` from it
    safecpp::zstring_view view{span};

    // Then it must not be empty
    EXPECT_FALSE(view.empty());

    // And its size must be the length of the string literal minus the null-terminator
    EXPECT_EQ(view.size(), 5U);

    // And its data must match the `zspan`'s one
    EXPECT_STREQ(view.c_str(), "hello");
    EXPECT_STREQ(view.data(), "hello");

    // And its underlying character buffer must be null-terminted
    EXPECT_EQ(view.data()[view.size()], '\0');
}

TEST(ZStringView, CanDefaultConstruct)
{
    // Given a default-constructed `zstring_view`
    zstring_view view{};

    // Then it must be empty
    EXPECT_TRUE(view.empty());

    // And its size must be zero
    EXPECT_EQ(view.size(), 0U);

    // And its data must be nullptr
    EXPECT_EQ(view.data(), nullptr);
}

TEST(ZStringView, CanCopyConstruct)
{
    // Given a preconstructed `zstring_view`
    auto view = "hello"_zsv;

    // When copy-constructing it based on the above one
    zstring_view copied{view};

    // Then it must not be empty
    EXPECT_FALSE(copied.empty());

    // And its size must match the original view's size
    EXPECT_EQ(copied.size(), view.size());

    // And its data must match the original view's data
    EXPECT_EQ(copied.data(), view.data());
    EXPECT_STREQ(copied.data(), view.data());
}

TEST(ZStringView, CanMoveConstruct)
{
    // Given a preconstructed `zstring_view`
    auto view = "hello"_zsv;

    // When move-constructing it based on the above one
    zstring_view moved{std::move(view)};

    // Then it must not be empty
    EXPECT_FALSE(moved.empty());

    // And its size must match the original view's size
    EXPECT_EQ(moved.size(), view.size());

    // And its data must match the original view's data
    EXPECT_EQ(moved.data(), view.data());
    EXPECT_STREQ(moved.data(), view.data());
}

TEST(ZStringView, CanCopyAssign)
{
    // Given a preconstructed `zstring_view`
    auto view = "hello"_zsv;

    // When copy-assigning it to another one
    zstring_view copied;
    copied = view;

    // Then it must not be empty
    EXPECT_FALSE(copied.empty());

    // And its size must match the original view's size
    EXPECT_EQ(copied.size(), view.size());

    // And its data must match the original view's data
    EXPECT_EQ(copied.data(), view.data());
    EXPECT_STREQ(copied.data(), view.data());
}

TEST(ZStringView, CanMoveAssign)
{
    // Given a preconstructed `zstring_view`
    auto view = "hello"_zsv;

    // When move-assigning it to another one
    zstring_view moved;
    moved = std::move(view);

    // Then it must not be empty
    EXPECT_FALSE(moved.empty());

    // And its size must match the original view's size
    EXPECT_EQ(moved.size(), view.size());

    // And its data must match the original view's data
    EXPECT_EQ(moved.data(), view.data());
    EXPECT_STREQ(moved.data(), view.data());
}

TEST(ZStringView, CanConvertToLogString)
{
    // Given a preconstructed `zstring_view`
    auto view = "hello"_zsv;

    // Then it must be usable in conjunction with mw::log's `LogStream`
    mw::log::LogDebug() << view;
}

TEST(ZStringView, CanConvertToStdStringView)
{
    // Given a preconstructed `zstring_view`
    auto view = "hello"_zsv;

    // Then it must be implicitly convertible to `std::string_view`
    std::string_view sv = view;
    EXPECT_FALSE(sv.empty());
    EXPECT_EQ(sv.size(), view.size());
    EXPECT_EQ(sv.data(), view.data());
    EXPECT_STREQ(sv.data(), view.data());

    // And it must also be explicitly convertible to `std::string`
    auto str = std::string{view};
    EXPECT_FALSE(str.empty());
    EXPECT_EQ(str.size(), view.size());
    EXPECT_STREQ(str.data(), view.data());
}

TEST(ZStringView, CanCompareWithStringView)
{
    // Given a preconstructed `zstring_view`
    auto view = "hello"_zsv;

    // When attempting, then equality checks with `std::string_view` must be possible
    EXPECT_TRUE(view == std::string_view{"hello"});
    EXPECT_FALSE(view != std::string_view{"hello"});
    EXPECT_TRUE(view != std::string_view{"hello world"});
    EXPECT_FALSE(view == std::string_view{"hello world"});

    // When attempting, then comparison checks with `std::string_view` must be possible
    EXPECT_TRUE(view > std::string_view{"hell"});
    EXPECT_FALSE(view < std::string_view{"hello"});
    EXPECT_TRUE(view <= std::string_view{"hello"});
    EXPECT_FALSE(view > std::string_view{"hello world"});
    EXPECT_FALSE(view >= std::string_view{"hello world"});
}

TEST(ZStringView, TypeTraits)
{
    EXPECT_TRUE((std::is_nothrow_default_constructible_v<safecpp::zstring_view>));
    EXPECT_TRUE((std::is_trivially_copy_constructible_v<safecpp::zstring_view>));
    EXPECT_TRUE((std::is_trivially_move_constructible_v<safecpp::zstring_view>));
    EXPECT_TRUE((std::is_trivially_destructible_v<safecpp::zstring_view>));
}

}  // namespace
}  // namespace score::safecpp
