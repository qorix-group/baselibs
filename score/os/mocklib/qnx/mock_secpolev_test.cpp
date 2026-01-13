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

#include "score/os/mocklib/qnx/mock_secpolev.h"

#include <gtest/gtest.h>

namespace score
{
namespace os
{
namespace qnx
{

// Verify mock is not abstract - can be instantiated
TEST(MockSecpolEvTest, CanBeInstantiated)
{
    MockSecpolEv mock;
    SUCCEED();
}

// Test init() method
TEST(MockSecpolEvTest, Init)
{
    MockSecpolEv mock;
    EXPECT_CALL(mock, init(0, 10, 1000)).WillOnce(testing::Return(score::cpp::expected_blank<score::os::Error>{}));

    EXPECT_TRUE(mock.init(0, 10, 1000).has_value());
}

// Test shutdown() method
TEST(MockSecpolEvTest, Shutdown)
{
    MockSecpolEv mock;
    EXPECT_CALL(mock, shutdown(SECPOLEV_SHUTDOWN_GRACEFUL))
        .WillOnce(testing::Return(score::cpp::expected_blank<score::os::Error>{}));

    EXPECT_TRUE(mock.shutdown(SECPOLEV_SHUTDOWN_GRACEFUL).has_value());
}

// Test wait_event() method
TEST(MockSecpolEvTest, WaitEvent)
{
    MockSecpolEv mock;
    unsigned flags = 0;
    EXPECT_CALL(mock, wait_event(&flags))
        .WillOnce(testing::Return(score::cpp::expected<secpolev_event_type_t, score::os::Error>{SECPOLEV_EVENT_ABILITY}));

    auto result = mock.wait_event(&flags);
    EXPECT_EQ(result.value(), SECPOLEV_EVENT_ABILITY);
}

// Test flush() method
TEST(MockSecpolEvTest, Flush)
{
    MockSecpolEv mock;
    EXPECT_CALL(mock, flush()).WillOnce(testing::Return(score::cpp::expected_blank<score::os::Error>{}));

    EXPECT_TRUE(mock.flush().has_value());
}

// Test get_dropped_event_count() method
TEST(MockSecpolEvTest, GetDroppedEventCount)
{
    MockSecpolEv mock;
    EXPECT_CALL(mock, get_dropped_event_count()).WillOnce(testing::Return(10));

    EXPECT_EQ(mock.get_dropped_event_count(), 10);
}

// Test get_pid() method
TEST(MockSecpolEvTest, GetPid)
{
    MockSecpolEv mock;
    EXPECT_CALL(mock, get_pid()).WillOnce(testing::Return(score::cpp::expected<pid_t, score::os::Error>{1234}));

    auto result = mock.get_pid();
    EXPECT_EQ(result.value(), 1234);
}

// Test get_process_name() method
TEST(MockSecpolEvTest, GetProcessName)
{
    MockSecpolEv mock;
    EXPECT_CALL(mock, get_process_name()).WillOnce(testing::Return(score::cpp::expected<const char*, score::os::Error>{"test"}));

    auto result = mock.get_process_name();
    EXPECT_STREQ(result.value(), "test");
}

// Test get_process_type() method
TEST(MockSecpolEvTest, GetProcessType)
{
    MockSecpolEv mock;
    EXPECT_CALL(mock, get_process_type())
        .WillOnce(testing::Return(score::cpp::expected<const char*, score::os::Error>{"system"}));

    auto result = mock.get_process_type();
    EXPECT_STREQ(result.value(), "system");
}

// Test get_ability_info() method
TEST(MockSecpolEvTest, GetAbilityInfo)
{
    MockSecpolEv mock;
    unsigned id, flags;
    const char* name;
    uint64_t start, end;

    EXPECT_CALL(mock, get_ability_info(&id, &name, &flags, &start, &end))
        .WillOnce(testing::Return(score::cpp::expected_blank<score::os::Error>{}));

    EXPECT_TRUE(mock.get_ability_info(&id, &name, &flags, &start, &end).has_value());
}

// Test get_path_info() method
TEST(MockSecpolEvTest, GetPathInfo)
{
    MockSecpolEv mock;
    const char* path;
    secpolev_path_event_status_t status;

    EXPECT_CALL(mock, get_path_info(&path, &status)).WillOnce(testing::Return(score::cpp::expected_blank<score::os::Error>{}));

    EXPECT_TRUE(mock.get_path_info(&path, &status).has_value());
}

// Test get_custom_perm_info() method
TEST(MockSecpolEvTest, GetCustomPermInfo)
{
    MockSecpolEv mock;
    const char *class_name, *perm_name, *obj_type;

    EXPECT_CALL(mock, get_custom_perm_info(&class_name, &perm_name, &obj_type))
        .WillOnce(testing::Return(score::cpp::expected_blank<score::os::Error>{}));

    EXPECT_TRUE(mock.get_custom_perm_info(&class_name, &perm_name, &obj_type).has_value());
}

}  // namespace qnx
}  // namespace os
}  // namespace score
