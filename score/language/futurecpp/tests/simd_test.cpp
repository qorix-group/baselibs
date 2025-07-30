///
/// \file
/// \copyright Copyright (C) 2020-2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/simd.hpp>
#include <score/simd.hpp> // test include guard

#include <score/assert_support.hpp>

#include <array>
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>

#include <gtest/gtest.h>

namespace score::cpp
{
namespace
{

template <typename T>
class generator
{
public:
    generator(const std::int32_t a, const std::int32_t b, const std::int32_t c, const std::int32_t d)
        : v_{static_cast<T>(a), static_cast<T>(b), static_cast<T>(c), static_cast<T>(d)}
    {
    }

    constexpr T operator()(std::integral_constant<std::size_t, 0U>) const noexcept { return std::get<0U>(v_); }
    constexpr T operator()(std::integral_constant<std::size_t, 1U>) const noexcept { return std::get<1U>(v_); }
    constexpr T operator()(std::integral_constant<std::size_t, 2U>) const noexcept { return std::get<2U>(v_); }
    constexpr T operator()(std::integral_constant<std::size_t, 3U>) const noexcept { return std::get<3U>(v_); }
    constexpr T operator[](std::size_t i) const { return v_[i]; }

private:
    std::array<T, 4U> v_;
};

class mask_generator
{
public:
    mask_generator(const bool a, const bool b, const bool c, const bool d) : v_{a, b, c, d} {}

