///
/// \file
/// \copyright Copyright (C) 2020-2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/simd.hpp>
#include <score/simd.hpp> // test include guard

#include <score/assert_support.hpp>

#include <array>

#include <gtest/gtest.h>

namespace score::cpp
{
namespace
{

class generator
{
public:
    generator(const bool a, const bool b, const bool c, const bool d) : v_{a, b, c, d} {}

    constexpr bool operator()(std::integral_constant<std::size_t, 0U>) const noexcept { return std::get<0U>(v_); }
    constexpr bool operator()(std::integral_constant<std::size_t, 1U>) const noexcept { return std::get<1U>(v_); }
    constexpr bool operator()(std::integral_constant<std::size_t, 2U>) const noexcept { return std::get<2U>(v_); }
    constexpr bool operator()(std::integral_constant<std::size_t, 3U>) const noexcept { return std::get<3U>(v_); }
    constexpr bool operator[](std::size_t i) const { return v_[i]; }

private:
    std::array<bool, 4U> v_;
};

template <typename T>
class simd_mask_fixture : public testing::Test
{
};

using ElementTypes = ::testing::Types<std::int32_t, float, double>;
TYPED_TEST_SUITE(simd_mask_fixture, ElementTypes, /*unused*/);

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398051
TYPED_TEST(simd_mask_fixture, Broadcast)
{
    const simd_mask<TypeParam> a{true};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_TRUE(a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398051
TYPED_TEST(simd_mask_fixture, Initialize)
{
    {
        const generator gen{true, false, false, false};
        const simd_mask<TypeParam> a{gen};

        for (std::size_t i{0U}; i < a.size(); ++i)
        {
            EXPECT_EQ(gen[i], a[i]);
        }
    }
    {
        const generator gen{false, true, false, false};
        const simd_mask<TypeParam> a{gen};

        for (std::size_t i{0U}; i < a.size(); ++i)
        {
            EXPECT_EQ(gen[i], a[i]);
        }
    }
    {
        const generator gen{false, false, true, false};
        const simd_mask<TypeParam> a{gen};

        for (std::size_t i{0U}; i < a.size(); ++i)
        {
            EXPECT_EQ(gen[i], a[i]);
        }
    }
    {
        const generator gen{false, false, false, true};
        const simd_mask<TypeParam> a{gen};

        for (std::size_t i{0U}; i < a.size(); ++i)
        {
            EXPECT_EQ(gen[i], a[i]);
        }
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398051
TYPED_TEST(simd_mask_fixture, Access_WhenOutOfBounds_ThenPreconditionViolated)
{
    const simd_mask<TypeParam> a{false};

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(a[a.size()]);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398051
TYPED_TEST(simd_mask_fixture, Not)
{
    {
        const simd_mask<TypeParam> a{true};
        EXPECT_TRUE(none_of(!a));
    }
    {
        const simd_mask<TypeParam> a{false};
        EXPECT_TRUE(all_of(!a));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398051
TYPED_TEST(simd_mask_fixture, And)
{
    {
        const simd_mask<TypeParam> a{true};
        EXPECT_TRUE(all_of(a && a));
    }
    {
        const simd_mask<TypeParam> a{true};
        EXPECT_TRUE(none_of(a && !a));
    }
    {
        const simd_mask<TypeParam> a{true};
        EXPECT_TRUE(none_of(!a && a));
    }
    {
        const simd_mask<TypeParam> a{false};
        EXPECT_TRUE(none_of(a && a));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398051
TYPED_TEST(simd_mask_fixture, Or)
{
    {
        const simd_mask<TypeParam> a{true};
        EXPECT_TRUE(all_of(a || a));
    }
    {
        const simd_mask<TypeParam> a{true};
        EXPECT_TRUE(all_of(a || !a));
    }
    {
        const simd_mask<TypeParam> a{true};
        EXPECT_TRUE(all_of(!a || a));
    }
    {
        const simd_mask<TypeParam> a{false};
        EXPECT_TRUE(none_of(a || a));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398051
TYPED_TEST(simd_mask_fixture, AllOf)
{
    {
        const simd_mask<TypeParam> a{generator{false, false, false, false}};
        EXPECT_FALSE(all_of(a));
    }
    {
        const simd_mask<TypeParam> a{generator{false, false, false, true}};
        EXPECT_FALSE(all_of(a));
    }
    {
        const simd_mask<TypeParam> a{generator{false, false, true, true}};
        EXPECT_FALSE(all_of(a));
    }
    {
        const simd_mask<TypeParam> a{generator{false, true, true, true}};
        EXPECT_FALSE(all_of(a));
    }
    {
        const simd_mask<TypeParam> a{generator{true, true, true, true}};
        EXPECT_TRUE(all_of(a));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398051
TYPED_TEST(simd_mask_fixture, AnyOf)
{
    {
        const simd_mask<TypeParam> a{generator{false, false, false, false}};
        EXPECT_FALSE(any_of(a));
    }
    {
        const simd_mask<TypeParam> a{generator{true, false, false, false}};
        EXPECT_TRUE(any_of(a));
    }
    {
        const simd_mask<TypeParam> a{generator{true, true, false, false}};
        EXPECT_TRUE(any_of(a));
    }
    {
        const simd_mask<TypeParam> a{generator{true, true, true, false}};
        EXPECT_TRUE(any_of(a));
    }
    {
        const simd_mask<TypeParam> a{generator{true, true, true, true}};
        EXPECT_TRUE(any_of(a));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398051
TYPED_TEST(simd_mask_fixture, NoneOf)
{
    {
        const simd_mask<TypeParam> a{generator{false, false, false, false}};
        EXPECT_TRUE(none_of(a));
    }
    {
        const simd_mask<TypeParam> a{generator{true, false, false, false}};
        EXPECT_FALSE(none_of(a));
    }
    {
        const simd_mask<TypeParam> a{generator{true, true, false, false}};
        EXPECT_FALSE(none_of(a));
    }
    {
        const simd_mask<TypeParam> a{generator{true, true, true, false}};
        EXPECT_FALSE(none_of(a));
    }
    {
        const simd_mask<TypeParam> a{generator{true, true, true, true}};
        EXPECT_FALSE(none_of(a));
    }
}

} // namespace
} // namespace score::cpp
