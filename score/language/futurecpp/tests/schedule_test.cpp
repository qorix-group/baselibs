///
/// @file
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/execution.hpp>
#include <score/execution.hpp> //  // test include guard

#include <cstdint>
#include <utility>

#include <gtest/gtest.h>

namespace score::cpp
{
namespace execution
{
namespace
{

struct scheduler_sender
{
    using sender_concept = sender_t;

    std::int32_t v;
};

struct scheduler
{
    using scheduler_concept = scheduler_t;

    scheduler_sender schedule() & { return {v}; }
    scheduler_sender schedule() && = delete;
    scheduler_sender schedule() const& = delete;
    scheduler_sender schedule() const&& = delete;

    std::int32_t v;
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(schedule_test, schedule_GivenRValueSender_ExpectSchedulerSender)
{
    scheduler sch{42};
    const scheduler_sender s{schedule(std::move(sch))};

    EXPECT_EQ(42, s.v);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(schedule_test, schedule_GivenLValueSender_ExpectSchedulerSender)
{
    scheduler sch{42};
    const scheduler_sender s{schedule(sch)};

    EXPECT_EQ(42, s.v);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(schedule_test, schedule_GivenConstRValueSender_ExpectSchedulerSender)
{
    const scheduler sch{42};
    const scheduler_sender s{schedule(std::move(sch))};

    EXPECT_EQ(42, s.v);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(schedule_test, schedule_GivenConstLValueSender_ExpectSchedulerSender)
{
    const scheduler sch{42};
    const scheduler_sender s{schedule(sch)};

    EXPECT_EQ(42, s.v);
}

} // namespace
} // namespace execution
} // namespace score::cpp
