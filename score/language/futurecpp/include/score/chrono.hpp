///
/// \file
/// \copyright Copyright (C) 2020, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#ifndef SCORE_LANGUAGE_FUTURECPP_CHRONO_HPP
#define SCORE_LANGUAGE_FUTURECPP_CHRONO_HPP

#include <chrono>
#include <limits>

namespace score::cpp
{
namespace chrono
{

/// @brief Returns the absolute value of the input duration.
template <typename Rep, typename Period, typename = std::enable_if_t<std::numeric_limits<Rep>::is_signed>>
constexpr std::chrono::duration<Rep, Period> abs(const std::chrono::duration<Rep, Period> duration)
{
    return duration >= duration.zero() ? duration : -duration;
}

} // namespace chrono
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_CHRONO_HPP
