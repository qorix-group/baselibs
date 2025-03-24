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
#include "score/language/safecpp/safe_math/details/negate/negate.h"
#include "score/language/safecpp/safe_math/details/test_type_collection.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>

namespace score::safe_math
{
namespace
{

template <typename>
class NegateTest : public ::testing::Test
{
};

TYPED_TEST_SUITE(NegateTest, IntegerTypes, /* unused */);

TYPED_TEST(NegateTest, CanNegateNegativeValue)
{
    if (std::is_unsigned<TypeParam>::value)
    {
        GTEST_SKIP() << "Test not supported for unsigned types";
    }
    const auto value_to_be_negated{static_cast<typename std::make_signed_t<TypeParam>>(-1)};
    const auto result = Negate(value_to_be_negated);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_TRUE(CmpEqual(result.value(), Abs(value_to_be_negated)));
}

TYPED_TEST(NegateTest, CanNegatePositiveValue)
{
    const auto value_to_be_negated{1U};
    const auto result = Negate(value_to_be_negated);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), -1);
}

TYPED_TEST(NegateTest, CanNegatePositiveToMinimum)
{
    const auto signed_max = std::numeric_limits<typename std::make_signed_t<TypeParam>>::max();
    const auto value_to_be_negated{static_cast<typename std::make_unsigned_t<TypeParam>>(signed_max) + 1U};
    const auto result = Negate<typename std::make_signed_t<TypeParam>>(value_to_be_negated);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), std::numeric_limits<typename std::make_signed_t<TypeParam>>::min());
}

TYPED_TEST(NegateTest, NegateWillThrowErrorWhenResultNotRepresentable)
{
    const auto signed_max = std::numeric_limits<typename std::make_signed_t<TypeParam>>::max();
    const auto value_to_be_negated{static_cast<typename std::make_unsigned_t<TypeParam>>(signed_max) + 2U};
    const auto result = Negate<typename std::make_signed_t<TypeParam>>(value_to_be_negated);
    ASSERT_FALSE(result.has_value()) << "Value: " << value_to_be_negated << " Result: " << result.value();
}

TEST(NegateMinimumTest, CanNegateLowestValue)
{
    const auto value_to_be_negated{std::numeric_limits<std::int32_t>::lowest()};
    const auto result = Negate<std::uint32_t>(value_to_be_negated);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), Abs(value_to_be_negated));
}

TEST(NegateFloatingPointsTest, CanNegatePositiveNumber)
{
    const double value{6.51};
    const auto result = Negate(value);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), -value);
}

TEST(NegateFloatingPointsTest, CanNegateNegativeNumber)
{
    const double value{-6.51};
    const auto result = Negate(value);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), -value);
}

TEST(NegateFloatingPointsTest, NegateWillReturnErrorWhenResultNotRepresentable)
{
    const auto value_to_be_negated = std::numeric_limits<double>::max();
    const auto result = Negate<float>(value_to_be_negated);
    ASSERT_FALSE(result.has_value()) << "Value: " << value_to_be_negated << " Result: " << result.value();
}

}  // namespace
}  // namespace score::safe_math
