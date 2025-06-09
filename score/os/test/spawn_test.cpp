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
#include "score/os/spawn_impl.h"

#include "gtest/gtest.h"

#include <fcntl.h>
#include <cstdint>

namespace score
{
namespace os
{
namespace test
{
constexpr std::int32_t kInvalidFD{-1};

class SpawnTest : public ::testing::Test
{
  public:
    posix_spawnattr_t attr;

    void SetUp() override
    {
        ASSERT_TRUE(score::os::Spawn::instance().posix_spawnattr_init(&attr).has_value());
    }

    void TearDown() override
    {
        ASSERT_TRUE(score::os::Spawn::instance().posix_spawnattr_destroy(&attr).has_value());
    }
};

TEST_F(SpawnTest, posix_spawnattr_setflags_success)
{
    const std::int16_t set_flags{POSIX_SPAWN_SETSIGDEF};

    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setflags(&attr, set_flags);
    ASSERT_TRUE(set_result.has_value());
    EXPECT_EQ(set_result.value(), 0);

    std::int16_t get_flags{0};
    ASSERT_EQ(::posix_spawnattr_getflags(&attr, &get_flags), 0);
    EXPECT_EQ(get_flags, POSIX_SPAWN_SETSIGDEF);
}

TEST_F(SpawnTest, posix_spawnattr_getflags_success)
{
    const std::int16_t set_flags{POSIX_SPAWN_SETSIGDEF};
    ASSERT_EQ(::posix_spawnattr_setflags(&attr, set_flags), 0);

    std::int16_t get_flags{0};
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getflags(&attr, &get_flags);
    ASSERT_TRUE(get_result.has_value());
    EXPECT_EQ(get_result.value(), 0);
    EXPECT_EQ(get_flags, POSIX_SPAWN_SETSIGDEF);
}

TEST(SpawnImpl, posix_spawnattr_getflags_failure)
{
    const std::int16_t set_flags{POSIX_SPAWN_SETSIGDEF};
    posix_spawnattr_t attr;

    ASSERT_EQ(::posix_spawnattr_init(&attr), 0);
    ASSERT_EQ(::posix_spawnattr_setflags(&attr, set_flags), 0);
    ASSERT_EQ(::posix_spawnattr_destroy(&attr), 0);

    std::int16_t get_flags{0};
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getflags(&attr, &get_flags);
    ASSERT_FALSE(get_result.has_value());
    EXPECT_NE(get_flags, POSIX_SPAWN_SETSIGDEF);
}

TEST_F(SpawnTest, posix_spawnattr_setflags_failure)
{
    const auto invalid_flag{-1};
    const auto result = score::os::Spawn::instance().posix_spawnattr_setflags(&attr, invalid_flag);
    ASSERT_FALSE(result.has_value());

    std::int16_t get_flags{0};
    ASSERT_EQ(::posix_spawnattr_getflags(&attr, &get_flags), 0);
    EXPECT_NE(get_flags, invalid_flag);
}

TEST_F(SpawnTest, posix_spawnattr_sigsetdefault_success)
{
    sigset_t set_sigset;
    ::sigemptyset(&set_sigset);
    ::sigaddset(&set_sigset, SIGUSR1);

    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setsigdefault(&attr, &set_sigset);
    ASSERT_TRUE(set_result.has_value());
    EXPECT_EQ(set_result.value(), 0);

    sigset_t get_sigset;
    ::sigemptyset(&get_sigset);
    ASSERT_EQ(::posix_spawnattr_getsigdefault(&attr, &get_sigset), 0);
    EXPECT_EQ(::sigismember(&set_sigset, SIGUSR1), ::sigismember(&get_sigset, SIGUSR1));
}

TEST_F(SpawnTest, posix_spawnattr_getsigdefault_success)
{
    sigset_t set_sigset;
    ::sigemptyset(&set_sigset);
    ::sigaddset(&set_sigset, SIGUSR1);
    ASSERT_EQ(::posix_spawnattr_setsigdefault(&attr, &set_sigset), 0);

    sigset_t get_sigset;
    ::sigemptyset(&get_sigset);
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getsigdefault(&attr, &get_sigset);
    ASSERT_TRUE(get_result.has_value());
    EXPECT_EQ(get_result.value(), 0);
    EXPECT_EQ(::sigismember(&set_sigset, SIGUSR1), ::sigismember(&get_sigset, SIGUSR1));
}

TEST(SpawnImpl, posix_spawnattr_setsigdefault_failure)
{
    sigset_t set_sigset;
    ::sigemptyset(&set_sigset);
    ::sigaddset(&set_sigset, SIGUSR1);
    posix_spawnattr_t attr;

    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setsigdefault(&attr, &set_sigset);
    ASSERT_FALSE(set_result.has_value());
}

TEST(SpawnImpl, posix_spawnattr_getsigdefault_failure)
{
    sigset_t set_sigset;
    ::sigemptyset(&set_sigset);
    ::sigaddset(&set_sigset, SIGUSR1);
    posix_spawnattr_t attr;

    ASSERT_EQ(::posix_spawnattr_init(&attr), 0);
    ASSERT_EQ(::posix_spawnattr_setsigdefault(&attr, &set_sigset), 0);
    ASSERT_EQ(::posix_spawnattr_destroy(&attr), 0);

    sigset_t get_sigset;
    ::sigemptyset(&get_sigset);
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getsigdefault(&attr, &get_sigset);
    ASSERT_FALSE(get_result.has_value());
    EXPECT_FALSE(::sigismember(&get_sigset, SIGUSR1));
}

TEST_F(SpawnTest, posix_spawnattr_setsigmask_success)
{
    sigset_t set_sigset;
    ::sigemptyset(&set_sigset);
    ::sigaddset(&set_sigset, SIGUSR1);

    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setsigmask(&attr, &set_sigset);
    ASSERT_TRUE(set_result.has_value());
    EXPECT_EQ(set_result.value(), 0);

    sigset_t get_sigset;
    ::sigemptyset(&get_sigset);
    ASSERT_EQ(::posix_spawnattr_getsigmask(&attr, &get_sigset), 0);
    EXPECT_EQ(::sigismember(&set_sigset, SIGUSR1), ::sigismember(&get_sigset, SIGUSR1));
}

TEST_F(SpawnTest, posix_spawnattr_getsigmask_success)
{
    sigset_t set_sigset;
    ::sigemptyset(&set_sigset);
    ::sigaddset(&set_sigset, SIGUSR1);
    ASSERT_EQ(::posix_spawnattr_setsigmask(&attr, &set_sigset), 0);

    sigset_t get_sigset;
    ::sigemptyset(&get_sigset);
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getsigmask(&attr, &get_sigset);
    ASSERT_TRUE(get_result.has_value());
    EXPECT_EQ(get_result.value(), 0);
    EXPECT_EQ(::sigismember(&set_sigset, SIGUSR1), ::sigismember(&get_sigset, SIGUSR1));
}

TEST(SpawnImpl, posix_spawnattr_setsigmask_failure)
{
    sigset_t set_sigset;
    ::sigemptyset(&set_sigset);
    ::sigaddset(&set_sigset, SIGUSR1);
    posix_spawnattr_t attr;

    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setsigmask(&attr, &set_sigset);
    ASSERT_FALSE(set_result.has_value());
}

TEST(SpawnImpl, posix_spawnattr_getsigmask_failure)
{
    sigset_t set_sigset;
    ::sigemptyset(&set_sigset);
    ::sigaddset(&set_sigset, SIGUSR1);
    posix_spawnattr_t attr;

    ASSERT_EQ(::posix_spawnattr_init(&attr), 0);
    EXPECT_EQ(::posix_spawnattr_setsigmask(&attr, &set_sigset), 0);
    ASSERT_EQ(::posix_spawnattr_destroy(&attr), 0);

    sigset_t get_sigset;
    ::sigemptyset(&get_sigset);
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getsigmask(&attr, &get_sigset);
    ASSERT_FALSE(get_result.has_value());
    EXPECT_FALSE(::sigismember(&get_sigset, SIGUSR1));
}

TEST_F(SpawnTest, posix_spawnattr_setpgroup_success)
{
    pid_t pid{4};
    ASSERT_EQ(::posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETPGROUP), 0);

    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setpgroup(&attr, pid);
    ASSERT_TRUE(set_result.has_value());
    EXPECT_EQ(set_result.value(), 0);

