///
/// @file
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_CPU_SCHEDULER_SENDER_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_CPU_SCHEDULER_SENDER_HPP

#include <score/private/execution/cpu_scheduler_op_state.hpp>
#include <score/private/execution/receiver_t.hpp>
#include <score/private/execution/sender_t.hpp>
#include <score/private/execution/thread_pool.hpp>

#include <utility>

namespace score::cpp
{
namespace execution
{
namespace detail
{

class cpu_scheduler_sender
{
public:
    using sender_concept = sender_t;

    explicit cpu_scheduler_sender(thread_pool& pool) : pool_{&pool} {}

    template <typename Receiver>
    auto connect(Receiver&& receiver) const
    {
        static_assert(is_receiver<Receiver>::value, "not a receiver");
        return score::cpp::execution::detail::make_cpu_scheduler_op_state(std::forward<Receiver>(receiver), *pool_);
    }

private:
    thread_pool* pool_;
};

} // namespace detail
} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_CPU_SCHEDULER_SENDER_HPP
