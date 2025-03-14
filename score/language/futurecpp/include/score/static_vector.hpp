///
/// \file
/// \copyright Copyright (C) 2016-2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.StaticVector component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_STATIC_VECTOR_HPP
#define SCORE_LANGUAGE_FUTURECPP_STATIC_VECTOR_HPP

#include <score/inplace_vector.hpp>

namespace score::cpp
{

template <typename T, std::size_t MaxSize>
using static_vector = inplace_vector<T, MaxSize>;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_STATIC_VECTOR_HPP