    pid_t get_pid{0};
    ASSERT_EQ(::posix_spawnattr_getpgroup(&attr, &get_pid), 0);
    EXPECT_EQ(get_pid, pid);
}

TEST_F(SpawnTest, posix_spawnattr_getpgroup_success)
{
    pid_t pid{4};
    ASSERT_TRUE(score::os::Spawn::instance().posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETPGROUP).has_value());
    ASSERT_EQ(::posix_spawnattr_setpgroup(&attr, pid), 0);

    pid_t get_pid{0};
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getpgroup(&attr, &get_pid);
    ASSERT_TRUE(get_result.has_value());
    EXPECT_EQ(get_result.value(), 0);
    EXPECT_EQ(get_pid, pid);
}

TEST(SpawnImpl, posix_spawnattr_setpgroup_failure)
{
    pid_t pid{4};
    posix_spawnattr_t attr;

    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setpgroup(&attr, pid);
    ASSERT_FALSE(set_result.has_value());
}

TEST(SpawnImpl, posix_spawnattr_getpgroup_failure)
{
    posix_spawnattr_t attr;
    pid_t get_pid{0};
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getpgroup(&attr, &get_pid);
    ASSERT_FALSE(get_result.has_value());
}

TEST_F(SpawnTest, posix_spawnattr_setschedparam_success)
{
    struct sched_param set_param;
    set_param.sched_priority = 4;
    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setschedparam(&attr, &set_param);
    ASSERT_TRUE(set_result.has_value());
    EXPECT_EQ(set_result.value(), 0);

    struct sched_param get_param;
    ASSERT_EQ(::posix_spawnattr_getschedparam(&attr, &get_param), 0);
    EXPECT_EQ(set_param.sched_priority, get_param.sched_priority);
}

TEST_F(SpawnTest, posix_spawnattr_getschedparam_success)
{
    struct sched_param set_param;
    set_param.sched_priority = 4;
    ASSERT_EQ(::posix_spawnattr_setschedparam(&attr, &set_param), 0);

    struct sched_param get_param;
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getschedparam(&attr, &get_param);
    ASSERT_TRUE(get_result.has_value());
    EXPECT_EQ(get_result.value(), 0);
    EXPECT_EQ(set_param.sched_priority, get_param.sched_priority);
}

TEST(SpawnImpl, posix_spawnattr_setschedparam_failure)
{
    posix_spawnattr_t attr;
    ASSERT_EQ(::posix_spawnattr_init(&attr), 0);
    ASSERT_EQ(::posix_spawnattr_destroy(&attr), 0);

    struct sched_param set_param;
    set_param.sched_priority = 4;
    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setschedparam(&attr, &set_param);
    ASSERT_FALSE(set_result.has_value());
}

TEST(SpawnImpl, posix_spawnattr_getschedparam_failure)
{
    posix_spawnattr_t attr;
    ASSERT_EQ(::posix_spawnattr_init(&attr), 0);
    ASSERT_EQ(::posix_spawnattr_destroy(&attr), 0);

    struct sched_param set_param;
    const auto set_result = score::os::Spawn::instance().posix_spawnattr_getschedparam(&attr, &set_param);
    ASSERT_FALSE(set_result.has_value());
}

TEST_F(SpawnTest, posix_spawnattr_setschedpolicy_success)
{
    const std::int32_t set_policy{1};
    ASSERT_EQ(::posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETSCHEDULER), 0);

    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setschedpolicy(&attr, set_policy);
    ASSERT_TRUE(set_result.has_value());
    EXPECT_EQ(set_result.value(), 0);

