///
/// \file
/// \copyright Copyright (C) 2020-2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/simd.hpp>
#include <score/simd.hpp> // test include guard

#include <gtest/gtest.h>

namespace score::cpp
{
namespace
{

template <typename T>
class simd_math_fixture : public testing::Test
{
};

using ElementTypes = ::testing::Types<float, double>;
TYPED_TEST_SUITE(simd_math_fixture, ElementTypes, /*unused*/);

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18397903
TYPED_TEST(simd_math_fixture, IsNan)
{
    const simd<TypeParam> nan{std::numeric_limits<TypeParam>::quiet_NaN()};
    const simd<TypeParam> inf{std::numeric_limits<TypeParam>::infinity()};
    const simd<TypeParam> one{static_cast<TypeParam>(1.0)};

    EXPECT_TRUE(none_of(is_nan(one)));
    EXPECT_TRUE(none_of(is_nan(inf)));
    EXPECT_TRUE(all_of(is_nan(nan)));
    EXPECT_TRUE(all_of(is_nan(-nan)));
}

} // namespace
} // namespace score::cpp
