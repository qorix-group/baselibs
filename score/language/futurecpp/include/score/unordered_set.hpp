///
/// \file
/// \copyright Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.UnorderedSet component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_UNORDERED_SET_HPP
#define SCORE_LANGUAGE_FUTURECPP_UNORDERED_SET_HPP

#include <score/private/iterator/at.hpp>   // IWYU pragma: export
#include <score/private/iterator/data.hpp> // IWYU pragma: export
#include <score/private/iterator/size.hpp> // IWYU pragma: export

#include <score/memory_resource.hpp>

#include <unordered_set> // IWYU pragma: export

namespace score::cpp
{
namespace pmr
{

template <class Key, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
using unordered_set = std::unordered_set<Key, Hash, Pred, score::cpp::pmr::polymorphic_allocator<Key>>;

template <class Key, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
using unordered_multiset = std::unordered_multiset<Key, Hash, Pred, score::cpp::pmr::polymorphic_allocator<Key>>;

} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_UNORDERED_SET_HPP