    std::int32_t get_policy{};
    ASSERT_EQ(::posix_spawnattr_getschedpolicy(&attr, &get_policy), 0);
    EXPECT_EQ(get_policy, set_policy);
}

TEST_F(SpawnTest, posix_spawnattr_getschedpolicy_success)
{
    const std::int32_t set_policy{1};
    ASSERT_EQ(::posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETSCHEDULER), 0);
    ASSERT_EQ(::posix_spawnattr_setschedpolicy(&attr, set_policy), 0);

    std::int32_t get_policy{};
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getschedpolicy(&attr, &get_policy);
    ASSERT_TRUE(get_result.has_value());
    EXPECT_EQ(get_result.value(), 0);
    EXPECT_EQ(get_policy, set_policy);
}

TEST(SpawnImpl, posix_spawnattr_getschedpolicy_failure)
{
    const std::int32_t set_policy{1};
    posix_spawnattr_t attr;

    ASSERT_EQ(::posix_spawnattr_init(&attr), 0);
    ASSERT_EQ(::posix_spawnattr_setschedpolicy(&attr, set_policy), 0);
    ASSERT_EQ(::posix_spawnattr_destroy(&attr), 0);

    std::int32_t get_policy{};
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getschedpolicy(&attr, &get_policy);
    ASSERT_FALSE(get_result.has_value());
    EXPECT_NE(get_policy, set_policy);
}

TEST_F(SpawnTest, posix_spawnattr_setschedpolicy_failure)
{
    std::int32_t get_policy{};
    ASSERT_EQ(::posix_spawnattr_getschedpolicy(&attr, &get_policy), 0);

    const std::int32_t invalid_policy{-1};
    const auto result = score::os::Spawn::instance().posix_spawnattr_setschedpolicy(&attr, invalid_policy);
    ASSERT_FALSE(result.has_value());
    EXPECT_NE(get_policy, invalid_policy);
}

TEST(SpawnImpl, posix_spawn_file_actions_init_success)
{
    posix_spawn_file_actions_t file_actions;
    const auto result = score::os::Spawn::instance().posix_spawn_file_actions_init(&file_actions);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);

    ASSERT_EQ(::posix_spawn_file_actions_destroy(&file_actions), 0);
}

