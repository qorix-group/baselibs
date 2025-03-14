///
/// @file
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_RECEIVER_T_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_RECEIVER_T_HPP

#include <score/private/type_traits/remove_cvref.hpp>

#include <type_traits>

namespace score::cpp
{
namespace execution
{

struct receiver_t
{
};

template <typename Receiver>
using is_receiver = std::is_same<receiver_t, typename score::cpp::remove_cvref_t<Receiver>::receiver_concept>;

} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_RECEIVER_T_HPP
