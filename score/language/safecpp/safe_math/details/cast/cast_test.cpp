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
#include "score/language/safecpp/safe_math/details/cast/cast.h"
#include "score/language/safecpp/safe_math/details/test_type_collection.h"

#include <score/math.hpp>

#include <gtest/gtest.h>

namespace score::safe_math
{
namespace
{

template <typename>
class CastTest : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(CastTest);

TYPED_TEST_P(CastTest, CastMinimumWorks)
{
    const auto min_1 = std::numeric_limits<typename TypeParam::first_type>::lowest();
    const auto min_2 = std::numeric_limits<typename TypeParam::second_type>::lowest();

    typename TypeParam::first_type value_to_be_cast{};
    if (CmpGreaterEqual(min_1, min_2))
    {
        value_to_be_cast = min_1;
    }
    else
    {
        value_to_be_cast = static_cast<typename TypeParam::first_type>(min_2);
    }

    const auto result = Cast<typename TypeParam::second_type>(value_to_be_cast);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), static_cast<typename TypeParam::second_type>(value_to_be_cast));
}

TYPED_TEST_P(CastTest, CastMaximumWorks)
{
    const auto max_1 = std::numeric_limits<typename TypeParam::first_type>::max();
    const auto max_2 = std::numeric_limits<typename TypeParam::second_type>::max();

    typename TypeParam::first_type value_to_be_cast{};
    if (CmpLessEqual(max_1, max_2))
    {
        value_to_be_cast = max_1;
    }
    else
    {
        value_to_be_cast = static_cast<typename TypeParam::first_type>(max_2);
    }

    const auto result = Cast<typename TypeParam::second_type>(value_to_be_cast);
    ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    EXPECT_EQ(result.value(), static_cast<typename TypeParam::second_type>(value_to_be_cast));
}

REGISTER_TYPED_TEST_SUITE_P(CastTest, CastMinimumWorks, CastMaximumWorks);

INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedTypes, CastTest, UnsignedTypePairs, /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedTypes, CastTest, SignedTypePairs, /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedUnsignedTypes, CastTest, SignedUnsignedTypePairs, /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedSignedTypes, CastTest, UnsignedSignedTypePairs, /* unused */);

TEST(CastErrorMinimumExceededTest, CastMinimumLimitExceededReturnsError)
{
    const auto min = std::numeric_limits<std::int16_t>::lowest();
    const auto value_to_be_cast = static_cast<std::int32_t>(min) - 1;

    const auto result = Cast<std::int16_t>(value_to_be_cast);
    ASSERT_FALSE(result.has_value()) << "Value: " << result.value();
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

TEST(CastErrorMaximumExceededTest, CastMaximumLimitExceededReturnsError)
{
    const auto max = std::numeric_limits<std::int16_t>::max();
    const auto value_to_be_cast = static_cast<std::int32_t>(max) + 1;

    const auto result = Cast<std::int16_t>(value_to_be_cast);
    ASSERT_FALSE(result.has_value()) << "Value: " << result.value();
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

TEST(FloatingToFloatingTest, CanCastNan)
{
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const auto result = Cast<double>(nan);
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(score::cpp::isnan(result.value()));
}

TEST(FloatingToFloatingTest, CanCastInfinity)
{
    const auto infinity = std::numeric_limits<double>::infinity();
    const auto result = Cast<double>(infinity);
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(score::cpp::isinf(result.value()));
}

TEST(FloatingToFloatingTest, CanCastNegativeInfinity)
{
    const auto infinity = -std::numeric_limits<double>::infinity();
    const auto result = Cast<double>(infinity);
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(score::cpp::signbit(result.value()));
    EXPECT_TRUE(score::cpp::isinf(result.value()));
}

TEST(FloatingToFloatingTest, ExceedingMaximumOfResultTypeWillReturnError)
{
    const auto max_float = static_cast<double>(std::numeric_limits<float>::max());
    const auto exceeding_float = score::cpp::nextafter(max_float, std::numeric_limits<double>::max());
    const auto result = Cast<float>(exceeding_float);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

TEST(FloatingToFloatingTest, ExceedingMinimumOfResultTypeWillReturnError)
{
    const auto min_float = static_cast<double>(std::numeric_limits<float>::lowest());
    const auto exceeding_float = score::cpp::nextafter(min_float, std::numeric_limits<double>::lowest());
    const auto result = Cast<float>(exceeding_float);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

TEST(FloatingToFloatingTest, CanCastFromDoubleToFloatWhenNotLoosingPrecision)
{
    const auto min_float = std::numeric_limits<float>::min();
    const auto result = Cast<float>(static_cast<double>(min_float));
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), min_float);
}

TEST(FloatingToFloatingTest, CastFromDoubleToFloatWillReturnErrorWhenLoosingPrecision)
{
    const auto min_float = std::numeric_limits<float>::min();
    const auto exceeding_float = score::cpp::nextafter(static_cast<double>(min_float), std::numeric_limits<double>::lowest());
    const auto result = Cast<float>(exceeding_float);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kImplicitRounding);
}

TEST(FloatingToIntegralTest, CanCastIntegralValue)
{
    const double to_be_casted{3.0};
    const auto result = Cast<std::uint8_t>(to_be_casted);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 3U);
}

TEST(FloatingToIntegralTest, CastExceedingMinimumWillReturnError)
{
    const double to_be_casted{-std::numeric_limits<double>::min()};
    const auto result = Cast<std::uint8_t>(to_be_casted);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

TEST(FloatingToIntegralTest, CastExceedingMaximumWillReturnError)
{
    const double to_be_casted{static_cast<double>(std::numeric_limits<std::uint8_t>::max()) + 1.0};
    const auto result = Cast<std::uint8_t>(to_be_casted);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

TEST(FloatingToIntegralTest, CastingNonIntegralValueReturnsError)
{
    const double integral_float{3.0};
    const auto non_integral_float = score::cpp::nextafter(integral_float, std::numeric_limits<double>::lowest());
    const auto result = Cast<std::uint8_t>(non_integral_float);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kImplicitRounding);
}

TEST(FloatingToIntegralTest, CastingNanReturnsError)
{
    const auto nan{std::numeric_limits<double>::quiet_NaN()};
    const auto result = Cast<std::uint8_t>(nan);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

TEST(FloatingToIntegralTest, CastingPositiveInfinityReturnsError)
{
    const auto infinity{std::numeric_limits<double>::infinity()};
    const auto result = Cast<std::uint8_t>(infinity);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

TEST(FloatingToIntegralTest, CastingNegativeInfinityReturnsError)
{
    const auto infinity{-std::numeric_limits<double>::infinity()};
    const auto result = Cast<std::uint8_t>(infinity);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

TEST(IntegralToFloatingTest, CastingRepresentableValueReturnsCastedValue)
{
    const auto representable_value{1U};
    const auto result = Cast<double>(representable_value);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1.0);
}

TEST(IntegralToFloatingTest, CastingUnrepresentableValueReturnsError)
{
    const auto unrepresentable_value{std::numeric_limits<std::uint64_t>::max()};
    const auto result = Cast<double>(unrepresentable_value);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kImplicitRounding);
}

}  // namespace
}  // namespace score::safe_math
