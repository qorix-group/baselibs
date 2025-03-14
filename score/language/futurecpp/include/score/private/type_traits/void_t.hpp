///
/// \file
/// \copyright Copyright (C) 2017-2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_VOID_T_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_VOID_T_HPP

namespace score::cpp
{

///
/// \brief void_t Utility metafunction that maps a sequence of any types to the type void. It is the C++14 version of
/// C++17
/// \see https://en.cppreference.com/w/cpp/types/void_t
///
///
template <class...>
using void_t = void;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_VOID_T_HPP