TEST(SpawnImpl, posix_spawn_file_actions_destroy_success)
{
    posix_spawn_file_actions_t file_actions;
    ASSERT_EQ(::posix_spawn_file_actions_init(&file_actions), 0);

    const auto ret_destroy = score::os::Spawn::instance().posix_spawn_file_actions_destroy(&file_actions);
    ASSERT_TRUE(ret_destroy.has_value());
    EXPECT_EQ(ret_destroy.value(), 0);
}

TEST(SpawnImpl, posix_spawn_file_actions_addclose_failure)
{
    posix_spawn_file_actions_t file_actions;
    ASSERT_EQ(::posix_spawn_file_actions_init(&file_actions), 0);
    const auto result = score::os::Spawn::instance().posix_spawn_file_actions_addclose(&file_actions, kInvalidFD);
    ASSERT_FALSE(result.has_value());
}

TEST(SpawnImpl, posix_spawn_file_actions_addopen_success)
{
    posix_spawn_file_actions_t file_actions;
    ASSERT_EQ(::posix_spawn_file_actions_init(&file_actions), 0);

    const std::int32_t new_fd{3};
    const char* path = "/tmp/test_file.txt";
    const auto result = score::os::Spawn::instance().posix_spawn_file_actions_addopen(
        &file_actions, new_fd, path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);

    ASSERT_EQ(::posix_spawn_file_actions_addclose(&file_actions, new_fd), 0);
    ASSERT_EQ(::posix_spawn_file_actions_destroy(&file_actions), 0);
}

TEST(SpawnImpl, posix_spawn_file_actions_addclose_success)
{
    posix_spawn_file_actions_t file_actions;
    ASSERT_EQ(::posix_spawn_file_actions_init(&file_actions), 0);

    const std::int32_t new_fd{3};
    const char* path = "/tmp/test_file.txt";
    ASSERT_EQ(::posix_spawn_file_actions_addopen(&file_actions, new_fd, path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR), 0);

    ASSERT_TRUE(score::os::Spawn::instance().posix_spawn_file_actions_addclose(&file_actions, new_fd).has_value());
    ASSERT_EQ(::posix_spawn_file_actions_destroy(&file_actions), 0);
}

TEST(SpawnImpl, posix_spawn_file_actions_addopen_failure)
{
    posix_spawn_file_actions_t file_actions;
    const std::int32_t new_fd{3};
    const char* path = "/tmp/test_file.txt";
    const auto result = score::os::Spawn::instance().posix_spawn_file_actions_addopen(
        &file_actions, new_fd, path, O_RDWR, S_IRUSR | S_IWUSR);
    ASSERT_FALSE(result.has_value());
    ASSERT_NE(::posix_spawn_file_actions_addclose(&file_actions, new_fd), 0);
}

TEST(SpawnImpl, posix_spawn_file_actions_adddup2_success)
{
    posix_spawn_file_actions_t file_actions;
    ASSERT_EQ(::posix_spawn_file_actions_init(&file_actions), 0);
    std::int32_t filedes, dup_filedes;
    filedes = ::open("file", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    dup_filedes = 2;
    const auto result =
        score::os::Spawn::instance().posix_spawn_file_actions_adddup2(&file_actions, filedes, dup_filedes);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);
    ::close(filedes);
    ::close(dup_filedes);
    ASSERT_EQ(::posix_spawn_file_actions_destroy(&file_actions), 0);
}

TEST(SpawnImpl, posix_spawn_file_actions_adddup2_failure)
{
    posix_spawn_file_actions_t file_actions;
    const std::int32_t filedes{};
    ASSERT_EQ(::posix_spawn_file_actions_init(&file_actions), 0);
    const auto result = score::os::Spawn::instance().posix_spawn_file_actions_adddup2(&file_actions, filedes, kInvalidFD);
    ASSERT_FALSE(result.has_value());
}

TEST_F(SpawnTest, Spawn_succcess)
{
    pid_t pid{0};
    char path[] = "/bin/ls";
    posix_spawn_file_actions_t file_actions;
    char* argv[] = {path, nullptr};
    char* envp[] = {nullptr};
    ASSERT_EQ(::posix_spawn_file_actions_init(&file_actions), 0);

    auto result = score::os::Spawn::instance().posix_spawn(&pid, path, &file_actions, &attr, argv, envp);
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), 0);
    ASSERT_GT(pid, 0);

    int status{};
    ::waitpid(pid, &status, 0);
    ASSERT_TRUE(WIFEXITED(status));
}

TEST_F(SpawnTest, Spawn_failure)
{
    pid_t pid{-1};
    char path[] = "/bin/ls";
    posix_spawn_file_actions_t file_actions;
    char* argv[] = {path, nullptr};
    char* envp[] = {nullptr};

    auto result = score::os::Spawn::instance().posix_spawn(&pid, path, &file_actions, &attr, argv, envp);
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(pid, -1);
}

