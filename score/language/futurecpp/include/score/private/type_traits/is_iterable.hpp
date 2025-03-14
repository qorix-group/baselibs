///
/// \file
/// \copyright Copyright (C) 2017-2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_ITERABLE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_ITERABLE_HPP

#include <iterator>
#include <type_traits>
#include <utility>

#include <score/private/type_traits/void_t.hpp>

namespace score::cpp
{

/// \brief Check whether the type is iterable
/// the negative case
/// \tparam T: type to be checked
///
template <typename T, typename = void>
struct is_iterable : std::false_type
{
};

/// \brief Check whether the type is iterable
/// the positive case
/// \tparam T: type to be checked
///
template <typename T>
struct is_iterable<T, score::cpp::void_t<decltype(std::begin(std::declval<T>())), decltype(std::end(std::declval<T>()))>>
    : std::true_type
{
};

/// \brief Check whether the type is a iterable
/// \tparam T: type to be checked
///
template <typename T>
constexpr auto is_iterable_v = is_iterable<T>::value;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_ITERABLE_HPP
