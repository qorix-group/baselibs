///
/// \file
/// \copyright Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.UnorderedMap component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_UNORDERED_MAP_HPP
#define SCORE_LANGUAGE_FUTURECPP_UNORDERED_MAP_HPP

#include <score/private/iterator/at.hpp>   // IWYU pragma: export
#include <score/private/iterator/data.hpp> // IWYU pragma: export
#include <score/private/iterator/size.hpp> // IWYU pragma: export

#include <score/memory_resource.hpp>

#include <unordered_map> // IWYU pragma: export

namespace score::cpp
{
namespace pmr
{

template <class Key, class T, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
using unordered_map = std::unordered_map<Key, T, Hash, Pred, score::cpp::pmr::polymorphic_allocator<std::pair<const Key, T>>>;

template <class Key, class T, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
using unordered_multimap =
    std::unordered_multimap<Key, T, Hash, Pred, score::cpp::pmr::polymorphic_allocator<std::pair<const Key, T>>>;

} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_UNORDERED_MAP_HPP
