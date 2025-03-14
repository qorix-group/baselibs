///
/// @file
/// @copyright Copyright (C) 2020, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/string.hpp>

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <type_traits>

namespace score::cpp
{
namespace pmr
{

namespace
{

template <typename T>
string to_string_impl(const T value, memory_resource* const resource)
{
    static_assert(std::is_integral<T>::value, "Must be an integral type");

    // std::numeric_limits<T>::digits10 yields the number of digits that can be *round-tripped* through T (e.g. 2 for
    // 8-bit int), so we need one extra char for the longest number, and another for the sign:
    std::array<string::value_type, std::numeric_limits<T>::digits10 + 2> reverse_result{};

    // Build the representation starting with last digit
    auto current_place = reverse_result.rbegin();
    T remaining_value{value};
    for (; current_place != std::prev(reverse_result.rend()); ++current_place)
    {
        const auto div_result = std::div(remaining_value, static_cast<T>(10));
        *current_place = static_cast<string::value_type>(static_cast<T>('0') + std::abs(div_result.rem));
        remaining_value = div_result.quot;
        if (remaining_value == 0)
        {
            ++current_place;
            break;
        }
    }

    if (value < 0)
    {
        *current_place = '-';
        ++current_place;
    }

    return string{current_place.base(), reverse_result.end(), resource};
}

string to_string_double_impl(const double value, memory_resource* const resource)
{
    score::cpp::pmr::string buffer{resource};
    const int n{std::snprintf(nullptr, 0U, "%lf", value)};
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(n >= 0);

    buffer.resize(static_cast<score::cpp::pmr::string::size_type>(n) + 1U); // +1 for nul-termination
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(std::snprintf(&buffer[0], buffer.size(), "%lf", value) == n);
    buffer.pop_back();
    return buffer;
}

} // namespace

string to_string(const std::int32_t value, memory_resource* const resource)
{
    return score::cpp::pmr::to_string_impl(value, resource);
}

string to_string(const std::int64_t value, memory_resource* const resource)
{
    return score::cpp::pmr::to_string_impl(value, resource);
}

string to_string(const double value, memory_resource* const resource)
{
    return score::cpp::pmr::to_string_double_impl(value, resource);
}

} // namespace pmr
} // namespace score::cpp
