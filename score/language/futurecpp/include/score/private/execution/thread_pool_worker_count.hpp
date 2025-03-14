///
/// @file
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THREAD_POOL_WORKER_COUNT_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THREAD_POOL_WORKER_COUNT_HPP

#include <cstdint>

namespace score::cpp
{
namespace execution
{
namespace detail
{

class thread_pool_worker_count
{
public:
    constexpr explicit thread_pool_worker_count(const std::int32_t count) noexcept : count_{count} {}
    constexpr std::int32_t value() const noexcept { return count_; }

private:
    std::int32_t count_;
};

} // namespace detail
} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THREAD_POOL_WORKER_COUNT_HPP
