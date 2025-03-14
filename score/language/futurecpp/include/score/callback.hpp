///
/// \file
/// \copyright Copyright (C) 2016-2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.Callback component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_CALLBACK_HPP
#define SCORE_LANGUAGE_FUTURECPP_CALLBACK_HPP

#include <score/private/functional/move_only_function.hpp>

namespace score::cpp
{

template <class Signature,
          std::size_t Capacity = score::cpp::detail::default_capacity(),
          std::size_t Alignment = alignof(std::max_align_t)>
using callback = score::cpp::move_only_function<Signature, Capacity, Alignment>;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_CALLBACK_HPP
