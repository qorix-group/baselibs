///
/// \file
/// \copyright Copyright (C) 2017-2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_TYPE_IDENTITY_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_TYPE_IDENTITY_HPP

#include <type_traits>

namespace score::cpp
{

///
/// \brief type_identity can be used to block template argument deduction and is the C++14 version of C++20
/// \see https://en.cppreference.com/w/cpp/types/type_identity
///
/// \tparam T: type which is exposed
///
template <typename T>
struct type_identity
{
    using type = T;
};

template <typename T>
using type_identity_t = typename score::cpp::type_identity<T>::type;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_TYPE_IDENTITY_HPP
