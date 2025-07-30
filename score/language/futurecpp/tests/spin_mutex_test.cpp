///
/// \file
/// \copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/private/execution/spin_mutex.hpp>
#include <score/private/execution/spin_mutex.hpp> // test include guard

#include <score/jthread.hpp>
#include <score/latch.hpp>

#include <gtest/gtest.h>

namespace
{

// NOTRACING
TEST(spin_mutex_test, lock_GivenNotOwnedMutex_ExpectMutexOwnedAndAnotherThreadCannotAcquireTheMutex)
{
    score::cpp::execution::detail::spin_mutex m{};

    m.lock();

    score::cpp::jthread t{[&m]() { EXPECT_FALSE(m.try_lock()); }};
}

// NOTRACING
TEST(spin_mutex_test, try_lock_GivenNotOwnedMutex_ExpectMutexOwnedAndAnotherThreadCannotAcquireTheMutex)
{
    score::cpp::execution::detail::spin_mutex m{};

    EXPECT_TRUE(m.try_lock());

    score::cpp::jthread t{[&m]() { EXPECT_FALSE(m.try_lock()); }};
}

// NOTRACING
TEST(spin_mutex_test, lock_unlock_GivenParallelCountingThreads_ExpectCountIsIncrementOnlyByOneThreadAtATime)
{
    score::cpp::latch l{3};
    score::cpp::execution::detail::spin_mutex m{};

    std::int32_t count{};
    auto atomic_count = [&l, &m, &count]() {
        l.arrive_and_wait();
        for (std::int32_t i{}; i < 10000; ++i)
        {
            m.lock();
            ++count;
            m.unlock();
        }
    };

    {
        score::cpp::jthread t1{atomic_count};
        score::cpp::jthread t2{atomic_count};
        score::cpp::jthread t3{atomic_count};
    }

    EXPECT_EQ(count, 30000);
}

} // namespace
