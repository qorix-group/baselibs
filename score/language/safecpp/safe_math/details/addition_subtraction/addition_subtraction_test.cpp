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
#include "score/language/safecpp/safe_math/details/addition_subtraction/addition_subtraction.h"
#include "score/language/safecpp/safe_math/details/test_type_collection.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <type_traits>

namespace score::safe_math
{
namespace
{

template <typename>
class AddTest : public ::testing::Test
{
};

TYPED_TEST_SUITE(AddTest, UnsignedTypes, /* unused */);

TYPED_TEST(AddTest, AddingTwoUnsignedWithoutOverflowWorks)
{
    const TypeParam val1{1};
    const TypeParam val2{2};
    const auto result = Add(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), val1 + val2);
}

TYPED_TEST(AddTest, AddingTwoUnsignedWithOverflowThrowsError)
{
    const TypeParam val1{std::numeric_limits<TypeParam>::max()};
    const TypeParam val2{1};
    const auto result = Add(val1, val2);
    EXPECT_FALSE(result.has_value()) << "Value: " << result.value();
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

TYPED_TEST(AddTest, AddingTwoUnsignedWithTooSmallResultTypeThrowsError)
{
    if constexpr (std::is_same_v<TypeParam, std::uint8_t>)
    {
        GTEST_SKIP() << "Skipping, because there is no smaller result type than the input type";
    }
    const TypeParam val1{std::numeric_limits<TypeParam>::max()};
    const TypeParam val2{1};
    const auto result = Add<std::uint8_t>(val1, val2);
    EXPECT_FALSE(result.has_value()) << "Value: " << result.value();
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

TYPED_TEST(AddTest, AddingUnsignedWithPositiveSignedWithoutOverflowWorks)
{
    const TypeParam val1{1};
    const typename std::make_signed_t<TypeParam> val2{2};
    const auto result = Add(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), val1 + val2);
}

TYPED_TEST(AddTest, AddingUnsignedWithPositiveSignedWithOverflowReturnsError)
{
    const TypeParam val1{std::numeric_limits<TypeParam>::max()};
    const typename std::make_signed_t<TypeParam> val2{1};
    const auto result = Add(val1, val2);
    EXPECT_FALSE(result.has_value()) << "Value: " << result.value();
}

TYPED_TEST(AddTest, AddingUnsignedWithNegativeSignedWithoutOverflowOrUnderflowWorks)
{
    const TypeParam val1{1};
    const typename std::make_signed_t<TypeParam> val2{-2};
    const auto result = Add<typename std::make_signed_t<TypeParam>>(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), -1);
}

TYPED_TEST(AddTest, AddingUnsignedWithNegativeSignedWithUnderflowReturnsError)
{
    const TypeParam val1{std::numeric_limits<TypeParam>::min()};
    const typename std::make_signed_t<TypeParam> val2{-1};
    const auto result = Add<TypeParam>(val1, val2);
    EXPECT_FALSE(result.has_value()) << "Value: " << result.value();
}

TYPED_TEST(AddTest, AddingSignedWithUnsignedWorks)
{
    const typename std::make_signed_t<TypeParam> val1{1};
    const TypeParam val2{2};
    const auto result = Add(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), 3);
}

TYPED_TEST(AddTest, AddingPositiveSignedWithPositiveSignedWorks)
{
    const typename std::make_signed_t<TypeParam> val1{1};
    const typename std::make_signed_t<TypeParam> val2{2};
    const auto result = Add(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), 3);
}

TYPED_TEST(AddTest, AddingSignedWithNegativeSignedWorks)
{
    const typename std::make_signed_t<TypeParam> val1{1};
    const typename std::make_signed_t<TypeParam> val2{-2};
    const auto result = Add(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), -1);
}

template <typename>
class AdditionMultipleTypesTest : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(AdditionMultipleTypesTest);

TYPED_TEST_P(AdditionMultipleTypesTest, AdditionWorks)
{
    const typename TypeParam::first_type val1{2};
    const typename TypeParam::second_type val2{3};
    const auto result = Add(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), 5);
}

REGISTER_TYPED_TEST_SUITE_P(AdditionMultipleTypesTest, AdditionWorks);

INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedTypes, AdditionMultipleTypesTest, UnsignedTypePairs, /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedTypes, AdditionMultipleTypesTest, SignedTypePairs, /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedToUnsignedTypes, AdditionMultipleTypesTest, SignedUnsignedTypePairs,
                               /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedToSignedTypes, AdditionMultipleTypesTest, UnsignedSignedTypePairs,
                               /* unused */);

TEST(AdditionFloatingPointsTest, CanAddTwoFloats)
{
    const double lhs{2.0};
    const double rhs{3.1};
    const auto result = Add(lhs, rhs);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), lhs + rhs);
}

TEST(AdditionFloatingPointsTest, CanAddFloatWithInteger)
{
    const double lhs{3.1};
    const std::uint32_t rhs{2U};
    const auto result = Add(lhs, rhs);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), 5.1);
}

TEST(AdditionFloatingPointsTest, AdditionFailsIfIntegerCanNotBeRepresentedInFloat)
{
    const double lhs{3.1};
    const auto rhs{std::numeric_limits<std::uint64_t>::max()};
    const auto result = Add(lhs, rhs);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kImplicitRounding);
}

TEST(AdditionFloatingPointsTest, CanAddIntegerWithFloat)
{
    const std::uint32_t lhs{2U};
    const double rhs{3.1};
    const auto result = Add(lhs, rhs);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), 5.1);
}
template <typename>
class SubtractTest : public ::testing::Test
{
};

TYPED_TEST_SUITE(SubtractTest, UnsignedTypes, /* unused */);

