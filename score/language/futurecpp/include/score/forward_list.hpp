///
/// \file
/// \copyright Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.ForwardList component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_FORWARD_LIST_HPP
#define SCORE_LANGUAGE_FUTURECPP_FORWARD_LIST_HPP

#include <score/private/iterator/at.hpp>   // IWYU pragma: export
#include <score/private/iterator/data.hpp> // IWYU pragma: export
#include <score/private/iterator/size.hpp> // IWYU pragma: export

#include <score/memory_resource.hpp>

#include <forward_list> // IWYU pragma: export

namespace score::cpp
{
namespace pmr
{

template <class T>
using forward_list = std::forward_list<T, score::cpp::pmr::polymorphic_allocator<T>>;

} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_FORWARD_LIST_HPP