TEST_F(SpawnTest, Spawnp_success)
{
    pid_t pid{0};
    char path[] = "/bin/ls";
    posix_spawn_file_actions_t file_actions;
    char* argv[] = {path, nullptr};
    char* envp[] = {nullptr};
    ASSERT_EQ(::posix_spawn_file_actions_init(&file_actions), 0);

    auto result = score::os::Spawn::instance().posix_spawnp(&pid, path, &file_actions, &attr, argv, envp);
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), 0);
    ASSERT_GT(pid, 0);

    int status{};
    ::waitpid(pid, &status, 0);
    ASSERT_TRUE(WIFEXITED(status));
}

TEST_F(SpawnTest, Spawnp_failure)
{
    pid_t pid{-1};
    char path[] = "/bin/ls";
    posix_spawn_file_actions_t file_actions;
    char* argv[] = {path, nullptr};
    char* envp[] = {nullptr};

    auto result = score::os::Spawn::instance().posix_spawnp(&pid, path, &file_actions, &attr, argv, envp);
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(pid, -1);
}

#if defined(__QNX__)
TEST_F(SpawnTest, posix_spawnattr_setxflags_success)
{
    const std::uint32_t set_flags{POSIX_SPAWN_SETSIGMASK};
    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setxflags(&attr, set_flags);
    ASSERT_TRUE(set_result.has_value());
    EXPECT_EQ(set_result.value(), 0);

    std::uint32_t get_flags{0};
    ASSERT_EQ(::posix_spawnattr_getxflags(&attr, &get_flags), 0);
    EXPECT_EQ(get_flags, set_flags);
}

TEST_F(SpawnTest, posix_spawnattr_getxflags_success)
{
    const std::uint32_t set_flags{POSIX_SPAWN_SETSIGMASK};
    ASSERT_EQ(::posix_spawnattr_setxflags(&attr, set_flags), 0);

    std::uint32_t get_flags{0};
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getxflags(&attr, &get_flags);
    ASSERT_TRUE(get_result.has_value());
    EXPECT_EQ(get_result.value(), 0);
    EXPECT_EQ(get_flags, set_flags);
}

TEST(SpawnImpl, posix_spawnattr_setxflags_failure)
{
    const std::uint32_t set_flags{POSIX_SPAWN_SETSIGMASK};
    posix_spawnattr_t attr;

    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setxflags(&attr, set_flags);
    ASSERT_FALSE(set_result.has_value());
}

TEST(SpawnImpl, posix_spawnattr_getxflags_failure)
{
    posix_spawnattr_t attr;
    std::uint32_t get_flags{0};
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getxflags(&attr, &get_flags);
    ASSERT_FALSE(get_result.has_value());
}

TEST_F(SpawnTest, posix_spawnattr_getrunmask_success)
{
    const std::uint32_t set_runmask{1};
    ASSERT_EQ(::posix_spawnattr_setrunmask(&attr, set_runmask), 0);

    std::uint32_t runmask{};
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getrunmask(&attr, &runmask);
    ASSERT_TRUE(get_result.has_value());
    EXPECT_EQ(get_result.value(), 0);
    EXPECT_EQ(runmask, set_runmask);
}

TEST_F(SpawnTest, posix_spawnattr_setrunmask_success)
{
    const std::uint32_t set_runmask{1};
    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setrunmask(&attr, set_runmask);
    ASSERT_TRUE(set_result.has_value());
    EXPECT_EQ(set_result.value(), 0);

    std::uint32_t runmask{};
    ASSERT_EQ(::posix_spawnattr_getrunmask(&attr, &runmask), 0);
    EXPECT_EQ(runmask, set_runmask);
}

TEST(SpawnImpl, posix_spawnattr_setrunmask_failure)
{
    const std::uint32_t set_runmask{1};
    posix_spawnattr_t attr;
    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setrunmask(&attr, set_runmask);
    ASSERT_FALSE(set_result.has_value());
}

TEST(SpawnImpl, posix_spawnattr_getrunmask_failure)
{
    posix_spawnattr_t attr;
    std::uint32_t runmask{};
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getrunmask(&attr, &runmask);
    ASSERT_FALSE(get_result.has_value());
}

TEST_F(SpawnTest, posix_spawnattr_setsigignore_success)
{
    sigset_t set_sigset;
    ::sigemptyset(&set_sigset);
    ::sigaddset(&set_sigset, SIGUSR1);

    const std::uint32_t set_flags{POSIX_SPAWN_SETSIGIGN};
    ASSERT_EQ(::posix_spawnattr_setxflags(&attr, set_flags), 0);

    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setsigignore(&attr, &set_sigset);
    ASSERT_TRUE(set_result.has_value());
    EXPECT_EQ(set_result.value(), 0);

    sigset_t get_sigset;
    ::sigemptyset(&get_sigset);
    ASSERT_EQ(::posix_spawnattr_getsigignore(&attr, &get_sigset), 0);
    EXPECT_EQ(::sigismember(&set_sigset, SIGUSR1), ::sigismember(&get_sigset, SIGUSR1));
}

