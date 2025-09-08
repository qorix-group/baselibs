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
#include "score/language/safecpp/string_view/zspan.h"

#include <gtest/gtest.h>

#include <type_traits>
#include <utility>

namespace score::safecpp
{
namespace
{

using safecpp::literals::operator""_zsp;

TEST(ZSpan, CanConstructFromLiteral)
{
    // Given a `zspan` constructed from a string literal
    constexpr auto span = "hello"_zsp;

    // Then it must not be empty
    EXPECT_FALSE(span.empty());

    // And its size must be the length of the string literal minus the null-terminator
    EXPECT_EQ(span.size(), 5U);

    // And its data must point to the start of the string literal
    EXPECT_STREQ(span.data(), "hello");

    // And its underlying character buffer must be null-terminted
    EXPECT_EQ(span.data()[span.size()], '\0');
}

TEST(ZSpan, CanConstructFromOtherZSpan)
{
    // Given a preconstructed `zspan`
    zspan<char> span{};

    // When constructing it based on the above one
    zspan<const char> copied{span};

    // Then it must have worked
    EXPECT_TRUE(copied.empty());
    EXPECT_EQ(copied.data(), nullptr);
}

TEST(ZSpan, CanDefaultConstruct)
{
    // Given a default-constructed `zspan`
    zspan<char> span{};

    // Then it must be empty
    EXPECT_TRUE(span.empty());

    // And its size must be zero
    EXPECT_EQ(span.size(), 0U);

    // And its data must be nullptr
    EXPECT_EQ(span.data(), nullptr);
}

TEST(ZSpan, CanCopyConstruct)
{
    // Given a preconstructed `zspan`
    auto span = "hello"_zsp;

    // When copy-constructing it based on the above one
    zspan<const char> copied{span};

    // Then it must not be empty
    EXPECT_FALSE(copied.empty());

    // And its size must match the original span's size
    EXPECT_EQ(copied.size(), span.size());

    // And its data must match the original span's data
    EXPECT_EQ(copied.data(), span.data());
    EXPECT_STREQ(copied.data(), span.data());
}

TEST(ZSpan, CanMoveConstruct)
{
    // Given a preconstructed `zspan`
    auto span = "hello"_zsp;

    // When move-constructing it based on the above one
    zspan<const char> moved{std::move(span)};

    // Then it must not be empty
    EXPECT_FALSE(moved.empty());

    // And its size must match the original span's size
    EXPECT_EQ(moved.size(), span.size());

    // And its data must match the original span's data
    EXPECT_EQ(moved.data(), span.data());
    EXPECT_STREQ(moved.data(), span.data());
}

TEST(ZSpan, CanCopyAssign)
{
    // Given a preconstructed `zspan`
    auto span = "hello"_zsp;

    // When copy-assigning it to another one
    zspan<const char> copied;
    copied = span;

    // Then it must not be empty
    EXPECT_FALSE(copied.empty());

    // And its size must match the original span's size
    EXPECT_EQ(copied.size(), span.size());

    // And its data must match the original span's data
    EXPECT_EQ(copied.data(), span.data());
    EXPECT_STREQ(copied.data(), span.data());
}

TEST(ZSpan, CanMoveAssign)
{
    // Given a preconstructed `zspan`
    auto span = "hello"_zsp;

    // When move-assigning it to another one
    zspan<const char> moved;
    moved = std::move(span);

    // Then it must not be empty
    EXPECT_FALSE(moved.empty());

    // And its size must match the original span's size
    EXPECT_EQ(moved.size(), span.size());

    // And its data must match the original span's data
    EXPECT_EQ(moved.data(), span.data());
    EXPECT_STREQ(moved.data(), span.data());
}

TEST(ZSpan, TypeTraits)
{
    using zspan = safecpp::zspan<char>;
    EXPECT_TRUE((std::is_nothrow_default_constructible_v<zspan>));
    EXPECT_TRUE((std::is_trivially_copy_constructible_v<zspan>));
    EXPECT_TRUE((std::is_trivially_move_constructible_v<zspan>));
    EXPECT_TRUE((std::is_trivially_destructible_v<zspan>));
}

}  // namespace
}  // namespace score::safecpp
