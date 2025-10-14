///
/// \file
/// \copyright Copyright (C) 2020-2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/simd.hpp>
#include <score/simd.hpp> // test include guard

#include <array>
#include <cfenv>
#include <limits>

#include <gtest/gtest.h>

namespace score::cpp
{
namespace
{

template <typename T>
class generator
{
    using type = typename T::value_type;
    static constexpr std::size_t size{T::size()};

public:
    explicit generator(const std::array<type, size> v) : v_{v} {}

    template <std::size_t M>
    constexpr type operator()(std::integral_constant<std::size_t, M>) const noexcept
    {
        return std::get<std::integral_constant<std::size_t, M>{}()>(v_);
    }
    constexpr type operator[](std::size_t i) const { return v_[i]; }

private:
    std::array<type, size> v_;
};

template <typename T>
class simd_math_fixture : public testing::Test
{
};

using ElementTypes = ::testing::Types<simd<float>, simd<double>>;
TYPED_TEST_SUITE(simd_math_fixture, ElementTypes, /*unused*/);

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18397903
TYPED_TEST(simd_math_fixture, GivenNan_ExpectIsNanIsTrue)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    EXPECT_TRUE(all_of(is_nan(nan)));
    EXPECT_TRUE(all_of(is_nan(-nan)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18397903
TYPED_TEST(simd_math_fixture, GivenNanInOneLane_ExpectIsNanIsTrueOnlyForThisLane)
{
    using value_type = typename TypeParam::value_type;
    for (std::size_t i{0U}; i < TypeParam::size(); ++i)
    {
        std::array<value_type, TypeParam::size()> in{};
        in[i] = std::numeric_limits<value_type>::quiet_NaN();

        const TypeParam a{generator<TypeParam>{in}};
        const auto r = is_nan(a);

        for (std::size_t j{0U}; j < a.size(); ++j)
        {
            if (j == i)
            {
                EXPECT_TRUE(r[j]);
            }
            else
            {
                EXPECT_FALSE(r[j]);
            }
        }
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18397903
TYPED_TEST(simd_math_fixture, GivenSignalingNan_ExpectNoFpuExceptionRaised)
{
#if !defined(__SSE4_2__) && !defined(__ARM_NEON)
    GTEST_SKIP() << "not IEE754";
#endif

    EXPECT_EQ(std::feclearexcept(FE_ALL_EXCEPT), 0);

    using value_type = typename TypeParam::value_type;
    const TypeParam nan{std::numeric_limits<value_type>::signaling_NaN()};
    EXPECT_TRUE(all_of(is_nan(nan)));
    EXPECT_TRUE(all_of(is_nan(-nan)));

    EXPECT_EQ(std::fetestexcept(FE_ALL_EXCEPT), 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18397903
TYPED_TEST(simd_math_fixture, GivenInf_ExpectIsNanIsFalse)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    EXPECT_TRUE(none_of(is_nan(inf)));
    EXPECT_TRUE(none_of(is_nan(-inf)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18397903
TYPED_TEST(simd_math_fixture, GivenDenorm_ExpectIsNanIsFalse)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam denorm{std::numeric_limits<value_type>::denorm_min()};
    EXPECT_TRUE(none_of(is_nan(denorm)));
    EXPECT_TRUE(none_of(is_nan(-denorm)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18397903
TYPED_TEST(simd_math_fixture, GivenMax_ExpectIsNanIsFalse)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam lowest{std::numeric_limits<value_type>::lowest()};
    const TypeParam max{std::numeric_limits<value_type>::max()};
    EXPECT_TRUE(none_of(is_nan(lowest)));
    EXPECT_TRUE(none_of(is_nan(max)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18397903
TYPED_TEST(simd_math_fixture, GivenMin_ExpectIsNanIsFalse)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam min{std::numeric_limits<value_type>::min()};
    EXPECT_TRUE(none_of(is_nan(min)));
    EXPECT_TRUE(none_of(is_nan(-min)));
}

} // namespace
} // namespace score::cpp
