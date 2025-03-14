///
/// @file
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_DERIVED_FROM_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_DERIVED_FROM_HPP

#include <score/private/type_traits/conjunction.hpp>
#include <type_traits>

namespace score::cpp
{
namespace detail
{

template <typename Derived, typename Base>
using is_derived_from = score::cpp::conjunction<std::is_base_of<Base, Derived>,
                                         std::is_convertible<const volatile Derived*, const volatile Base*>>;

}
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_DERIVED_FROM_HPP
