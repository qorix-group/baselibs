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
#include "score/language/safecpp/safe_math/details/division/division.h"
#include "score/language/safecpp/safe_math/details/test_type_collection.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>

namespace score::safe_math
{
namespace
{

template <typename>
class DivisionMultipleTypesTest : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(DivisionMultipleTypesTest);

TYPED_TEST_P(DivisionMultipleTypesTest, DivisionWorks)
{
    const auto max{std::numeric_limits<typename TypeParam::first_type>::max()};
    const typename TypeParam::first_type val1{max - static_cast<typename TypeParam::first_type>(1)};
    const typename TypeParam::second_type val2{2};
    const auto result = Divide(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), (max - 1) / 2);
}

TYPED_TEST_P(DivisionMultipleTypesTest, DivisionWithRoundingReturnsError)
{
    const typename TypeParam::first_type val1{3};
    const typename TypeParam::second_type val2{2};
    const auto result = Divide(val1, val2);
    ASSERT_FALSE(result.has_value()) << "Value: " << result.value();
    EXPECT_EQ(result.error(), ErrorCode::kImplicitRounding);
}

TYPED_TEST_P(DivisionMultipleTypesTest, DivisionByZeroReturnsError)
{
    const typename TypeParam::first_type val1{3};
    const typename TypeParam::second_type val2{0};
    const auto result = Divide(val1, val2);
    ASSERT_FALSE(result.has_value()) << "Value: " << result.value();
    EXPECT_EQ(result.error(), ErrorCode::kDivideByZero);
}

REGISTER_TYPED_TEST_SUITE_P(DivisionMultipleTypesTest,
                            DivisionWorks,
                            DivisionWithRoundingReturnsError,
                            DivisionByZeroReturnsError);

INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedTypes, DivisionMultipleTypesTest, UnsignedTypePairs, /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedTypes, DivisionMultipleTypesTest, SignedTypePairs, /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedUnsignedTypes, DivisionMultipleTypesTest, SignedUnsignedTypePairs, /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedSignedTypes, DivisionMultipleTypesTest, UnsignedSignedTypePairs, /* unused */);

template <typename>
class DivisionUnsignedSignedTest : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(DivisionUnsignedSignedTest);

TYPED_TEST_P(DivisionUnsignedSignedTest, DivisionWithPositiveSignWorks)
{
    const typename TypeParam::first_type val1{6};
    const typename TypeParam::second_type val2{2};
    const auto result = Divide(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), 3);
}

TYPED_TEST_P(DivisionUnsignedSignedTest, DivisionWithNegativeSignWorks)
{
    const typename TypeParam::first_type val1{6};
    const typename TypeParam::second_type val2{-2};
    const auto result = Divide<std::int8_t>(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), -3);
}

REGISTER_TYPED_TEST_SUITE_P(DivisionUnsignedSignedTest, DivisionWithPositiveSignWorks, DivisionWithNegativeSignWorks);

INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedSignedTypes, DivisionUnsignedSignedTest, UnsignedSignedTypePairs, /* unused */);

template <typename>
class DivisionSignedUnsignedTest : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(DivisionSignedUnsignedTest);

TYPED_TEST_P(DivisionSignedUnsignedTest, DivisionWithPositiveSignWorks)
{
    const typename TypeParam::first_type val1{6};
    const typename TypeParam::second_type val2{2};
    const auto result = Divide(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), 3);
}

TYPED_TEST_P(DivisionSignedUnsignedTest, DivisionWithNegativeSignWorks)
{
    const typename TypeParam::first_type val1{-6};
    const typename TypeParam::second_type val2{2};
    const auto result = Divide<std::int8_t>(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), -3);
}

REGISTER_TYPED_TEST_SUITE_P(DivisionSignedUnsignedTest, DivisionWithPositiveSignWorks, DivisionWithNegativeSignWorks);

INSTANTIATE_TYPED_TEST_SUITE_P(SignedUnsignedTypes, DivisionSignedUnsignedTest, SignedUnsignedTypePairs, /* unused */);

TEST(DivisionFloatingPointsTest, CanDivideTwoFloats)
{
    const double lhs{6.51};
    const double rhs{2.1};
    const auto result = Divide(lhs, rhs);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), lhs / rhs);
}

TEST(DivisionFloatingPointsTest, DivisionByZeroReturnsError)
{
    const double lhs{1.0};
    const auto rhs{0.0};
    const auto result = Divide(lhs, rhs);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kDivideByZero);
}

TEST(DivisionFloatingPointsTest, CanDivideFloatWithInteger)
{
    const double lhs{3.1};
    const std::uint32_t rhs{2U};
    const auto result = Divide(lhs, rhs);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), 1.55);
}

TEST(DivisionFloatingPointsTest, DivisionFailsIfRhsIntegerCanNotBeRepresentedInFloat)
{
    const double lhs{3.1};
    const auto rhs{std::numeric_limits<std::uint64_t>::max()};
    const auto result = Divide(lhs, rhs);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kImplicitRounding);
}

TEST(DivisionFloatingPointsTest, CanDivideIntegerWithFloat)
{
    const std::uint32_t lhs{2U};
    const double rhs{0.5};
    const auto result = Divide(lhs, rhs);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), 4.0);
}

TEST(DivisionFloatingPointsTest, CanDivideIntegerWithVerySmallFloat)
{
    const std::uint32_t lhs{2U};
    const double rhs{0.0000000001};
    const auto result = Divide(lhs, rhs);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), 2.0e10);
}

TEST(DivisionFloatingPointsTest, DivisionFailsIfLhsIntegerCanNotBeRepresentedInFloat)
{
    const auto lhs{std::numeric_limits<std::uint64_t>::max()};
    const double rhs{3.1};
    const auto result = Divide(lhs, rhs);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kImplicitRounding);
}

}  // namespace
}  // namespace score::safe_math