TEST_F(SpawnTest, posix_spawnattr_getsigignore_success)
{
    sigset_t set_sigset;
    ::sigemptyset(&set_sigset);
    ::sigaddset(&set_sigset, SIGUSR1);

    const std::uint32_t set_flags{POSIX_SPAWN_SETSIGIGN};
    ASSERT_EQ(::posix_spawnattr_setxflags(&attr, set_flags), 0);
    ASSERT_EQ(::posix_spawnattr_setsigignore(&attr, &set_sigset), 0);

    sigset_t get_sigset;
    ::sigemptyset(&get_sigset);
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getsigignore(&attr, &get_sigset);
    ASSERT_TRUE(get_result.has_value());
    EXPECT_EQ(get_result.value(), 0);
    EXPECT_EQ(::sigismember(&set_sigset, SIGUSR1), ::sigismember(&get_sigset, SIGUSR1));
}

TEST(SpawnImpl, posix_spawnattr_setsigignore_failure)
{
    posix_spawnattr_t attr;
    sigset_t set_sigset;
    ::sigemptyset(&set_sigset);

    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setsigignore(&attr, &set_sigset);
    ASSERT_FALSE(set_result.has_value());
}

TEST(SpawnImpl, posix_spawnattr_getsigignore_failure)
{
    posix_spawnattr_t attr;
    sigset_t get_sigset;
    ::sigemptyset(&get_sigset);
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getsigignore(&attr, &get_sigset);
    ASSERT_FALSE(get_result.has_value());
}

TEST_F(SpawnTest, posix_spawnattr_setstackmax_success)
{
    const std::uint32_t set_size{1};
    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setstackmax(&attr, set_size);
    ASSERT_TRUE(set_result.has_value());
    EXPECT_EQ(set_result.value(), 0);

    std::uint32_t get_size{};
    ASSERT_EQ(::posix_spawnattr_getstackmax(&attr, &get_size), 0);
    EXPECT_EQ(get_size, set_size);
}

TEST_F(SpawnTest, posix_spawnattr_getstackmax_success)
{
    const std::uint32_t set_size{1};
    ASSERT_EQ(::posix_spawnattr_setstackmax(&attr, set_size), 0);

    std::uint32_t get_size{};
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getstackmax(&attr, &get_size);
    ASSERT_TRUE(get_result.has_value());
    EXPECT_EQ(get_result.value(), 0);
    EXPECT_EQ(get_size, set_size);
}

TEST(SpawnImpl, posix_spawnattr_setstackmax_failure)
{
    posix_spawnattr_t attr;
    const std::uint32_t set_size{1};
    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setstackmax(&attr, set_size);
    ASSERT_FALSE(set_result.has_value());
}

TEST(SpawnImpl, posix_spawnattr_getstackmax_failure)
{
    posix_spawnattr_t attr;
    std::uint32_t get_size{};
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getstackmax(&attr, &get_size);
    ASSERT_FALSE(get_result.has_value());
}

TEST_F(SpawnTest, posix_spawnattr_setnode_success)
{
    const std::uint32_t set_node{1};
    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setnode(&attr, set_node);
    ASSERT_TRUE(set_result.has_value());
    EXPECT_EQ(set_result.value(), 0);

    std::uint32_t get_node{};
    ASSERT_EQ(::posix_spawnattr_getnode(&attr, &get_node), 0);
    EXPECT_EQ(get_node, set_node);
}

TEST_F(SpawnTest, posix_spawnattr_getnode_success)
{
    const std::uint32_t set_node{1};
    ASSERT_EQ(::posix_spawnattr_setnode(&attr, set_node), 0);

    std::uint32_t get_node{};
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getnode(&attr, &get_node);
    ASSERT_TRUE(get_result.has_value());
    EXPECT_EQ(get_result.value(), 0);
    EXPECT_EQ(get_node, set_node);
}

TEST(SpawnImpl, posix_spawnattr_setnode_failure)
{
    posix_spawnattr_t attr;
    const std::uint32_t set_node{1};
    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setnode(&attr, set_node);
    ASSERT_FALSE(set_result.has_value());
}

TEST(SpawnImpl, posix_spawnattr_getnode_failure)
{
    posix_spawnattr_t attr;
    std::uint32_t get_node{};
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getnode(&attr, &get_node);
    ASSERT_FALSE(get_result.has_value());
}

