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
#include "score/language/safecpp/string_view/null_termination_violation_policies.h"

#include "score/language/safecpp/string_view/zspan.h"
#include "score/language/safecpp/string_view/zstring_view.h"

#include <score/string.hpp>

#include <gtest/gtest.h>

#include <stdexcept>
#include <type_traits>
#include <utility>

namespace score::safecpp
{
namespace
{

constexpr const char null_terminated_char_array[] = "hello";

using safecpp::literals::operator""_zsp;
using safecpp::literals::operator""_zsv;

TEST(ZSpan, CanConstructConstexpr)
{
    // Given a `zspan`s constructed from a constexpr buffer
    constexpr safecpp::zspan<const char> span{null_terminated_char_array};
    constexpr safecpp::zspan<const char> other_span{"hello world"};

    // Then they must not be empty
    EXPECT_FALSE(span.empty());
    EXPECT_FALSE(other_span.empty());

    // And their size must be the length of the respective string literal minus the null-terminator
    EXPECT_EQ(span.size(), 5U);
    EXPECT_EQ(other_span.size(), 11U);

    // And their data must point to the start of the respective string literal
    EXPECT_STREQ(span.data(), "hello");
    EXPECT_STREQ(other_span.data(), "hello world");

    // And their underlying character buffers must be null-terminted
    EXPECT_EQ(span.data()[span.size()], '\0');
    EXPECT_EQ(other_span.data()[other_span.size()], '\0');
}

TEST(ZSpan, CanConstructFromBuffer)
{
    // Given a null-terminated character buffer
    char buffer[] = {'h', 'e', 'l', 'l', 'o', '\0'};

    // When constructing a `zspan` from it
    safecpp::zspan<char> span{buffer, sizeof(buffer)};

    // Then it must not be empty
    EXPECT_FALSE(span.empty());

    // And its size must be the length of the buffer minus the null-terminator
    EXPECT_EQ(span.size(), 5U);

    // And its data must match the buffer's data
    EXPECT_STREQ(span.data(), "hello");
    EXPECT_EQ(span.front(), 'h');
    EXPECT_EQ(span.back(), 'o');

    // When constructing a `zspan` from it in conjunction with a size of zero & the `throw_exception` violation policy
    // Then `std::invalid_argument` is expected to get thrown
    EXPECT_THROW(
        (span =
             safecpp::zspan<char>{
                 buffer, 0U, safecpp::null_termination_violation_policies::throw_exception<std::invalid_argument>{}}),
        std::invalid_argument);

    // When constructing a `zspan` from a nullptr in conjunction with the `throw_exception` violation policy
    // Then `std::invalid_argument` is expected to get thrown
    EXPECT_THROW((span =
                      safecpp::zspan<char>{
                          nullptr,
                          sizeof(buffer),
                          safecpp::null_termination_violation_policies::throw_exception<std::invalid_argument>{}}),
                 std::invalid_argument);

    // Given a non-null-terminated character buffer
    char invalid_buffer[] = {'h', 'e', 'l', 'l', 'o'};

    // When constructing a `zspan` from it in conjunction with the `set_empty` violation policy
    span = safecpp::zspan<char>{
        invalid_buffer, sizeof(invalid_buffer), safecpp::null_termination_violation_policies::set_empty{}};

    // Then it must be empty
    EXPECT_TRUE(span.empty());
    EXPECT_EQ(span.size(), 0U);
    EXPECT_EQ(span.data(), nullptr);

    // When constructing a `zspan` from it in conjunction with the `throw_exception` violation policy
    // Then `std::invalid_argument` is expected to get thrown
    EXPECT_THROW((span =
                      safecpp::zspan<char>{
                          invalid_buffer,
                          sizeof(invalid_buffer),
                          safecpp::null_termination_violation_policies::throw_exception<std::invalid_argument>{}}),
                 std::invalid_argument);
}

TEST(ZSpan, CanConstructFromRange)
{
    // Given a null-terminated character range
    char range[] = {'h', 'e', 'l', 'l', 'o', '\0'};

    // When constructing a `zspan` from it
    safecpp::zspan<char> span{range};

    // Then it must not be empty
    EXPECT_FALSE(span.empty());

    // And its size must be the length of the range minus the null-terminator
    EXPECT_EQ(span.size(), 5U);

    // And its data must match the buffer's data
    EXPECT_STREQ(span.data(), "hello");
    EXPECT_EQ(span.front(), 'h');
    EXPECT_EQ(span.back(), 'o');

    // Given a non-null-terminated character range
    char invalid_range[] = {'h', 'e', 'l', 'l', 'o'};

    // When constructing a `zspan` from it in conjunction with the `set_empty` violation policy
    span = safecpp::zspan<char>{invalid_range, safecpp::null_termination_violation_policies::set_empty{}};

    // Then it must be empty
    EXPECT_TRUE(span.empty());
    EXPECT_EQ(span.size(), 0U);
    EXPECT_EQ(span.data(), nullptr);

    // When constructing a `zspan` from it in conjunction with the `throw_exception` violation policy
    // Then `std::invalid_argument` is expected to get thrown
    EXPECT_THROW((span =
                      safecpp::zspan<char>{
                          invalid_range,
                          safecpp::null_termination_violation_policies::throw_exception<std::invalid_argument>{}}),
                 std::invalid_argument);
}

TEST(ZSpan, CanConstructFromAmpString)
{
    // Given a preconstructed `std::string`
    score::cpp::pmr::string str{"hello"};

    // When constructing it based on the above one
    safecpp::zspan<const char> span{str};

    // Then it must have worked
    ASSERT_FALSE(span.empty());
    EXPECT_EQ(span.size(), 5U);
    EXPECT_STREQ(span.data(), "hello");
}

TEST(ZSpan, CanConstructFromStdString)
{
    // Given a preconstructed `std::string`
    std::string str{"hello"};

    // When constructing it based on the above one
    safecpp::zspan<const char> span{str};

    // Then it must have worked
    ASSERT_FALSE(span.empty());
    EXPECT_EQ(span.size(), 5U);
    EXPECT_STREQ(span.data(), "hello");
}

TEST(ZSpan, CanConstructFromZStringView)
{
    // Given a preconstructed `zstring_view`
    constexpr static auto view = "hello"_zsv;

    // When constructing it based on the above one
    constexpr safecpp::zspan<const char> span = view;

    // Then it must have worked
    ASSERT_FALSE(span.empty());
    EXPECT_EQ(span.size(), 5U);
    EXPECT_STREQ(span.data(), "hello");
}

TEST(ZSpan, CanConstructFromOtherZSpan)
{
    // Given an empty preconstructed `zspan`
    zspan<char> empty_span{};

    // When constructing a `zspan` based on the above one
    zspan<const char> copied_empty{empty_span};

    // Then it must have worked
    EXPECT_TRUE(copied_empty.empty());
    EXPECT_EQ(copied_empty.data(), nullptr);

    // Given an non-empty preconstructed `zspan`
    char range[] = {'h', 'e', 'l', 'l', 'o', '\0'};
    zspan<char> span{range};

    // When constructing a `zspan` based on the above one
    zspan<const char> copied{span};

    // Then it must have worked correctly
    EXPECT_FALSE(copied.empty());
    EXPECT_EQ(copied.data(), std::data(range));
    EXPECT_STREQ(copied.data(), "hello");
    EXPECT_EQ(copied.front(), 'h');
    EXPECT_EQ(copied.back(), 'o');
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
