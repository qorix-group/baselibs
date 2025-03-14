///
/// \file
/// \copyright Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.Set component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_SET_HPP
#define SCORE_LANGUAGE_FUTURECPP_SET_HPP

#include <score/private/iterator/at.hpp>   // IWYU pragma: export
#include <score/private/iterator/data.hpp> // IWYU pragma: export
#include <score/private/iterator/size.hpp> // IWYU pragma: export

#include <score/memory_resource.hpp>

#include <set> // IWYU pragma: export

namespace score::cpp
{
namespace pmr
{

template <class Key, class Compare = std::less<Key>>
using set = std::set<Key, Compare, score::cpp::pmr::polymorphic_allocator<Key>>;

template <class Key, class Compare = std::less<Key>>
using multiset = std::multiset<Key, Compare, score::cpp::pmr::polymorphic_allocator<Key>>;

} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_SET_HPP