TEST_F(SpawnTest, posix_spawnattr_setcred_success)
{
    const uid_t set_uid{1};
    const gid_t set_gid{1};
    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setcred(&attr, set_uid, set_gid);
    ASSERT_TRUE(set_result.has_value());
    EXPECT_EQ(set_result.value(), 0);

    uid_t get_uid{};
    gid_t get_gid{};
    ASSERT_EQ(::posix_spawnattr_getcred(&attr, &get_uid, &get_gid), 0);
    EXPECT_EQ(get_uid, set_uid);
    EXPECT_EQ(get_gid, set_gid);
}

TEST_F(SpawnTest, posix_spawnattr_getcred_success)
{
    const uid_t set_uid{1};
    const gid_t set_gid{1};
    ASSERT_EQ(::posix_spawnattr_setcred(&attr, set_uid, set_gid), 0);

    uid_t get_uid{};
    gid_t get_gid{};
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getcred(&attr, &get_uid, &get_gid);
    ASSERT_TRUE(get_result.has_value());
    EXPECT_EQ(get_result.value(), 0);
    EXPECT_EQ(get_uid, set_uid);
    EXPECT_EQ(get_gid, set_gid);
}

TEST(SpawnImpl, posix_spawnattr_setcred_failure)
{
    posix_spawnattr_t attr;
    const uid_t set_uid{1};
    const gid_t set_gid{1};
    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setcred(&attr, set_uid, set_gid);
    ASSERT_FALSE(set_result.has_value());
}

TEST(SpawnImpl, posix_spawnattr_getcred_failure)
{
    posix_spawnattr_t attr;
    const uid_t set_uid{1};
    const gid_t set_gid{1};
    ASSERT_EQ(::posix_spawnattr_init(&attr), 0);
    ASSERT_EQ(::posix_spawnattr_setcred(&attr, set_uid, set_gid), 0);
    ASSERT_EQ(::posix_spawnattr_destroy(&attr), 0);

    uid_t get_uid{};
    gid_t get_gid{};
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getcred(&attr, &get_uid, &get_gid);
    ASSERT_FALSE(get_result.has_value());
    EXPECT_NE(get_uid, set_uid);
    EXPECT_NE(get_gid, set_gid);
}

TEST_F(SpawnTest, posix_spawnattr_settypeid_success)
{
    const std::uint32_t set_type_id{1};
    const auto set_result = score::os::Spawn::instance().posix_spawnattr_settypeid(&attr, set_type_id);
    ASSERT_TRUE(set_result.has_value());
    EXPECT_EQ(set_result.value(), 0);

    std::uint32_t get_type_id{};
    ASSERT_EQ(::posix_spawnattr_gettypeid(&attr, &get_type_id), 0);
    EXPECT_EQ(get_type_id, set_type_id);
}

TEST_F(SpawnTest, posix_spawnattr_gettypeid_success)
{
    const std::uint32_t set_type_id{1};
    ASSERT_EQ(::posix_spawnattr_settypeid(&attr, set_type_id), 0);

    std::uint32_t get_type_id{};
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_gettypeid(&attr, &get_type_id);
    ASSERT_TRUE(get_result.has_value());
    EXPECT_EQ(get_result.value(), 0);
    EXPECT_EQ(get_type_id, set_type_id);
}

TEST(SpawnImpl, posix_spawnattr_settypeid_failure)
{
    posix_spawnattr_t attr;
    const std::uint32_t set_type_id{1};
    const auto set_result = score::os::Spawn::instance().posix_spawnattr_settypeid(&attr, set_type_id);
    ASSERT_FALSE(set_result.has_value());
}

TEST(SpawnImpl, posix_spawnattr_gettypeid_failure)
{
    posix_spawnattr_t attr;
    const std::uint32_t set_type_id{1};
    ASSERT_EQ(::posix_spawnattr_init(&attr), 0);
    ASSERT_EQ(::posix_spawnattr_settypeid(&attr, set_type_id), 0);
    ASSERT_EQ(::posix_spawnattr_destroy(&attr), 0);

    std::uint32_t get_type_id{};
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_gettypeid(&attr, &get_type_id);
    ASSERT_FALSE(get_result.has_value());
    EXPECT_NE(get_type_id, set_type_id);
}

TEST_F(SpawnTest, posix_spawnattr_setasid_success)
{
    const std::uint32_t set_asid{1};
    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setasid(&attr, set_asid);
    ASSERT_TRUE(set_result.has_value());
    EXPECT_EQ(set_result.value(), 0);
}

TEST(SpawnImpl, posix_spawnattr_setasid_failure)
{
    posix_spawnattr_t attr;
    const std::uint32_t set_asid{1};
    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setasid(&attr, set_asid);
    ASSERT_FALSE(set_result.has_value());
}

