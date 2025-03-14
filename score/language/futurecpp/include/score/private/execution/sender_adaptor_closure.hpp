///
/// @file
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_SENDER_ADAPTOR_CLOSURE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_SENDER_ADAPTOR_CLOSURE_HPP

#include <score/private/execution/sender_t.hpp>
#include <score/private/type_traits/is_derived_from.hpp>

#include <type_traits>
#include <utility>

namespace score::cpp
{
namespace execution
{

template <typename Sender>
struct sender_adaptor_closure
{
};

template <typename Sender>
using is_sender_adaptor_closure =
    score::cpp::detail::is_derived_from<std::decay_t<Sender>, sender_adaptor_closure<std::decay_t<Sender>>>;

template <typename Sender,
          typename Closure,
          typename = std::enable_if_t<is_sender<Sender>::value && is_sender_adaptor_closure<Closure>::value>>
auto operator|(Sender&& sender, Closure&& closure)
{
    return std::forward<Closure>(closure)(std::forward<Sender>(sender));
}

} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_SENDER_ADAPTOR_CLOSURE_HPP