    constexpr bool operator()(std::integral_constant<std::size_t, 0U>) const noexcept { return std::get<0U>(v_); }
    constexpr bool operator()(std::integral_constant<std::size_t, 1U>) const noexcept { return std::get<1U>(v_); }
    constexpr bool operator()(std::integral_constant<std::size_t, 2U>) const noexcept { return std::get<2U>(v_); }
    constexpr bool operator()(std::integral_constant<std::size_t, 3U>) const noexcept { return std::get<3U>(v_); }
    constexpr bool operator[](std::size_t i) const { return v_[i]; }

private:
    std::array<bool, 4U> v_;
};

template <typename T>
class simd_fixture : public testing::Test
{
};

using ElementTypes = ::testing::Types<std::int32_t, float, double>;
TYPED_TEST_SUITE(simd_fixture, ElementTypes, /*unused*/);

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, TrivialAndStandardLayout)
{
    static_assert(std::is_standard_layout<simd<TypeParam>>::value, "No standard layout.");
    static_assert(std::is_trivial<simd<TypeParam>>::value, "Not a trivial type.");
    static_assert(std::is_trivially_copyable<simd<TypeParam>>::value, "Not trivially copyable.");
    static_assert(std::is_trivially_default_constructible<simd<TypeParam>>::value,
                  "Not trivially default constructable.");
    static_assert(std::is_trivially_copy_constructible<simd<TypeParam>>::value, "Not trivially copy constructable.");
    static_assert(std::is_trivially_move_constructible<simd<TypeParam>>::value, "Not trivially move constructable.");
    static_assert(std::is_trivially_copy_assignable<simd<TypeParam>>::value, "Not trivially copy assignable.");
    static_assert(std::is_trivially_move_assignable<simd<TypeParam>>::value, "Not trivially move assignable.");
    static_assert(std::is_trivially_destructible<simd<TypeParam>>::value, "Not trivially destructable.");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Broadcast)
{
    const simd<TypeParam> a{TypeParam{23}};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(TypeParam{23}, a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Initialize)
{
    const generator<TypeParam> gen{1, 2, 3, 4};
    const simd<TypeParam> a{gen};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(gen[i], a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_fixture, InitializeUnaligned)
{
    const std::array<TypeParam, 4U> scalars{TypeParam{1}, TypeParam{2}, TypeParam{3}, TypeParam{4}};
    const simd<TypeParam> vector{scalars.data(), element_aligned};
    static_assert(vector.size() <= scalars.size(), "");

    for (std::size_t i{0U}; i < vector.size(); ++i)
    {
        EXPECT_EQ(scalars[i], vector[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_fixture, InitializeAligned)
{
    alignas(16) const std::array<TypeParam, 4U> scalars{TypeParam{1}, TypeParam{2}, TypeParam{3}, TypeParam{4}};
    const simd<TypeParam> vector{scalars.data(), vector_aligned};
    static_assert(vector.size() <= scalars.size(), "");

    for (std::size_t i{0U}; i < vector.size(); ++i)
    {
        EXPECT_EQ(scalars[i], vector[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_fixture, LoadUnaligned)
{
    simd<TypeParam> vector;
    const std::array<TypeParam, 4U> scalars{TypeParam{1}, TypeParam{2}, TypeParam{3}, TypeParam{4}};
    static_assert(vector.size() <= scalars.size(), "");
    vector.copy_from(scalars.data(), element_aligned);

    for (std::size_t i{0U}; i < vector.size(); ++i)
    {
        EXPECT_EQ(scalars[i], vector[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_fixture, LoadAligned)
{
    simd<TypeParam> vector;
    alignas(16) const std::array<TypeParam, 4U> scalars{TypeParam{1}, TypeParam{2}, TypeParam{3}, TypeParam{4}};
    static_assert(vector.size() <= scalars.size(), "");
    vector.copy_from(scalars.data(), vector_aligned);

    for (std::size_t i{0U}; i < vector.size(); ++i)
    {
        EXPECT_EQ(scalars[i], vector[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_fixture, LoadAligned_WhenCopyingFromUnalignedMemory_ThenPreconditionViolated)
{
    simd<TypeParam> vector;
    alignas(16) const std::array<TypeParam, vector.size() + 1> scalars{};

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.copy_from(&scalars[1], vector_aligned));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_fixture, StoreUnaligned)
{
    const generator<TypeParam> gen{1, 2, 3, 4};
    const simd<TypeParam> vector{gen};
    std::array<TypeParam, vector.size()> scalars;
    vector.copy_to(scalars.data(), element_aligned);

    for (std::size_t i{0U}; i < vector.size(); ++i)
    {
        EXPECT_EQ(gen[i], scalars[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_fixture, StoreAligned)
{
    const generator<TypeParam> gen{1, 2, 3, 4};
    const simd<TypeParam> vector{gen};
    alignas(16) std::array<TypeParam, vector.size()> scalars;
    vector.copy_to(scalars.data(), vector_aligned);

    for (std::size_t i{0U}; i < vector.size(); ++i)
    {
        EXPECT_EQ(gen[i], scalars[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_fixture, StoreAligned_WhenCopyingToUnalignedMemory_ThenPreconditionViolated)
{
    const simd<TypeParam> vector{TypeParam{23}};
    alignas(16) std::array<TypeParam, vector.size() + 1U> scalars;

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.copy_to(&scalars[1], vector_aligned));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Access_WhenOutOfBounds_ThenPreconditionViolated)
{
    const simd<TypeParam> a{TypeParam{23}};

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(a[a.size()]);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Add)
{
    const simd<TypeParam> one{1};

    EXPECT_TRUE(all_of(simd<TypeParam>{TypeParam{2}} == one + one));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TEST(simd, AddFloatSpecialValues)
{
    const simd<float> nan{std::numeric_limits<float>::quiet_NaN()};
    const simd<float> inf{std::numeric_limits<float>::infinity()};
    const simd<float> one{1.0F};

    EXPECT_TRUE(all_of(inf == one + inf));
    EXPECT_TRUE(all_of(is_nan(one + nan)));

    EXPECT_TRUE(all_of(inf == inf + inf));
    EXPECT_TRUE(all_of(-inf == -inf + -inf));

    EXPECT_TRUE(all_of(is_nan(inf + -inf)));
    EXPECT_TRUE(all_of(is_nan(-inf + inf)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, AssignmentAdd)
{
    simd<TypeParam> a{TypeParam{1}};
    a += simd<TypeParam>{TypeParam{1}};
    EXPECT_TRUE(all_of(simd<TypeParam>{TypeParam{2}} == a));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Subtract)
{
    const simd<TypeParam> one{TypeParam{1}};

    EXPECT_TRUE(all_of(simd<TypeParam>{TypeParam{0}} == one - one));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TEST(simd, SubtractFloatSpecialValues)
{
    const simd<float> nan{std::numeric_limits<float>::quiet_NaN()};
    const simd<float> inf{std::numeric_limits<float>::infinity()};
    const simd<float> one{1.0F};

    EXPECT_TRUE(all_of(simd<float>{0.0F} == one - one));
    EXPECT_TRUE(all_of(-inf == one - inf));
    EXPECT_TRUE(all_of(is_nan(one - nan)));

    EXPECT_TRUE(all_of(-inf == -inf - inf));
    EXPECT_TRUE(all_of(inf == inf - -inf));

    EXPECT_TRUE(all_of(is_nan(inf - inf)));
    EXPECT_TRUE(all_of(is_nan(-inf - -inf)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, AssignmentSubtract)
{
    simd<TypeParam> a{TypeParam{1}};
    a -= simd<TypeParam>{TypeParam{1}};
    EXPECT_TRUE(all_of(simd<TypeParam>{TypeParam{0}} == a));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Multiply)
{
    const simd<TypeParam> two{TypeParam{2}};

    EXPECT_TRUE(all_of(simd<TypeParam>{TypeParam{4}} == two * two));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TEST(simd, MultiplyFloatSpecialValues)
{
    const simd<float> nan{std::numeric_limits<float>::quiet_NaN()};
    const simd<float> inf{std::numeric_limits<float>::infinity()};
    const simd<float> two{2.0F};
    const simd<float> zero{0.0F};

    EXPECT_TRUE(all_of(simd<float>{4.0F} == two * two));
    EXPECT_TRUE(all_of(inf == two * inf));
    EXPECT_TRUE(all_of(is_nan(two * nan)));

    EXPECT_TRUE(all_of(is_nan(zero * inf)));
    EXPECT_TRUE(all_of(is_nan(-zero * inf)));
    EXPECT_TRUE(all_of(is_nan(zero * -inf)));
    EXPECT_TRUE(all_of(is_nan(-zero * -inf)));
    EXPECT_TRUE(all_of(is_nan(inf * zero)));
    EXPECT_TRUE(all_of(is_nan(-inf * zero)));
    EXPECT_TRUE(all_of(is_nan(inf * -zero)));
    EXPECT_TRUE(all_of(is_nan(-inf * -zero)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, AssignmentMultiply)
{
    simd<TypeParam> a{TypeParam{2}};
    a *= simd<TypeParam>{TypeParam{2}};
    EXPECT_TRUE(all_of(simd<TypeParam>{TypeParam{4}} == a));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Divide)
{
    const simd<TypeParam> two{TypeParam{2}};

    EXPECT_TRUE(all_of(simd<TypeParam>{TypeParam{1}} == two / two));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TEST(simd, DivideFloatSpecialValues)
{
    const simd<float> nan{std::numeric_limits<float>::quiet_NaN()};
    const simd<float> inf{std::numeric_limits<float>::infinity()};
    const simd<float> two{2.0F};
    const simd<float> zero{0.0F};

    EXPECT_TRUE(all_of(simd<float>{1.0F} == two / two));
    EXPECT_TRUE(all_of(zero == two / inf));
    EXPECT_TRUE(all_of(inf == two / zero));
    EXPECT_TRUE(all_of(is_nan(two / nan)));

    EXPECT_TRUE(all_of(is_nan(zero / zero)));
    EXPECT_TRUE(all_of(is_nan(-zero / zero)));
    EXPECT_TRUE(all_of(is_nan(zero / -zero)));
    EXPECT_TRUE(all_of(is_nan(-zero / -zero)));
    EXPECT_TRUE(all_of(is_nan(inf / inf)));
    EXPECT_TRUE(all_of(is_nan(-inf / inf)));
    EXPECT_TRUE(all_of(is_nan(inf / -inf)));
    EXPECT_TRUE(all_of(is_nan(-inf / -inf)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, AssignmentDivide)
{
    simd<TypeParam> a{TypeParam{2}};
    a /= simd<TypeParam>{TypeParam{2}};
    EXPECT_TRUE(all_of(simd<TypeParam>{TypeParam{1}} == a));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Negate)
{
    simd<TypeParam> a{TypeParam{23}};
    EXPECT_TRUE(all_of(simd<TypeParam>{TypeParam{-23}} == -a));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TEST(simd, NegateFloatSpecialValues)
{
    const simd<float> nan{std::numeric_limits<float>::quiet_NaN()};
    const simd<float> inf{std::numeric_limits<float>::infinity()};
    const simd<float> zero{0.0F};

    const std::uint32_t nan_bits{0x7FC00000};
    const std::uint32_t neg_nan_bits{0xFFC00000};
    const std::uint32_t inf_bits{0x7F800000};
    const std::uint32_t neg_inf_bits{0xFF800000};
    const std::uint32_t null_bits{0x00000000};
    const std::uint32_t neg_null_bits{0x80000000};
    EXPECT_EQ(nan_bits, score::cpp::bit_cast<std::uint32_t>(nan[0]));
    EXPECT_EQ(neg_nan_bits, score::cpp::bit_cast<std::uint32_t>((-nan)[0]));
    EXPECT_EQ(inf_bits, score::cpp::bit_cast<std::uint32_t>(inf[0]));
    EXPECT_EQ(neg_inf_bits, score::cpp::bit_cast<std::uint32_t>((-inf)[0]));
    EXPECT_EQ(null_bits, score::cpp::bit_cast<std::uint32_t>(zero[0]));
    EXPECT_EQ(neg_null_bits, score::cpp::bit_cast<std::uint32_t>((-zero)[0]));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Equal)
{
    const simd<TypeParam> one{TypeParam{1}};

    EXPECT_TRUE(all_of(one == one));
    EXPECT_TRUE(none_of(one == -one));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TEST(simd, EqualFloatSpecialValues)
{
    const simd<float> nan{std::numeric_limits<float>::quiet_NaN()};
    const simd<float> inf{std::numeric_limits<float>::infinity()};
    const simd<float> one{1.0F};

    EXPECT_TRUE(none_of(one == nan));
    EXPECT_TRUE(none_of(nan == one));
    EXPECT_TRUE(none_of(nan == inf));
    EXPECT_TRUE(none_of(inf == nan));
    EXPECT_TRUE(none_of(nan == -inf));
    EXPECT_TRUE(none_of(-inf == nan));
    EXPECT_TRUE(all_of(inf == inf));
    EXPECT_TRUE(none_of(-inf == inf));
    EXPECT_TRUE(none_of(inf == -inf));
    EXPECT_TRUE(all_of(-inf == -inf));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, NotEqual)
{
    const simd<TypeParam> one{TypeParam{1}};

    EXPECT_TRUE(none_of(one != one));
    EXPECT_TRUE(all_of(one != -one));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TEST(simd, NotEqualFloatSpecialValues)
{
    const simd<float> nan{std::numeric_limits<float>::quiet_NaN()};
    const simd<float> inf{std::numeric_limits<float>::infinity()};
    const simd<float> one{1.0F};

    EXPECT_TRUE(all_of(one != nan));
    EXPECT_TRUE(all_of(nan != one));
    EXPECT_TRUE(all_of(nan != inf));
    EXPECT_TRUE(all_of(inf != nan));
    EXPECT_TRUE(all_of(nan != -inf));
    EXPECT_TRUE(all_of(-inf != nan));
    EXPECT_TRUE(none_of(inf != inf));
    EXPECT_TRUE(all_of(-inf != inf));
    EXPECT_TRUE(all_of(inf != -inf));
    EXPECT_TRUE(none_of(-inf != -inf));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, LessThan)
{
    const simd<TypeParam> one{TypeParam{1}};

    EXPECT_TRUE(all_of(-one < one));
    EXPECT_TRUE(none_of(one < one));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TEST(simd, LessThanFloatSpecialValues)
{
    const simd<float> nan{std::numeric_limits<float>::quiet_NaN()};
    const simd<float> inf{std::numeric_limits<float>::infinity()};
    const simd<float> one{1.0F};

    EXPECT_TRUE(all_of(-one < one));
    EXPECT_TRUE(none_of(one < one));
    EXPECT_TRUE(none_of(one < nan));
    EXPECT_TRUE(none_of(nan < one));
    EXPECT_TRUE(none_of(nan < inf));
    EXPECT_TRUE(none_of(inf < nan));
    EXPECT_TRUE(none_of(nan < -inf));
    EXPECT_TRUE(none_of(-inf < nan));
    EXPECT_TRUE(none_of(inf < inf));
    EXPECT_TRUE(all_of(-inf < inf));
    EXPECT_TRUE(none_of(inf < -inf));
    EXPECT_TRUE(none_of(-inf < -inf));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, LessEqual)
{
    const simd<TypeParam> one{TypeParam{1}};

    EXPECT_TRUE(all_of(-one <= one));
    EXPECT_TRUE(all_of(one <= one));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TEST(simd, LessEqualFloatSpecialValues)
{
    const simd<float> nan{std::numeric_limits<float>::quiet_NaN()};
    const simd<float> inf{std::numeric_limits<float>::infinity()};
    const simd<float> one{1.0F};

    EXPECT_TRUE(none_of(one <= nan));
    EXPECT_TRUE(none_of(nan <= one));
    EXPECT_TRUE(none_of(nan <= inf));
    EXPECT_TRUE(none_of(inf <= nan));
    EXPECT_TRUE(none_of(nan <= -inf));
    EXPECT_TRUE(none_of(-inf <= nan));
    EXPECT_TRUE(all_of(inf <= inf));
    EXPECT_TRUE(all_of(-inf <= inf));
    EXPECT_TRUE(none_of(inf <= -inf));
    EXPECT_TRUE(all_of(-inf <= -inf));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, GreaterThan)
{
    const simd<TypeParam> one{TypeParam{1}};

    EXPECT_TRUE(all_of(one > -one));
    EXPECT_TRUE(none_of(one > one));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TEST(simd, GreaterThanFloatSpecialValues)
{
    const simd<float> nan{std::numeric_limits<float>::quiet_NaN()};
    const simd<float> inf{std::numeric_limits<float>::infinity()};
    const simd<float> one{1.0F};

    EXPECT_TRUE(none_of(one > nan));
    EXPECT_TRUE(none_of(nan > one));
    EXPECT_TRUE(none_of(nan > inf));
    EXPECT_TRUE(none_of(inf > nan));
    EXPECT_TRUE(none_of(nan > -inf));
    EXPECT_TRUE(none_of(-inf > nan));
    EXPECT_TRUE(none_of(inf > inf));
    EXPECT_TRUE(none_of(-inf > inf));
    EXPECT_TRUE(all_of(inf > -inf));
    EXPECT_TRUE(none_of(-inf > -inf));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, GreaterEqual)
{
    const simd<TypeParam> one{TypeParam{1}};

    EXPECT_TRUE(all_of(one >= -one));
    EXPECT_TRUE(all_of(one >= one));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TEST(simd, GreaterEqualFloatSpecialValues)
{
    const simd<float> nan{std::numeric_limits<float>::quiet_NaN()};
    const simd<float> inf{std::numeric_limits<float>::infinity()};
    const simd<float> one{1.0F};

    EXPECT_TRUE(none_of(one >= nan));
    EXPECT_TRUE(none_of(nan >= one));
    EXPECT_TRUE(none_of(inf >= nan));
    EXPECT_TRUE(none_of(-inf >= nan));
    EXPECT_TRUE(none_of(nan >= inf));
    EXPECT_TRUE(none_of(nan >= -inf));
    EXPECT_TRUE(all_of(inf >= inf));
    EXPECT_TRUE(none_of(-inf >= inf));
    EXPECT_TRUE(all_of(inf >= -inf));
    EXPECT_TRUE(all_of(-inf >= -inf));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Min)
{
    const simd<TypeParam> one{TypeParam{1}};

    EXPECT_TRUE(all_of(one == min(one, simd<TypeParam>{TypeParam{2}})));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TEST(simd, MinFloatSpecialValues)
{
    const simd<float> nan{std::numeric_limits<float>::quiet_NaN()};
    const simd<float> inf{std::numeric_limits<float>::infinity()};
    const simd<float> one{1.0F};

    EXPECT_TRUE(all_of(-inf == min(one, -inf)));
    EXPECT_TRUE(all_of(one == min(one, nan)));
    EXPECT_TRUE(all_of(is_nan(min(nan, one))));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Max)
{
    const simd<TypeParam> two{TypeParam{2}};

    EXPECT_TRUE(all_of(two == max(two, simd<TypeParam>{TypeParam{1}})));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TEST(simd, MaxSpecialFloatValues)
{
    const simd<float> nan{std::numeric_limits<float>::quiet_NaN()};
    const simd<float> inf{std::numeric_limits<float>::infinity()};
    const simd<float> two{2.0F};

    EXPECT_TRUE(all_of(inf == max(two, inf)));
    EXPECT_TRUE(all_of(two == max(two, nan)));
    EXPECT_TRUE(all_of(is_nan(max(nan, two))));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Clamp)
{
    const simd<TypeParam> one{TypeParam{1}};
    const simd<TypeParam> low{TypeParam{-1}};
    const simd<TypeParam> high{TypeParam{1}};

    EXPECT_TRUE(all_of(simd<TypeParam>{TypeParam{0}} == clamp(simd<TypeParam>{TypeParam{0}}, low, high)));
    EXPECT_TRUE(all_of(low == clamp(simd<TypeParam>{TypeParam{-2}}, low, high)));
    EXPECT_TRUE(all_of(high == clamp(simd<TypeParam>{TypeParam{2}}, low, high)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Clscore_future_cpp_WhenNoValidBoundaryInterval_ThenPreconditionViolated)
{
    const simd<TypeParam> one{TypeParam{1}};
    const simd<TypeParam> low{TypeParam{-1}};
    const simd<TypeParam> high{TypeParam{1}};

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(clamp(one, high, low));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TEST(simd, ConvertFloatToInt)
{
    EXPECT_TRUE(all_of(simd<std::int32_t>{-23} == static_simd_cast<simd<std::int32_t>>(simd<float>{-23.75F})));
    EXPECT_TRUE(all_of(simd<std::int32_t>{23} == static_simd_cast<simd<std::int32_t>>(simd<float>{23.0F})));
    EXPECT_TRUE(all_of(simd<std::int32_t>{23} == static_simd_cast<simd<std::int32_t>>(simd<float>{23.75F})));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TEST(simd, ConvertIntToFloat)
{
    EXPECT_TRUE(all_of(simd<float>{-23.0F} == static_simd_cast<simd<float>>(simd<std::int32_t>{-23})));
    EXPECT_TRUE(all_of(simd<float>{23.0F} == static_simd_cast<simd<float>>(simd<std::int32_t>{23})));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, WhereAssignment)
{
    simd<TypeParam> value{generator<TypeParam>{6, 9, 16, 25}};
    const simd_mask<TypeParam> mask{mask_generator{true, false, true, false}};

    where(mask, value) = simd<TypeParam>{generator<TypeParam>{2, 3, 4, 5}};

    EXPECT_TRUE(all_of(simd<TypeParam>{generator<TypeParam>{2, 9, 4, 25}} == value));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, WhereAssignmentAdd)
{
    simd<TypeParam> value{generator<TypeParam>{6, 9, 16, 25}};
    const simd_mask<TypeParam> mask{mask_generator{true, false, true, false}};

    where(mask, value) += simd<TypeParam>{generator<TypeParam>{2, 3, 4, 5}};

    EXPECT_TRUE(all_of(simd<TypeParam>{generator<TypeParam>{8, 9, 20, 25}} == value));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, WhereAssignmentSubtract)
{
    simd<TypeParam> value{generator<TypeParam>{6, 9, 16, 25}};
    const simd_mask<TypeParam> mask{mask_generator{true, false, true, false}};

    where(mask, value) -= simd<TypeParam>{generator<TypeParam>{2, 3, 4, 5}};

    EXPECT_TRUE(all_of(simd<TypeParam>{generator<TypeParam>{4, 9, 12, 25}} == value));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, WhereAssignmentMultiply)
{
    simd<TypeParam> value{generator<TypeParam>{6, 9, 16, 25}};
    const simd_mask<TypeParam> mask{mask_generator{true, false, true, false}};

    where(mask, value) *= simd<TypeParam>{generator<TypeParam>{2, 3, 4, 5}};

    EXPECT_TRUE(all_of(simd<TypeParam>{generator<TypeParam>{12, 9, 64, 25}} == value));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, WhereAssignmentDivide)
{
    simd<TypeParam> value{generator<TypeParam>{6, 9, 16, 25}};
    const simd_mask<TypeParam> mask{mask_generator{true, false, true, false}};

    where(mask, value) /= simd<TypeParam>{generator<TypeParam>{2, 3, 4, 5}};

    EXPECT_TRUE(all_of(simd<TypeParam>{generator<TypeParam>{3, 9, 4, 25}} == value));
}

} // namespace
} // namespace score::cpp
