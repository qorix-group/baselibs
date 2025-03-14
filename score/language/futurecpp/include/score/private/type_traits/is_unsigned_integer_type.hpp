///
/// \file
/// \copyright Copyright (C) 2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_UNSIGNED_INTEGER_TYPE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_UNSIGNED_INTEGER_TYPE_HPP

#include <type_traits>

namespace score::cpp
{

namespace detail
{

template <typename T>
struct is_unsigned_integer_type
{
    static constexpr bool value{std::is_same<T, unsigned char>::value || std::is_same<T, unsigned short int>::value ||
                                std::is_same<T, unsigned int>::value || std::is_same<T, unsigned long int>::value ||
                                std::is_same<T, unsigned long long int>::value};
};

} // namespace detail

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_UNSIGNED_INTEGER_TYPE_HPP