TEST_F(SpawnTest, posix_spawnattr_setaslr_success)
{
    const posix_spawnattr_aslr_t set_aslr = {};
    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setaslr(&attr, set_aslr);
    ASSERT_TRUE(set_result.has_value());
    EXPECT_EQ(set_result.value(), 0);

    posix_spawnattr_aslr_t get_aslr;
    ASSERT_EQ(::posix_spawnattr_getaslr(&attr, &get_aslr), 0);
    EXPECT_EQ(get_aslr, set_aslr);
}

TEST_F(SpawnTest, posix_spawnattr_getaslr_success)
{
    const posix_spawnattr_aslr_t set_aslr = {};
    ASSERT_EQ(::posix_spawnattr_setaslr(&attr, set_aslr), 0);

    posix_spawnattr_aslr_t get_aslr;
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getaslr(&attr, &get_aslr);
    ASSERT_TRUE(get_result.has_value());
    EXPECT_EQ(get_result.value(), 0);
    EXPECT_EQ(get_aslr, set_aslr);
}

TEST(SpawnImpl, posix_spawnattr_setaslr_failure)
{
    posix_spawnattr_t attr;
    const posix_spawnattr_aslr_t set_aslr = {};
    const auto set_result = score::os::Spawn::instance().posix_spawnattr_setaslr(&attr, set_aslr);
    ASSERT_FALSE(set_result.has_value());
}

TEST(SpawnImpl, posix_spawnattr_getaslr_failure)
{
    posix_spawnattr_t attr;
    const posix_spawnattr_aslr_t set_aslr = {};
    ASSERT_EQ(::posix_spawnattr_init(&attr), 0);
    ASSERT_EQ(::posix_spawnattr_setaslr(&attr, set_aslr), 0);
    ASSERT_EQ(::posix_spawnattr_destroy(&attr), 0);

    posix_spawnattr_aslr_t get_aslr;
    const auto get_result = score::os::Spawn::instance().posix_spawnattr_getaslr(&attr, &get_aslr);
    ASSERT_FALSE(get_result.has_value());
    EXPECT_NE(get_aslr, set_aslr);
}

TEST_F(SpawnTest, posix_spawnattr_setcwd_np_success)
{
    const std::int32_t dirfd{1};
    const auto result = score::os::Spawn::instance().posix_spawnattr_setcwd_np(&attr, dirfd);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);
}

TEST(SpawnImpl, posix_spawnattr_setcwd_np_failure)
{
    posix_spawnattr_t attr;
    const std::int32_t dirfd{1};
    const auto result = score::os::Spawn::instance().posix_spawnattr_setcwd_np(&attr, dirfd);
    ASSERT_FALSE(result.has_value());
}

TEST_F(SpawnTest, spawn_success)
{
    std::int32_t fd_count{0};
    const std::int32_t* fd_map{nullptr};
    const struct inheritance* inherit{nullptr};
    char path[] = "/bin/ls";
    char* argv[] = {path, nullptr};
    char* envp[] = {nullptr};

    auto result = score::os::Spawn::instance().spawn(path, fd_count, fd_map, inherit, argv, envp);
    ASSERT_TRUE(result.has_value());

    int status{};
    ::waitpid(result.value(), &status, 0);
    ASSERT_TRUE(WIFEXITED(status));
}

TEST_F(SpawnTest, spawn_failure)
{
    char path[] = "/nonexistent/executable";
    std::int32_t fd_count{0};
    const std::int32_t* fd_map{nullptr};
    const struct inheritance* inherit{nullptr};
    char* const argv[] = {nullptr};
    char* const envp[] = {nullptr};

    auto result = score::os::Spawn::instance().spawn(path, fd_count, fd_map, inherit, argv, envp);
    ASSERT_FALSE(result.has_value());
}

TEST_F(SpawnTest, spawnp_success)
{
    std::int32_t fd_count{0};
    const std::int32_t* fd_map{nullptr};
    const struct inheritance* inherit{nullptr};
    char path[] = "/bin/ls";
    char* argv[] = {path, nullptr};
    char* envp[] = {nullptr};

    auto result = score::os::Spawn::instance().spawnp(path, fd_count, fd_map, inherit, argv, envp);
    ASSERT_TRUE(result.has_value());

    int status{};
    ::waitpid(result.value(), &status, 0);
    ASSERT_TRUE(WIFEXITED(status));
}

TEST_F(SpawnTest, spawnp_failure)
{
    char file[] = "/nonexistent/executable";
    std::int32_t fd_count{0};
    const std::int32_t* fd_map{nullptr};
    const struct inheritance* inherit{nullptr};
    char* const argv[] = {nullptr};
    char* const envp[] = {nullptr};

    auto result = score::os::Spawn::instance().spawnp(file, fd_count, fd_map, inherit, argv, envp);
    ASSERT_FALSE(result.has_value());
}

#endif  //__QNX__

}  // namespace test
}  // namespace os
}  // namespace score
