///
/// @file
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_SENDER_T_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_SENDER_T_HPP

#include <score/private/type_traits/remove_cvref.hpp>
#include <type_traits>

namespace score::cpp
{
namespace execution
{

struct sender_t
{
};

template <typename Sender>
using is_sender = std::is_same<sender_t, typename score::cpp::remove_cvref_t<Sender>::sender_concept>;

} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_SENDER_T_HPP
