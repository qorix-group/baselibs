/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
///
/// @file
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include "score/os/mocklib/qnx/mock_timer.h"
#include "score/os/qnx/timer_impl.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace
{

constexpr std::int32_t kInvalidId{-1};

struct TimerTest : ::testing::Test
{
  protected:
    TimerTest() : event_({0}), expiration_time_({0}) {}

    void SetUp() override
    {
        timer_ = std::make_unique<score::os::qnx::TimerImpl>();
    };

    const struct sigevent event_;
    const struct _itimer expiration_time_;
    std::unique_ptr<score::os::qnx::Timer> timer_;
};

TEST_F(TimerTest, timer_create_succeed_for_realtime_clock_nullptr_event)
{
    const auto result = timer_->TimerCreate(CLOCK_REALTIME, nullptr);
    EXPECT_TRUE(result.has_value());
}

TEST_F(TimerTest, timer_create_succeed_for_realtime_clock_real_struct_event)
{
    const auto result = timer_->TimerCreate(CLOCK_REALTIME, &event_);
    EXPECT_TRUE(result.has_value());
}

TEST_F(TimerTest, timer_create_fail_for_invalid_clock)
{
    const auto result = timer_->TimerCreate(kInvalidId, &event_);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kInvalidArgument);
}

TEST_F(TimerTest, timer_settime_succeed_for_realtime_clock_nullptr_event)
{
    const auto timer_id = timer_->TimerCreate(CLOCK_REALTIME, nullptr);
    const auto result = timer_->TimerSettime(timer_id.value(), TIMER_ABSTIME, &expiration_time_, nullptr);

    EXPECT_TRUE(timer_id.has_value());
    EXPECT_TRUE(result.has_value());
}

TEST_F(TimerTest, timer_settime_fail_for_invalid_clock_id)
{
    const auto result = timer_->TimerSettime(kInvalidId, TIMER_ABSTIME, &expiration_time_, nullptr);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kInvalidArgument);
}

TEST_F(TimerTest, timer_destroy_succeed_for_created_monotonic_clock_timer)
{
    const auto timer_id = timer_->TimerCreate(CLOCK_MONOTONIC, nullptr);
    const auto result = timer_->TimerDestroy(timer_id.value());

    EXPECT_TRUE(timer_id.has_value());
    EXPECT_TRUE(result.has_value());
}

TEST_F(TimerTest, timer_destroy_fail_for_invalid_clock_id)
{
    const auto result = timer_->TimerDestroy(kInvalidId);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kInvalidArgument);
}

TEST_F(TimerTest, timer_settime_fail_after_timer_destroy)
{
    const auto timer_id = timer_->TimerCreate(CLOCK_REALTIME, nullptr);
    timer_->TimerDestroy(timer_id.value());
    const auto result = timer_->TimerSettime(timer_id.value(), TIMER_ABSTIME, &expiration_time_, nullptr);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kInvalidArgument);
}

}  // namespace
