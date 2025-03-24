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
#include "score/language/safecpp/safe_math/details/multiplication/multiplication.h"
#include "score/language/safecpp/safe_math/details/test_type_collection.h"

#include <gtest/gtest.h>

#include <limits>
#include <type_traits>

namespace score::safe_math
{
namespace
{

template <typename>
class MultiplicationTest : public ::testing::Test
{
};

TYPED_TEST_SUITE(MultiplicationTest, UnsignedTypes, /* unused */);

TYPED_TEST(MultiplicationTest, MultiplicationWithUnsignedAndUnsignedWorks)
{
    const TypeParam val1{2U};
    const TypeParam val2{3U};
    const auto result = Multiply(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), val1 * val2);
}

TYPED_TEST(MultiplicationTest, MultiplicationWithUnsignedAndUnsignedMaxWorks)
{
    const TypeParam val1{1U};
    const TypeParam val2{std::numeric_limits<TypeParam>::max()};
    const auto result = Multiply(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), val1 * val2);
}

TYPED_TEST(MultiplicationTest, MultiplicationWithPositiveSignedAndPositiveSignedWorks)
{
    const typename std::make_signed_t<TypeParam> val1{2};
    const typename std::make_signed_t<TypeParam> val2{3};
    const auto result = Multiply(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), val1 * val2);
}

TYPED_TEST(MultiplicationTest, MultiplicationWithNegativeSignedAndNegativeSignedWorks)
{
    const typename std::make_signed_t<TypeParam> val1{-2};
    const typename std::make_signed_t<TypeParam> val2{-3};
    const auto result = Multiply(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), val1 * val2);
}

TYPED_TEST(MultiplicationTest, MultiplicationWithNegativeSignedAndNegativeSignedLowestWithoutOverflowWorks)
{
    const typename std::make_signed_t<TypeParam> val1{-1};
    const auto val2{std::numeric_limits<typename std::make_signed_t<TypeParam>>::lowest() + 1};
    const auto result = Multiply(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), val1 * val2);
}

TYPED_TEST(MultiplicationTest, MultiplicationWithNegativeSignedAndNegativeSignedAndOverflowThrowsError)
{
    const typename std::make_signed_t<TypeParam> val1{-1};
    const auto val2{std::numeric_limits<typename std::make_signed_t<TypeParam>>::lowest()};
    const auto result = Multiply(val1, val2);
    EXPECT_FALSE(result.has_value()) << "Value: " << result.value();
}

TYPED_TEST(MultiplicationTest, MultiplicationWithNegativeSignedAndPositiveSignedWorks)
{
    const typename std::make_signed_t<TypeParam> val1{-2};
    const typename std::make_signed_t<TypeParam> val2{3};
    const auto result = Multiply(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), val1 * val2);
}

TYPED_TEST(MultiplicationTest, MultiplicationWithPositiveSignedAndLowestSignedWorks)
{
    const typename std::make_signed_t<TypeParam> val1{1};
    const auto val2{std::numeric_limits<typename std::make_signed_t<TypeParam>>::lowest()};
    const auto result = Multiply(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), val1 * val2);
}

TYPED_TEST(MultiplicationTest, MultiplicationWithNegativeSignedAndMaxSignedWorks)
{
    const typename std::make_signed_t<TypeParam> val1{-1};
    const auto val2{std::numeric_limits<typename std::make_signed_t<TypeParam>>::max()};
    const auto result = Multiply(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), val1 * val2);
}

TYPED_TEST(MultiplicationTest, MultiplicationWithNegativeSignedAndFirstUnsupportedNegativeSignedWorks)
{
    const typename std::make_signed_t<TypeParam> val1{-2};
    const auto val2{-((std::numeric_limits<typename std::make_signed_t<TypeParam>>::max() / 2) + 1)};
    const auto result = Multiply(val1, val2);
    EXPECT_FALSE(result.has_value()) << "Value: " << result.value();
}

TYPED_TEST(MultiplicationTest, MultiplicationWithPositiveSignedAndNegativeSignedAndUnderflowThrowsError)
{
    const typename std::make_signed_t<TypeParam> val1{3};
    const auto val2{(std::numeric_limits<typename std::make_signed_t<TypeParam>>::lowest() / val1) - 1};
    const auto result = Multiply(val1, val2);
    EXPECT_FALSE(result.has_value()) << "Value: " << result.value();
}

TYPED_TEST(MultiplicationTest, MultiplicationWithNegativeSignedAndUnsignedExceedingLimitsThrowsError)
{
    const typename std::make_signed_t<TypeParam> val1{-1};
    const auto max = std::numeric_limits<typename std::make_signed_t<TypeParam>>::max();
    const TypeParam val2{static_cast<TypeParam>(max) + 2U};
    const auto result = Multiply(val1, val2);
    EXPECT_FALSE(result.has_value()) << "Value: " << result.value();
}

