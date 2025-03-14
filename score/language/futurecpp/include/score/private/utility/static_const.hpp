///
/// \file
/// \copyright Copyright (C) 2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_STATIC_CONST_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_STATIC_CONST_HPP

namespace score::cpp
{

/// @brief Define global constexpr variables in header files without ODR-violations using score::cpp::static_const.
/// @see http://ericniebler.github.io/std/wg21/D4381.html#no-violations-of-the-one-definition-rule
///
/// Example:
/// namespace score::cpp
/// {
/// struct in_place_t
/// {
///     explicit in_place_t() = default;
/// };
/// namespace
/// {
/// constexpr auto& in_place = static_const<in_place_t>::value;
/// }
/// } // namespace score::cpp
template <typename T>
struct static_const
{
    static constexpr T value{};
};

template <typename T>
constexpr T static_const<T>::value;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_STATIC_CONST_HPP
