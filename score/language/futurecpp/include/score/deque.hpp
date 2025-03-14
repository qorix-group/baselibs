///
/// \file
/// \copyright Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.Deque component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_DEQUE_HPP
#define SCORE_LANGUAGE_FUTURECPP_DEQUE_HPP

#include <score/private/iterator/at.hpp>   // IWYU pragma: export
#include <score/private/iterator/data.hpp> // IWYU pragma: export
#include <score/private/iterator/size.hpp> // IWYU pragma: export

#include <score/memory_resource.hpp>

#include <deque> // IWYU pragma: export

namespace score::cpp
{
namespace pmr
{

template <class T>
using deque = std::deque<T, score::cpp::pmr::polymorphic_allocator<T>>;

} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_DEQUE_HPP
