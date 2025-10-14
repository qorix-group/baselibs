///
/// \file
/// \copyright Copyright (C) 2025 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_MAKE_OFFSET_INDEX_SEQUENCE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_MAKE_OFFSET_INDEX_SEQUENCE_HPP

#include <utility>

namespace score::cpp
{

namespace detail
{

template <std::size_t Offset, std::size_t... Is>
constexpr auto make_offset_index_sequence(std::index_sequence<Is...>)
{
    return std::index_sequence<(Offset + Is)...>{};
}

} // namespace detail

template <std::size_t O, std::size_t N>
using make_offset_index_sequence = decltype(score::cpp::detail::make_offset_index_sequence<O>(std::make_index_sequence<N>{}));

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_MAKE_OFFSET_INDEX_SEQUENCE_HPP
