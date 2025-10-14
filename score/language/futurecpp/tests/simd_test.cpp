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
#include <numeric>
#include <type_traits>

#include <gtest/gtest.h>

namespace score::cpp
{
namespace
{

/// @brief returns a sequence 1,2,3...
template <typename TypeParam>
auto integer_sequence()
{
    std::array<typename TypeParam::value_type, TypeParam::size()> v;
    std::iota(v.begin(), v.end(), typename TypeParam::value_type{1});
    return v;
}

/// @brief returns a sequence ...-3,-2,-1
template <typename TypeParam>
auto negative_integer_sequence()
{
    std::array<typename TypeParam::value_type, TypeParam::size()> v;
    std::iota(v.begin(), v.end(), -static_cast<std::ptrdiff_t>(TypeParam::size()));
    return v;
}

/// @brief returns a sequence 1,-2,3,-4...
template <typename TypeParam>
auto alternating_integer_sequence()
{
    std::array<typename TypeParam::value_type, TypeParam::size()> v{integer_sequence<TypeParam>()};
    for (std::size_t i{1U}; i < v.size(); i += 2)
    {
        v[i] = -v[i];
    }
    return v;
}

/// @brief returns a sequence true,false,true,false...
template <std::size_t N>
auto alternating_boolean_sequence()
{
    std::array<bool, N> v;

    bool start{false};
    for (std::size_t i{0U}; i < v.size(); ++i)
    {
        start = !start;
        v[i] = start;
    }
    return v;
}

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
class simd_fixture : public testing::Test
{
};

using ElementTypes = ::testing::Types<simd<std::int32_t>, simd<float>, simd<double>>;
TYPED_TEST_SUITE(simd_fixture, ElementTypes, /*unused*/);

template <typename T>
class simd_floating_point_fixture : public testing::Test
{
};

using ElementFloatingPointTypes = ::testing::Types<simd<float>, simd<double>>;
TYPED_TEST_SUITE(simd_floating_point_fixture, ElementFloatingPointTypes, /*unused*/);

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, TrivialAndStandardLayout)
{
    static_assert(std::is_standard_layout<TypeParam>::value, "failed");
    static_assert(std::is_trivial<TypeParam>::value, "failed");
    static_assert(std::is_trivially_copyable<TypeParam>::value, "failed");
    static_assert(std::is_trivially_default_constructible<TypeParam>::value, "failed");
    static_assert(std::is_trivially_copy_constructible<TypeParam>::value, "failed");
    static_assert(std::is_trivially_move_constructible<TypeParam>::value, "failed");
    static_assert(std::is_trivially_copy_assignable<TypeParam>::value, "failed");
    static_assert(std::is_trivially_move_assignable<TypeParam>::value, "failed");
    static_assert(std::is_trivially_destructible<TypeParam>::value, "failed");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Broadcast)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam a{value_type{23}};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(value_type{23}, a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, InitializeWithGenerator)
{
    const generator<TypeParam> gen{integer_sequence<TypeParam>()};
    const TypeParam a{gen};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(gen[i], a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_fixture, InitializeByDefaultIsUnaligned)
{
    const auto scalars{integer_sequence<TypeParam>()};
    const TypeParam vector{scalars.data()};

    for (std::size_t i{0U}; i < vector.size(); ++i)
    {
        EXPECT_EQ(scalars[i], vector[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_fixture, InitializeUnaligned)
{
    const auto scalars{integer_sequence<TypeParam>()};
    const TypeParam vector{scalars.data(), element_aligned};

    for (std::size_t i{0U}; i < vector.size(); ++i)
    {
        EXPECT_EQ(scalars[i], vector[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_fixture, InitializeAligned)
{
    alignas(16) const auto scalars{integer_sequence<TypeParam>()};
    const TypeParam vector{scalars.data(), vector_aligned};

    for (std::size_t i{0U}; i < vector.size(); ++i)
    {
        EXPECT_EQ(scalars[i], vector[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_fixture, LoadByDefaultIsUnaligned)
{
    const auto scalars{integer_sequence<TypeParam>()};
    TypeParam vector;
    vector.copy_from(scalars.data());

    for (std::size_t i{0U}; i < vector.size(); ++i)
    {
        EXPECT_EQ(scalars[i], vector[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_fixture, LoadUnaligned)
{
    const auto scalars{integer_sequence<TypeParam>()};
    TypeParam vector;
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
    alignas(16) const auto scalars{integer_sequence<TypeParam>()};
    TypeParam vector;
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
    TypeParam vector;
    alignas(16) const std::array<typename TypeParam::value_type, vector.size() + 1> scalars{};

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.copy_from(&scalars[1], vector_aligned));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_fixture, StoreByDefaultIsUnaligned)
{
    const auto scalars{integer_sequence<TypeParam>()};
    const TypeParam vector{scalars.data()};
    std::array<typename TypeParam::value_type, vector.size()> result;
    vector.copy_to(result.data());

    EXPECT_EQ(result, scalars);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_fixture, StoreUnaligned)
{
    const auto scalars{integer_sequence<TypeParam>()};
    const TypeParam vector{scalars.data()};
    std::array<typename TypeParam::value_type, vector.size()> result;
    vector.copy_to(result.data(), element_aligned);

    EXPECT_EQ(result, scalars);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_fixture, StoreAligned)
{
    const auto scalars{integer_sequence<TypeParam>()};
    const TypeParam vector{scalars.data()};
    alignas(16) std::array<typename TypeParam::value_type, vector.size()> result;
    vector.copy_to(result.data(), vector_aligned);

    EXPECT_EQ(result, scalars);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_fixture, StoreAligned_WhenCopyingToUnalignedMemory_ThenPreconditionViolated)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam vector{value_type{23}};
    alignas(16) std::array<value_type, vector.size() + 1U> scalars;

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.copy_to(&scalars[1], vector_aligned));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Access_WhenOutOfBounds_ThenPreconditionViolated)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam a{value_type{23}};

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(a[a.size()]);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Add)
{
    using value_type = typename TypeParam::value_type;
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq.data()};

    const auto r = a + TypeParam{value_type{1}};
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(seq[i] + value_type{1}, r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, AddFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam one{value_type{1}};

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
    using value_type = typename TypeParam::value_type;
    const auto seq = integer_sequence<TypeParam>();
    TypeParam a{seq.data()};
    a += TypeParam{value_type{1}};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(seq[i] + value_type{1}, a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Subtract)
{
    using value_type = typename TypeParam::value_type;
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq.data()};

    const auto r = a - TypeParam{value_type{1}};
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(seq[i] - value_type{1}, r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, SubtractFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam one{value_type{1}};

    EXPECT_TRUE(all_of(TypeParam{value_type{0}} == one - one));
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
    using value_type = typename TypeParam::value_type;
    const auto seq = integer_sequence<TypeParam>();
    TypeParam a{seq.data()};
    a -= TypeParam{value_type{1}};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(seq[i] - value_type{1}, a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Multiply)
{
    using value_type = typename TypeParam::value_type;
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq.data()};

    const auto r = a * TypeParam{value_type{2}};
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(seq[i] * value_type{2}, r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, MultiplyFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam two{value_type{2}};
    const TypeParam zero{value_type{0}};

    EXPECT_TRUE(all_of(TypeParam{value_type{4}} == two * two));
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
    using value_type = typename TypeParam::value_type;
    const auto seq = integer_sequence<TypeParam>();
    TypeParam a{seq.data()};
    a *= TypeParam{value_type{2}};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(seq[i] * value_type{2}, a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Divide)
{
    using value_type = typename TypeParam::value_type;
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq.data()};

    const auto r = a / TypeParam{value_type{2}};
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(seq[i] / value_type{2}, r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, DivideFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam two{value_type{2}};
    const TypeParam zero{value_type{0}};

    EXPECT_TRUE(all_of(TypeParam{value_type{1}} == two / two));
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
    using value_type = typename TypeParam::value_type;
    const auto seq = integer_sequence<TypeParam>();
    TypeParam a{seq.data()};
    a /= TypeParam{value_type{2}};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(seq[i] / value_type{2}, a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Negate)
{
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq.data()};

    const auto r = -a;
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(-seq[i], r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, NegateFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;

    const value_type nan{std::numeric_limits<value_type>::quiet_NaN()};
    const value_type inf{std::numeric_limits<value_type>::infinity()};
    const value_type zero{0};

    static_assert(std::is_floating_point<value_type>::value, "failed");
    using bits = std::conditional_t<std::is_same<float, value_type>::value, std::uint32_t, std::uint64_t>;

    for (std::size_t i{0U}; i < TypeParam::size(); ++i)
    {
        EXPECT_EQ(score::cpp::bit_cast<bits>(nan), score::cpp::bit_cast<bits>(TypeParam{nan}[i]));
        EXPECT_EQ(score::cpp::bit_cast<bits>(-nan), score::cpp::bit_cast<bits>(TypeParam{-nan}[i]));
        EXPECT_EQ(score::cpp::bit_cast<bits>(inf), score::cpp::bit_cast<bits>(TypeParam{inf}[i]));
        EXPECT_EQ(score::cpp::bit_cast<bits>(-inf), score::cpp::bit_cast<bits>(TypeParam{-inf}[i]));
        EXPECT_EQ(score::cpp::bit_cast<bits>(zero), score::cpp::bit_cast<bits>(TypeParam{zero}[i]));
        EXPECT_EQ(score::cpp::bit_cast<bits>(-zero), score::cpp::bit_cast<bits>(TypeParam{-zero}[i]));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Equal_SameValue)
{
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq.data()};

    const auto r = a == a;
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_TRUE(r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Equal_DifferentValue)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = alternating_integer_sequence<TypeParam>();

    const TypeParam a{seq_a.data()};
    const TypeParam b{seq_b.data()};

    const auto r = a == b;
    const auto expected = alternating_boolean_sequence<TypeParam::size()>();
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(r[i], expected[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, EqualFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;

    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam one{value_type{1}};

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
TYPED_TEST(simd_fixture, NotEqual_SameValue)
{
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq.data()};

    const auto r = a != a;
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_FALSE(r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, NotEqual_DifferentValue)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = alternating_integer_sequence<TypeParam>();

    const TypeParam a{seq_a.data()};
    const TypeParam b{seq_b.data()};

    const auto r = a != b;
    const auto expected = alternating_boolean_sequence<TypeParam::size()>();
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(r[i], !expected[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, NotEqualFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;

    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam one{value_type{1}};

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
TYPED_TEST(simd_fixture, LessThan_SameValue)
{
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq.data()};

    const auto r = a < a;
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_FALSE(r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, LessThan_DifferentValue)
{
    const auto seq_a = alternating_integer_sequence<TypeParam>();
    const auto seq_b = integer_sequence<TypeParam>();

    const TypeParam a{seq_a.data()};
    const TypeParam b{seq_b.data()};

    const auto r = a < b;
    const auto expected = alternating_boolean_sequence<TypeParam::size()>();
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(r[i], !expected[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, LessThanFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;

    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam one{value_type{1}};

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
TYPED_TEST(simd_fixture, LessEqual_SameValue)
{
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq.data()};

    const auto r = a <= a;
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_TRUE(r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, LessEqual_DifferentValue)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = alternating_integer_sequence<TypeParam>();

    const TypeParam a{seq_a.data()};
    const TypeParam b{seq_b.data()};

    const auto r = a <= b;
    const auto expected = alternating_boolean_sequence<TypeParam::size()>();
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(r[i], expected[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, LessEqualFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;

    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam one{value_type{1}};

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
TYPED_TEST(simd_fixture, GreateThan_SameValue)
{
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq.data()};

    const auto r = a > a;
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_FALSE(r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, GreaterThan_DifferentValue)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = alternating_integer_sequence<TypeParam>();

    const TypeParam a{seq_a.data()};
    const TypeParam b{seq_b.data()};

    const auto r = a > b;
    const auto expected = alternating_boolean_sequence<TypeParam::size()>();
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(r[i], !expected[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, GreaterThanFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;

    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam one{value_type{1}};

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
TYPED_TEST(simd_fixture, GreateEqual_SameValue)
{
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq.data()};

    const auto r = a >= a;
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_TRUE(r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, GreaterEqual_DifferentValue)
{
    const auto seq_a = alternating_integer_sequence<TypeParam>();
    const auto seq_b = integer_sequence<TypeParam>();

    const TypeParam a{seq_a.data()};
    const TypeParam b{seq_b.data()};

    const auto r = a >= b;
    const auto expected = alternating_boolean_sequence<TypeParam::size()>();
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(r[i], expected[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, GreaterEqualFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;

    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam one{value_type{1}};

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
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = negative_integer_sequence<TypeParam>();

    const TypeParam a{seq_a.data()};
    const TypeParam b{seq_b.data()};

    const auto r = min(a, b);
    const auto r_swapped = min(b, a);
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(r[i], std::min(seq_a[i], seq_b[i]));
        EXPECT_EQ(r_swapped[i], r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, MinFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;

    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam one{value_type{1}};

    EXPECT_TRUE(all_of(-inf == min(one, -inf)));
    EXPECT_TRUE(all_of(one == min(one, nan)));
    EXPECT_TRUE(all_of(is_nan(min(nan, one))));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Max)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = negative_integer_sequence<TypeParam>();

    const TypeParam a{seq_a.data()};
    const TypeParam b{seq_b.data()};

    const auto r = max(a, b);
    const auto r_swapped = max(b, a);
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(r[i], std::max(seq_a[i], seq_b[i]));
        EXPECT_EQ(r_swapped[i], r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, MaxSpecialFloatValues)
{
    using value_type = typename TypeParam::value_type;

    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam two{value_type{2}};

    EXPECT_TRUE(all_of(inf == max(two, inf)));
    EXPECT_TRUE(all_of(two == max(two, nan)));
    EXPECT_TRUE(all_of(is_nan(max(nan, two))));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Clamp)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam one{value_type{1}};
    const TypeParam low{value_type{-1}};
    const TypeParam high{value_type{1}};

    EXPECT_TRUE(all_of(TypeParam{value_type{0}} == clamp(TypeParam{value_type{0}}, low, high)));
    EXPECT_TRUE(all_of(low == clamp(TypeParam{value_type{-2}}, low, high)));
    EXPECT_TRUE(all_of(high == clamp(TypeParam{value_type{2}}, low, high)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, Clscore_future_cpp_WhenNoValidBoundaryInterval_ThenPreconditionViolated)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam one{value_type{1}};
    const TypeParam low{value_type{-1}};
    const TypeParam high{value_type{1}};

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(clamp(one, high, low));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TEST(simd, ConvertFloatToInt)
{
    EXPECT_TRUE(all_of(simd<std::int32_t>{-23} == static_simd_cast<simd<std::int32_t>>(simd<float>{-23.75F})));
    EXPECT_TRUE(all_of(simd<std::int32_t>{23} == static_simd_cast<simd<std::int32_t>>(simd<float>{23.0F})));
    EXPECT_TRUE(all_of(simd<std::int32_t>{23} == static_simd_cast<simd<std::int32_t>>(simd<float>{23.75F})));
    EXPECT_TRUE(all_of(simd<std::int32_t>{-23} == simd<std::int32_t>{simd<float>{-23.75F}}));
    EXPECT_TRUE(all_of(simd<std::int32_t>{23} == simd<std::int32_t>{simd<float>{23.0F}}));
    EXPECT_TRUE(all_of(simd<std::int32_t>{23} == simd<std::int32_t>{simd<float>{23.75F}}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TEST(simd, ConvertIntToFloat)
{
    EXPECT_TRUE(all_of(simd<float>{-23.0F} == static_simd_cast<simd<float>>(simd<std::int32_t>{-23})));
    EXPECT_TRUE(all_of(simd<float>{23.0F} == static_simd_cast<simd<float>>(simd<std::int32_t>{23})));
    EXPECT_TRUE(all_of(simd<float>{-23.0F} == simd<float>{simd<std::int32_t>{-23}}));
    EXPECT_TRUE(all_of(simd<float>{23.0F} == simd<float>{simd<std::int32_t>{23}}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, WhereAssignment)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = negative_integer_sequence<TypeParam>();
    const auto seq_mask = alternating_boolean_sequence<TypeParam::size()>();

    TypeParam a{seq_a.data()};
    const typename TypeParam::mask_type mask{generator<typename TypeParam::mask_type>{seq_mask}};

    where(mask, a) = TypeParam{seq_b.data()};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(a[i], seq_mask[i] ? seq_b[i] : seq_a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, WhereAssignmentAdd)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = negative_integer_sequence<TypeParam>();
    const auto seq_mask = alternating_boolean_sequence<TypeParam::size()>();

    TypeParam a{seq_a.data()};
    const typename TypeParam::mask_type mask{generator<typename TypeParam::mask_type>{seq_mask}};

    where(mask, a) += TypeParam{seq_b.data()};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(a[i], seq_mask[i] ? seq_a[i] + seq_b[i] : seq_a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, WhereAssignmentSubtract)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = negative_integer_sequence<TypeParam>();
    const auto seq_mask = alternating_boolean_sequence<TypeParam::size()>();

    TypeParam a{seq_a.data()};
    const typename TypeParam::mask_type mask{generator<typename TypeParam::mask_type>{seq_mask}};

    where(mask, a) -= TypeParam{seq_b.data()};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(a[i], seq_mask[i] ? seq_a[i] - seq_b[i] : seq_a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, WhereAssignmentMultiply)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = negative_integer_sequence<TypeParam>();
    const auto seq_mask = alternating_boolean_sequence<TypeParam::size()>();

    TypeParam a{seq_a.data()};
    const typename TypeParam::mask_type mask{generator<typename TypeParam::mask_type>{seq_mask}};

    where(mask, a) *= TypeParam{seq_b.data()};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(a[i], seq_mask[i] ? seq_a[i] * seq_b[i] : seq_a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_fixture, WhereAssignmentDivide)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = negative_integer_sequence<TypeParam>();
    const auto seq_mask = alternating_boolean_sequence<TypeParam::size()>();

    TypeParam a{seq_a.data()};
    const typename TypeParam::mask_type mask{generator<typename TypeParam::mask_type>{seq_mask}};

    where(mask, a) /= TypeParam{seq_b.data()};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(a[i], seq_mask[i] ? seq_a[i] / seq_b[i] : seq_a[i]);
    }
}

} // namespace
} // namespace score::cpp
