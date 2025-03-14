///
/// \file
/// \copyright Copyright (C) 2016-2018, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.Blank component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_BLANK_HPP
#define SCORE_LANGUAGE_FUTURECPP_BLANK_HPP

namespace score::cpp
{

///
/// \brief This type represents a 'blank' object which can be used in Score.Futurecpp.Optional or Score.Futurecpp.Variant
///
/// Implements `std::monostate`
/// https://en.cppreference.com/w/cpp/utility/variant/monostate
///
struct blank
{
};

/// \brief Specialization of comparison operators for \ref blank. All instances of \ref blank are equal by definition.
constexpr bool operator<(blank, blank) noexcept { return false; }
/// \brief Specialization of comparison operators for \ref blank. All instances of \ref blank are equal by definition.
constexpr bool operator>(blank, blank) noexcept { return false; }
/// \brief Specialization of comparison operators for \ref blank. All instances of \ref blank are equal by definition.
constexpr bool operator<=(blank, blank) noexcept { return true; }
/// \brief Specialization of comparison operators for \ref blank. All instances of \ref blank are equal by definition.
constexpr bool operator>=(blank, blank) noexcept { return true; }
/// \brief Specialization of comparison operators for \ref blank. All instances of \ref blank are equal by definition.
constexpr bool operator==(blank, blank) noexcept { return true; }
/// \brief Specialization of comparison operators for \ref blank. All instances of \ref blank are equal by definition.
constexpr bool operator!=(blank, blank) noexcept { return false; }

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_BLANK_HPP
