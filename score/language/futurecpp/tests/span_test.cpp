///
/// @file
/// @copyright Copyright (C) 2018-2020, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/span.hpp>
#include <score/span.hpp> //test include guard

#include <score/assert_support.hpp>
#include <score/size.hpp>
#include <score/utility.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace score::cpp
{
namespace
{

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, DefaultConstruction)
{
    span<const std::int32_t> view{};

    EXPECT_TRUE(view.empty());
    EXPECT_EQ(nullptr, view.data());
    EXPECT_EQ(0U, view.size());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, OneDimensionalCArray)
{
    std::int32_t data[]{23, 42, 72};
    span<const std::int32_t> view{data};

    EXPECT_EQ(3U, view.size());
    EXPECT_EQ(3, (view.end() - view.begin()));
    EXPECT_EQ(72, view.data()[2]);
    EXPECT_EQ(72, at(view, 2));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, OneDimensionalPointerSize)
{
    std::int32_t data[]{23, 42, 72};
    span<const std::int32_t> view{&data[0], score::cpp::size(data)};

    EXPECT_EQ(3U, view.size());
    EXPECT_EQ(3, (view.end() - view.begin()));
    EXPECT_EQ(72, view.data()[2]);
    EXPECT_EQ(72, at(view, 2));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, OneDimensionalConst)
{
    const std::int32_t data[]{23, 42, 72};
    const span<const std::int32_t> view{data};

    EXPECT_EQ(3U, view.size());
    EXPECT_EQ(3, (view.end() - view.begin()));
    EXPECT_EQ(72, view.data()[2]);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, CopyConstruction)
{
    const std::vector<std::int32_t> data{12, 5, -5, -6};
    const span<const std::int32_t> view{data};
    const span<const std::int32_t> view2{view};

    EXPECT_EQ(view2.size(), data.size());
    EXPECT_EQ(view2.data(), data.data());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, MoveConstruction)
{
    const std::vector<std::int32_t> data{12, 5, -5, -6};
    const span<const std::int32_t> view{data};
    const span<const std::int32_t> view2{std::move(view)};

    EXPECT_EQ(view2.size(), data.size());
    EXPECT_EQ(view2.data(), data.data());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, Empty)
{
    {
        std::vector<std::int32_t> data{};
        const span<const std::int32_t> view{data.data(), data.size()};
        EXPECT_TRUE(view.empty());
    }
    {
        std::vector<std::int32_t> data{12, 5, -5, -6};
        const span<const std::int32_t> view{data.data(), data.size()};
        EXPECT_FALSE(view.empty());
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, DereferencingNullptrShallTriggerContracViolation)
{
    {
        std::vector<std::int32_t> data{};
        ASSERT_EQ(data.data(), nullptr);
        const span<const std::int32_t> view{data.data(), data.size()};
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(*view.begin());
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(view.begin().operator->());
    }
    {
        std::vector<std::int32_t> data{24, 15, 2};
        ASSERT_NE(data.data(), nullptr);
        const span<const std::int32_t> view{data.data(), data.size()};
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(*view.begin());
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(view.begin().operator->());
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, AsBytes)
{
    const std::int32_t data[]{0x0A0A0A0A, 0x0B0B0B0B};
    const span<const std::int32_t> view{data};
    EXPECT_EQ(2U, view.size());

    const span<const std::uint8_t> bytes{as_bytes(view)};
    EXPECT_EQ(8U, bytes.size());
    const std::array<std::uint8_t, 8U> expected{{0x0A, 0x0A, 0x0A, 0x0A, 0x0B, 0x0B, 0x0B, 0x0B}};
    EXPECT_TRUE(std::equal(expected.begin(), expected.end(), bytes.begin()));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, ReverseIterator)
{
    std::int32_t data[4]{1, 2, 3, 4};
    const span<std::int32_t> view{data};

    const std::int32_t expected[4]{4, 3, 2, 1};

    // member function
    EXPECT_TRUE(std::equal(view.rbegin(), view.rend(), std::cbegin(expected), std::cend(expected)));

    // free function
    EXPECT_TRUE(std::equal(std::rbegin(view), std::rend(view), std::cbegin(expected), std::cend(expected)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, ConstReverseIterator)
{
    const std::int32_t data[4]{1, 2, 3, 4};
    const span<const std::int32_t> view{data};

    const std::int32_t expected[4]{4, 3, 2, 1};

    // member function (const)
    EXPECT_TRUE(std::equal(view.crbegin(), view.crend(), std::cbegin(expected), std::cend(expected)));

    // free function (const)
    EXPECT_TRUE(std::equal(std::crbegin(view), std::crend(view), std::cbegin(expected), std::cend(expected)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, ConstCorrectness1d)
{
    {
        using T = span<char>;
        static_assert(std::is_same<char&, decltype(*std::declval<T>().begin())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().cbegin())>::value, "Failed.");
        static_assert(std::is_same<char&, decltype(*std::declval<T>().end())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().cend())>::value, "Failed.");
        static_assert(std::is_same<char&, decltype(*std::declval<T>().rbegin())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().crbegin())>::value, "Failed.");
        static_assert(std::is_same<char&, decltype(*std::declval<T>().rend())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().crend())>::value, "Failed.");
        static_assert(std::is_same<char*, decltype(std::declval<T>().data())>::value, "Failed.");
    }
    {
        using T = span<const char>;
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().begin())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().cbegin())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().end())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().cend())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().rbegin())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().crbegin())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().rend())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().crend())>::value, "Failed.");
        static_assert(std::is_same<const char*, decltype(std::declval<T>().data())>::value, "Failed.");
    }
    {
        using T = const span<char>;
        static_assert(std::is_same<char&, decltype(*std::declval<T>().begin())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().cbegin())>::value, "Failed.");
        static_assert(std::is_same<char&, decltype(*std::declval<T>().end())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().cend())>::value, "Failed.");
        static_assert(std::is_same<char&, decltype(*std::declval<T>().rbegin())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().crbegin())>::value, "Failed.");
        static_assert(std::is_same<char&, decltype(*std::declval<T>().rend())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().crend())>::value, "Failed.");
        static_assert(std::is_same<char*, decltype(std::declval<T>().data())>::value, "Failed.");
    }
    {
        using T = const span<const char>;
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().begin())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().cbegin())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().end())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().cend())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().rbegin())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().crbegin())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().rend())>::value, "Failed.");
        static_assert(std::is_same<const char&, decltype(*std::declval<T>().crend())>::value, "Failed.");
        static_assert(std::is_same<const char*, decltype(std::declval<T>().data())>::value, "Failed.");
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, Front)
{
    const std::vector<std::int32_t> data{42, 0, -1, -6};
    const span<const std::int32_t> unit{data};
    EXPECT_EQ(unit.front(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, WhenFrontAndEmptyThenViolated)
{
    const span<const std::int32_t> unit{};
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(unit.front());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, Back)
{
    const std::vector<std::int32_t> data{42, 0, -1, -6};
    const span<const std::int32_t> unit{data};
    EXPECT_EQ(unit.back(), -6);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, WhenBackAndEmptyThenViolated)
{
    const span<const std::int32_t> unit{};
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(unit.back());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, subspan)
{
    const std::vector<std::int32_t> data{12, 5, -5, -6};
    const span<const std::int32_t> view{data.data(), data.size()};
    {
        std::size_t offset{1};
        std::size_t count{2};
        const span<const std::int32_t> unit{view.subspan(offset, count)};
        EXPECT_TRUE(std::equal(unit.begin(),
                               unit.end(),
                               data.begin() + static_cast<std::ptrdiff_t>(offset),
                               data.begin() + static_cast<std::ptrdiff_t>(offset + count)));
        EXPECT_TRUE(unit.data() == view.data() + offset);
        EXPECT_TRUE(unit.size() == count);
    }
    {
        std::size_t offset{3};
        const span<const std::int32_t> unit{view.subspan(offset)};
        EXPECT_TRUE(
            std::equal(unit.begin(), unit.end(), data.begin() + static_cast<std::ptrdiff_t>(offset), data.end()));
        EXPECT_TRUE(unit.data() == view.data() + offset);
        EXPECT_TRUE(unit.size() == view.size() - offset);
    }
    {
        std::size_t offset{0};
        std::size_t count{view.size()};
        const span<const std::int32_t> unit{view.subspan(offset, count)};
        EXPECT_TRUE(std::equal(unit.begin(), unit.end(), data.begin(), data.end()));
        EXPECT_TRUE(unit.data() == view.data() + offset);
        EXPECT_TRUE(unit.size() == count);
    }
    {
        std::size_t offset{2};
        std::size_t count{0};
        const span<const std::int32_t> unit{view.subspan(offset, count)};
        EXPECT_TRUE(unit.empty());
        EXPECT_TRUE(unit.data() == view.data() + offset);
        EXPECT_TRUE(unit.size() == count);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, WhenSubspanAndOffsetIsGreaterThanSizeThenViolated)
{
    const std::vector<std::int32_t> data{12, 5, -5, -6};
    const span<const std::int32_t> view{data.data(), data.size()};

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(view.subspan(5U));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, WhenSubspanAndOffsetPlusCountIsGreaterThanSizeThenViolated)
{
    const std::vector<std::int32_t> data{12, 5, -5, -6};
    const span<const std::int32_t> view{data.data(), data.size()};

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(view.subspan(0U, 5U));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(view.subspan(1U, 4U));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(view.subspan(4U, 1U));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, first)
{
    const std::vector<std::int32_t> data{12, 5, -5, -6};
    const span<const std::int32_t> view{data.data(), data.size()};
    {
        std::size_t count{2};
        const span<const std::int32_t> unit{view.first(count)};
        EXPECT_TRUE(
            std::equal(unit.begin(), unit.end(), data.begin(), data.begin() + static_cast<std::ptrdiff_t>(count)));
        EXPECT_TRUE(unit.data() == view.data());
        EXPECT_TRUE(unit.size() == count);
    }
    {
        std::size_t count{view.size()};
        const span<const std::int32_t> unit{view.first(count)};
        EXPECT_TRUE(std::equal(unit.begin(), unit.end(), data.begin(), data.end()));
        EXPECT_TRUE(unit.data() == view.data());
        EXPECT_TRUE(unit.size() == count);
    }
    {
        std::size_t count{0};
        const span<const std::int32_t> unit{view.first(count)};
        EXPECT_TRUE(unit.empty());
        EXPECT_TRUE(unit.data() == view.data());
        EXPECT_TRUE(unit.size() == count);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, WhenNonConstRangeIsPassedToNonConstSpan)
{
    std::vector<std::int32_t> data{12, 5, -5, -6};
    const span<std::int32_t> view{data};

    EXPECT_EQ(view.size(), data.size());
    EXPECT_EQ(view.data(), data.data());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, WhenConstRangeIsPassedToConstSpan)
{
    const std::vector<std::int32_t> data{12, 5, -5, -6};
    const span<const std::int32_t> view{data};

    EXPECT_EQ(view.size(), data.size());
    EXPECT_EQ(view.data(), data.data());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, WhenNonConstRangeIsPassedToConstSpan)
{
    std::vector<std::int32_t> data{12, 5, -5, -6};
    const span<const std::int32_t> view{data};

    EXPECT_EQ(view.size(), data.size());
    EXPECT_EQ(view.data(), data.data());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, WhenConstRangeIsPassedToNonConstSpan)
{
    static_assert(!std::is_convertible<const std::vector<std::int32_t>&, const span<std::int32_t>>::value, "Failed.");
}

int test_range_overload_set(const span<const float>) { return 0; }
int test_range_overload_set(const span<const double>) { return 1; }

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, WhenOverloadSetTakesSpans)
{
    const std::array<float, 4> float_data{12.0F, 5.0F, -5.0F, -6.0F};
    const std::array<double, 4> double_data{12.0, 5.0, -5.0, -6.0};

    EXPECT_EQ(0, test_range_overload_set(float_data));
    EXPECT_EQ(1, test_range_overload_set(double_data));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, WhenFirstAndCountIsGreaterThanSizeThenViolated)
{
    const std::vector<std::int32_t> data{12, 5, -5, -6};
    const span<const std::int32_t> view{data.data(), data.size()};

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(view.first(5));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, last)
{
    const std::vector<std::int32_t> data{12, 5, -5, -6};
    const span<const std::int32_t> view{data.data(), data.size()};
    {
        std::size_t count{2};
        const span<const std::int32_t> unit{view.last(count)};
        EXPECT_TRUE(std::equal(unit.begin(), unit.end(), data.end() - 2, data.end()));
        EXPECT_TRUE(unit.data() == view.data() + (view.size() - count));
        EXPECT_TRUE(unit.size() == count);
    }
    {
        std::size_t count{view.size()};
        const span<const std::int32_t> unit{view.last(count)};
        EXPECT_TRUE(std::equal(unit.begin(), unit.end(), data.begin(), data.end()));
        EXPECT_TRUE(unit.data() == view.data() + (view.size() - count));
        EXPECT_TRUE(unit.size() == count);
    }
    {
        std::size_t count{0};
        const span<const std::int32_t> unit{view.last(count)};
        EXPECT_TRUE(unit.empty());
        EXPECT_TRUE(unit.data() == view.data() + (view.size() - count));
        EXPECT_TRUE(unit.size() == count);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, WhenLastAndCountIsGreaterThanSizeThenViolated)
{
    const std::vector<std::int32_t> data{12, 5, -5, -6};
    const span<const std::int32_t> view{data.data(), data.size()};

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(view.last(5));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, WhenComparingIterators)
{
    const std::vector<std::int32_t> data{12, 5, -5, -6};
    const span<const std::int32_t> view{data.data(), data.size()};

    EXPECT_EQ(view.begin(), view.begin());
    EXPECT_EQ(view.end(), view.end());
    EXPECT_NE(view.begin(), view.end());
    EXPECT_NE(view.end(), view.begin());

    EXPECT_LT(view.begin(), view.end());
    EXPECT_LE(view.begin(), view.begin());
    EXPECT_GT(view.end(), view.begin());
    EXPECT_GE(view.begin(), view.begin());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, default_constructed_iterator_is_assignable_and_destructible)
{
    std::int32_t data[]{23, 42, 72};
    span<std::int32_t> view{data};

    span<std::int32_t>::const_iterator const_it{};
    span<std::int32_t>::iterator it{};

    const_it = view.cbegin();
    it = view.begin();
    EXPECT_EQ(const_it, view.cbegin());
    EXPECT_EQ(it, view.begin());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, iterator_conversion_from_non_const_to_const_iterator)
{
    std::int32_t data[]{23, 42, 72};
    span<std::int32_t> view{data};
    span<std::int32_t>::const_iterator it{view.begin()};

    static_assert(std::is_same<const std::int32_t&, decltype(*it)>::value, "failure");
}

template <typename T>
struct span_associated_type_test : testing::Test
{
};

using span_element_types = testing::Types<double, const double>;
TYPED_TEST_SUITE(span_associated_type_test, span_element_types, /*unused*/);

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TYPED_TEST(span_associated_type_test, that_type_traits_api_is_compatible_with_std20_span)
{
    using ElementType = TypeParam;
    using GivenSpan = span<ElementType>;
    using ExpectedValueType = std::remove_cv_t<ElementType>;

    static_assert(std::is_same<typename GivenSpan::size_type, std::size_t>::value, "");
    static_assert(std::is_same<typename GivenSpan::difference_type, std::ptrdiff_t>::value, "");
    static_assert(std::is_same<typename GivenSpan::value_type, ExpectedValueType>::value, "");
    static_assert(std::is_same<typename GivenSpan::element_type, ElementType>::value, "");
    static_assert(std::is_same<typename GivenSpan::pointer, ElementType*>::value, "");
    static_assert(std::is_same<typename GivenSpan::const_pointer, const ElementType*>::value, "");
    static_assert(std::is_same<typename GivenSpan::reference, ElementType&>::value, "");
    static_assert(std::is_same<typename GivenSpan::const_reference, const ElementType&>::value, "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, noexcept_non_std20_api)
{
    using GivenSomeElementType = const double;
    using GivenSpan = span<GivenSomeElementType>;

    static_assert(noexcept(GivenSpan().cbegin()), "");
    static_assert(noexcept(GivenSpan().cend()), "");
    static_assert(noexcept(GivenSpan().crbegin()), "");
    static_assert(noexcept(GivenSpan().crend()), "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338069
TEST(span, that_noexcept_api_is_compatible_with_std20_span)
{
    using GivenSomeElementType = const double;
    using GivenSpan = span<GivenSomeElementType>;

    constexpr GivenSomeElementType x[1] = {{}};

    static_assert(noexcept(GivenSpan()), "");
    static_assert(noexcept(GivenSpan(x)), "");
    static_assert(noexcept(GivenSpan().size()), "");
    static_assert(noexcept(GivenSpan().empty()), "");
    static_assert(noexcept(GivenSpan().data()), "");

    static_assert(noexcept(GivenSpan().begin()), "");
    static_assert(noexcept(GivenSpan().end()), "");
    static_assert(noexcept(GivenSpan().rbegin()), "");
    static_assert(noexcept(GivenSpan().rend()), "");

    static_assert(noexcept(as_bytes(GivenSpan())), "");
}

} // namespace
} // namespace score::cpp
