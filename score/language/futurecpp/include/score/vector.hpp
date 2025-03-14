///
/// \file
/// \copyright Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.Vector component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_VECTOR_HPP
#define SCORE_LANGUAGE_FUTURECPP_VECTOR_HPP

#include <score/private/iterator/at.hpp>   // IWYU pragma: export
#include <score/private/iterator/data.hpp> // IWYU pragma: export
#include <score/private/iterator/size.hpp> // IWYU pragma: export

#include <score/memory_resource.hpp>

#include <vector> // IWYU pragma: export

namespace score::cpp
{
namespace pmr
{

template <class T>
using vector = std::vector<T, score::cpp::pmr::polymorphic_allocator<T>>;

} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_VECTOR_HPP
