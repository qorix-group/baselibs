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
#include "score/os/mocklib/qnx/mock_procmgr.h"
#include "score/os/mocklib/qnx/mock_procmgr_daemon_raw.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace
{

constexpr pid_t kCurrentPid{0};
constexpr pid_t kInvalidPid{INT_MAX};

// Mock test – uses ObjectSeam to inject a full MockProcMgr (tests the mock hook-up).

struct ProcMgrMockTest : ::testing::Test
{
    void SetUp() override
    {
        score::os::ProcMgr::set_testing_instance(procmgrmock);
    };
    void TearDown() override
    {
        score::os::ProcMgr::restore_instance();
    };

    score::os::MockProcMgr procmgrmock;
};

// Real-impl test – uses the link-time seam to mock only the raw ::procmgr_daemon
// QNX syscall, so ProcMgrImpl::procmgr_daemon is exercised end-to-end.
struct ProcMgrRealImplDaemonTest : ::testing::Test
{
    void SetUp() override
    {
        score::os::SetProcMgrDaemonRawMock(&daemon_raw_mock_);
    };
    void TearDown() override
    {
        score::os::SetProcMgrDaemonRawMock(nullptr);
    };

    ::testing::StrictMock<score::os::MockProcMgrDaemonRaw> daemon_raw_mock_;
};

TEST_F(ProcMgrMockTest, procmgr_ability)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Ability");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(procmgrmock, procmgr_ability(kCurrentPid, PROCMGR_AID_EOL));
    score::os::ProcMgr::instance().procmgr_ability(kCurrentPid, PROCMGR_AID_EOL);
}

TEST(ProcMgrTest, procmgr_subrange_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Subrange Fails");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_FALSE(score::os::ProcMgr::instance().procmgr_ability(kCurrentPid, PROCMGR_AID_EOL | PROCMGR_AOP_SUBRANGE));
}

TEST(ProcMgrTest, procmgr_generic_invalid_pid_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Generic Invalid Pid Fails");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_FALSE(score::os::ProcMgr::instance().procmgr_ability(kInvalidPid, PROCMGR_AID_EOL));
}

TEST(ProcMgrTest, procmgr_invalid_ability_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Invalid Ability Fails");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_FALSE(score::os::ProcMgr::instance().procmgr_ability(
        kCurrentPid, PROCMGR_ADN_ROOT | PROCMGR_AOP_ALLOW | PROCMGR_AID_PUBLIC_CHANNEL | PROCMGR_AID_UNCREATED));
}

TEST(ProcMgrTest, procmgr_generic_succeeds)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Generic Succeeds");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_TRUE(score::os::ProcMgr::instance().procmgr_ability(kCurrentPid,
                                                             PROCMGR_ADN_ROOT | PROCMGR_AOP_ALLOW | PROCMGR_AID_EOL));
}

TEST(ProcMgrTest, procmgr_specific_succeeds)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Specific Succeeds");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_TRUE(score::os::ProcMgr::instance().procmgr_ability(
        kCurrentPid, PROCMGR_ADN_ROOT | PROCMGR_AOP_ALLOW | PROCMGR_AID_PUBLIC_CHANNEL));
}

TEST(ProcMgrTest, procmgr_ability_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Ability Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto result = score::os::ProcMgr::instance().procmgr_ability(
        kCurrentPid,
        PROCMGR_ADN_NONROOT | PROCMGR_AOP_ALLOW | PROCMGR_AID_SPAWN_SETUID,
        PROCMGR_ADN_NONROOT | PROCMGR_AOP_SUBRANGE | PROCMGR_AOP_LOCK | PROCMGR_AID_SPAWN_SETUID,
        10000U,
        ~0U,
        PROCMGR_ADN_ROOT | PROCMGR_AOP_DENY | PROCMGR_AOP_LOCK | PROCMGR_AID_EOL);
    EXPECT_TRUE(result.has_value());
}

TEST(ProcMgrTest, procmgr_ability_failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Ability Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto result =
        score::os::ProcMgr::instance().procmgr_ability(kInvalidPid, PROCMGR_AID_EOL, 0U, 0U, 0U, PROCMGR_AID_EOL);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kNoSuchProcess);
}

TEST_F(ProcMgrRealImplDaemonTest, procmgr_daemon_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Daemon Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(daemon_raw_mock_,
                procmgr_daemon(kCurrentPid,
                               PROCMGR_DAEMON_KEEPUMASK | PROCMGR_DAEMON_NOCHDIR | PROCMGR_DAEMON_NOCLOSE |
                                   PROCMGR_DAEMON_NODEVNULL))
        .WillOnce(::testing::Return(0));

    auto result = score::os::ProcMgr::instance().procmgr_daemon(
        kCurrentPid,
        PROCMGR_DAEMON_KEEPUMASK | PROCMGR_DAEMON_NOCHDIR | PROCMGR_DAEMON_NOCLOSE | PROCMGR_DAEMON_NODEVNULL);
    EXPECT_TRUE(result.has_value());
}

TEST_F(ProcMgrRealImplDaemonTest, procmgr_daemon_failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Daemon Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(daemon_raw_mock_, procmgr_daemon(kCurrentPid, 0U)).WillOnce(::testing::Return(-1));

    auto result = score::os::ProcMgr::instance().procmgr_daemon(kCurrentPid, 0U);
    EXPECT_FALSE(result.has_value());
    // ProcMgrImpl passes the return value (-1) to createFromErrno rather than errno,
    // so the error code maps to kUnexpected.
    EXPECT_EQ(result.error(), score::os::Error::Code::kUnexpected);
}

}  // namespace