TYPED_TEST(SubtractTest, SubtractBiggerUnsignedFromSmallerUnsignedWorks)
{
    const TypeParam val1{1U};
    const TypeParam val2{2U};
    const auto result = Subtract<std::int8_t>(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), -1);
}

TYPED_TEST(SubtractTest, SubtractEqualUnsignedWorks)
{
    const TypeParam val1{2U};
    const TypeParam val2{2U};
    const auto result = Subtract(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), 0);
}

TYPED_TEST(SubtractTest, SubtractSmallerUnsignedFromBiggerUnsignedWorks)
{
    const TypeParam val1{2U};
    const TypeParam val2{1U};
    const auto result = Subtract(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), 1);
}

TYPED_TEST(SubtractTest, SubtractNegativeSignedFromUnsignedWorks)
{
    const TypeParam val1{2U};
    const typename std::make_signed_t<TypeParam> val2{-1};
    const auto result = Subtract(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), 3);
}

TYPED_TEST(SubtractTest, SubtractPositiveSignedFromUnsignedWorks)
{
    const TypeParam val1{2U};
    const typename std::make_signed_t<TypeParam> val2{1};
    const auto result = Subtract(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), 1);
}

TYPED_TEST(SubtractTest, SubtractUnsignedFromPositiveSignedWorks)
{
    const typename std::make_signed_t<TypeParam> val1{0};
    const TypeParam val2{static_cast<TypeParam>(std::numeric_limits<typename std::make_signed_t<TypeParam>>::max()) +
                         1U};
    const auto result = Subtract(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), std::numeric_limits<typename std::make_signed_t<TypeParam>>::min());
}

TYPED_TEST(SubtractTest, SubtractUnsignedFromNegativeSignedWorks)
{
    const auto lowest{std::numeric_limits<typename std::make_signed_t<TypeParam>>::lowest()};
    const TypeParam val2{2U};
    const auto val1{lowest + static_cast<typename std::make_signed_t<TypeParam>>(val2)};
    const auto result = Subtract(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), lowest);
}

TEST(SubtractTestAddition, SubtractUnsignedFromNegativeSignedReturnsErrorOnOverflow)
{
    const std::int32_t val1{-1};
    const auto val2{std::numeric_limits<std::uint64_t>::max()};
    const auto result = Subtract(val1, val2);
    ASSERT_FALSE(result.has_value()) << "Value: " << result.value();
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

TYPED_TEST(SubtractTest, SubtractNegativeSignedFromSignedWorks)
{
    const typename std::make_signed_t<TypeParam> val1{0};
    const typename std::make_signed_t<TypeParam> val2{-2};
    const auto result = Subtract(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), Abs(val2));
}

TYPED_TEST(SubtractTest, SubtractLowestNegativeSignedFromSignedWorks)
{
    const auto lowest{std::numeric_limits<typename std::make_signed_t<TypeParam>>::lowest()};
    const typename std::make_signed_t<TypeParam> val1{0};
    const auto val2{lowest};
    const auto result = Subtract<TypeParam>(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), Abs(val2));
}

TYPED_TEST(SubtractTest, SubtractPositiveSignedMaximumFromSignedZeroWorks)
{
    const auto max{std::numeric_limits<typename std::make_signed_t<TypeParam>>::max()};
    const typename std::make_signed_t<TypeParam> val1{0};
    const auto val2{max};
    const auto result = Subtract(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), Negate(max).value());
}

template <typename>
class SubtractionMultipleTypesTest : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(SubtractionMultipleTypesTest);

TYPED_TEST_P(SubtractionMultipleTypesTest, SubtractionWorks)
{
    const typename TypeParam::first_type val1{3};
    const typename TypeParam::second_type val2{2};
    const auto result = Subtract(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), 1);
}

REGISTER_TYPED_TEST_SUITE_P(SubtractionMultipleTypesTest, SubtractionWorks);

INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedTypes, SubtractionMultipleTypesTest, UnsignedTypePairs, /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedTypes, SubtractionMultipleTypesTest, SignedTypePairs, /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedToUnsignedTypes, SubtractionMultipleTypesTest, SignedUnsignedTypePairs,
                               /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedToSignedTypes, SubtractionMultipleTypesTest, UnsignedSignedTypePairs,
                               /* unused */);

TEST(SubtractionFloatingPointsTest, CanSubtractTwoFloats)
{
    const double lhs{3.1};
    const double rhs{2.2};
    const auto result = Subtract(lhs, rhs);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), lhs - rhs);
}

TEST(SubtractionFloatingPointsTest, CanSubtractIntegerFromFloat)
{
    const double lhs{3.1};
    const std::uint32_t rhs{2U};
    const auto result = Subtract(lhs, rhs);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), 1.1);
}

TEST(SubtractionFloatingPointsTest, SubtractionFailsIfRhsIntegerCanNotBeRepresentedInFloat)
{
    const double lhs{3.1};
    const auto rhs{std::numeric_limits<std::uint64_t>::max()};
    const auto result = Subtract(lhs, rhs);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kImplicitRounding);
}

TEST(SubtractionFloatingPointsTest, CanSubtractFloatFromInteger)
{
    const std::uint32_t lhs{2U};
    const double rhs{3.1};
    const auto result = Subtract(lhs, rhs);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), -1.1);
}

TEST(SubtractionFloatingPointsTest, SubtractionFailsIfLhsIntegerCanNotBeRepresentedInFloat)
{
    const auto lhs{std::numeric_limits<std::uint64_t>::max()};
    const double rhs{3.1};
    const auto result = Subtract(lhs, rhs);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kImplicitRounding);
}

}  // namespace
}  // namespace score::safe_math
