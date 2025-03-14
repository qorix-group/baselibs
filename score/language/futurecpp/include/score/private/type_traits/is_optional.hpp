///
/// @file
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_OPTIONAL_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_OPTIONAL_HPP

#include <type_traits>

namespace score::cpp
{

template <typename T>
class optional;

/// \brief Check whether the type is an optional type
/// \tparam T: type to be checked
template <typename T>
struct is_optional : std::false_type
{
};

/// \brief Check whether the type is an optional type
/// \tparam T: type to be checked
template <typename T>
struct is_optional<score::cpp::optional<T>> : std::true_type
{
};

/// \brief Check whether the type is an optional type
/// \tparam T: type to be checked
template <typename T>
constexpr auto is_optional_v = is_optional<T>::value;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_OPTIONAL_HPP
