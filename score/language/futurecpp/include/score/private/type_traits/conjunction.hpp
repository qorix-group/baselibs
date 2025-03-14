///
/// \file
/// \copyright Copyright (C) 2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_CONJUNCTION_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_CONJUNCTION_HPP

#include <type_traits>

namespace score::cpp
{

/// @brief Forms the "logical AND" conjunction on the sequence of traits Ts....
///
/// \{
template <typename...>
struct conjunction : std::true_type
{
};
template <typename T>
struct conjunction<T> : T
{
};
template <typename T, typename... Ts>
struct conjunction<T, Ts...> : std::conditional_t<static_cast<bool>(T::value), conjunction<Ts...>, T>
{
};
template <typename... Ts>
constexpr bool conjunction_v = conjunction<Ts...>::value;
/// \}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_CONJUNCTION_HPP