TYPED_TEST(MultiplicationTest, MultiplicationWithNegativeSignedAndUnsignedWorks)
{
    const typename std::make_signed_t<TypeParam> val1{-1};
    const auto max = std::numeric_limits<typename std::make_signed_t<TypeParam>>::max();
    const TypeParam val2{max};
    const auto result = Multiply(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), -static_cast<typename std::make_signed_t<TypeParam>>(max));
}

TYPED_TEST(MultiplicationTest, MultiplicationWithPositiveSignedAndUnsignedWorks)
{
    const typename std::make_signed_t<TypeParam> val1{1};
    const TypeParam val2{std::numeric_limits<typename std::make_signed_t<TypeParam>>::max()};
    const auto result = Multiply(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), std::numeric_limits<typename std::make_signed_t<TypeParam>>::max());
}

TYPED_TEST(MultiplicationTest, MultiplicationWithUnsignedAndPositiveSignedWorks)
{
    const auto val1{std::numeric_limits<TypeParam>::max()};
    const typename std::make_signed_t<TypeParam> val2{1};
    const auto result = Multiply(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), std::numeric_limits<TypeParam>::max());
}

TYPED_TEST(MultiplicationTest, MultiplicationByZeroAsFirstArgumentReturnsZero)
{
    const TypeParam val1{0};
    const TypeParam val2{1};
    const auto result = Multiply(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), 0);
}

TYPED_TEST(MultiplicationTest, MultiplicationByZeroAsSecondArgumentReturnsZero)
{
    const TypeParam val1{1};
    const TypeParam val2{0};
    const auto result = Multiply(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), 0);
}

template <typename>
class MultiplicationMultipleTypesTest : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(MultiplicationMultipleTypesTest);

TYPED_TEST_P(MultiplicationMultipleTypesTest, MultiplicationWorks)
{
    const auto max{std::numeric_limits<typename TypeParam::first_type>::max()};
    const typename TypeParam::first_type val1{max / static_cast<typename TypeParam::first_type>(2)};
    const typename TypeParam::second_type val2{2};
    const auto result = Multiply(val1, val2);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), val1 * val2);
}

TYPED_TEST_P(MultiplicationMultipleTypesTest, MultiplicationExceedingLimitsReturnsError)
{
    const auto max{std::numeric_limits<typename TypeParam::first_type>::max()};
    const auto half_max{(max / static_cast<typename TypeParam::first_type>(2))};
    const typename TypeParam::first_type val1{half_max + static_cast<typename TypeParam::first_type>(1)};
    const typename TypeParam::second_type val2{2};
    const auto result = Multiply(val1, val2);
    ASSERT_FALSE(result.has_value()) << "Value: " << result.value();
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

REGISTER_TYPED_TEST_SUITE_P(MultiplicationMultipleTypesTest,
                            MultiplicationWorks,
                            MultiplicationExceedingLimitsReturnsError);

INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedTypes, MultiplicationMultipleTypesTest, UnsignedTypePairs, /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedTypes, MultiplicationMultipleTypesTest, SignedTypePairs, /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedToUnsignedTypes, MultiplicationMultipleTypesTest, SignedUnsignedTypePairs,
                               /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedToSignedTypes, MultiplicationMultipleTypesTest, UnsignedSignedTypePairs,
                               /* unused */);

TEST(MultiplicationFloatingPointsTest, CanMultiplyTwoFloats)
{
    const double lhs{3.1};
    const double rhs{2.1};
    const auto result = Multiply(lhs, rhs);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), lhs * rhs);
}

TEST(MultiplicationFloatingPointsTest, CanMultiplyFloatWithInteger)
{
    const double lhs{3.1};
    const std::uint32_t rhs{2U};
    const auto result = Multiply(lhs, rhs);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), 6.2);
}

TEST(MultiplicationFloatingPointsTest, MultiplicationFailsIfIntegerCanNotBeRepresentedInFloat)
{
    const double lhs{3.1};
    const auto rhs{std::numeric_limits<std::uint64_t>::max()};
    const auto result = Multiply(lhs, rhs);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kImplicitRounding);
}

TEST(MultiplicationFloatingPointsTest, CanMultiplyIntegerWithFloat)
{
    const std::uint32_t lhs{2U};
    const double rhs{3.1};
    const auto result = Multiply(lhs, rhs);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), 6.2);
}

}  // namespace
}  // namespace score::safe_math
