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
#include "score/os/qnx/sigevent_qnx_impl.h"
#include "score/os/sigevent_error.h"

#include <gtest/gtest.h>
#include <sys/siginfo.h>

namespace score::os
{

class SigEventQnxTest : public testing::Test
{
  public:
    void SetUp() override
    {
        signal_event_qnx_ = std::make_unique<SigEventQnxImpl>();
    }

    std::unique_ptr<SigEventQnx> signal_event_qnx_;

    static constexpr std::int32_t kConnectionId = 12;
    static constexpr std::int32_t kPriority = 10;
    static constexpr std::int32_t kCode = 42;
    static constexpr std::int32_t kSignalEventValue = 1337;
    static constexpr std::int32_t kSignalNumber = 30;
    static constexpr std::int32_t kSignalCode = 50;
    static constexpr pid_t kThreadId = 20;
};

TEST_F(SigEventQnxTest, SetUnblock)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventQnxTest set unblock");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    signal_event_qnx_->SetUnblock();

    const auto raw_sigevent = signal_event_qnx_->GetSigevent();
    EXPECT_EQ(raw_sigevent.sigev_notify, SIGEV_UNBLOCK);
}

TEST_F(SigEventQnxTest, SetPulse)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventQnxTest set pulse");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    signal_event_qnx_->SetPulse(kConnectionId, kPriority, kCode, kSignalEventValue);

    const auto& raw_sigevent = signal_event_qnx_->GetSigevent();
    EXPECT_EQ(raw_sigevent.sigev_notify, SIGEV_PULSE);
    EXPECT_EQ(raw_sigevent.sigev_coid, kConnectionId);
    EXPECT_EQ(raw_sigevent.sigev_priority, kPriority);
    EXPECT_EQ(raw_sigevent.sigev_code, kCode);
    EXPECT_EQ(raw_sigevent.sigev_value.sival_int, kSignalEventValue);
}

TEST_F(SigEventQnxTest, SetSignalThread)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventQnxTest set signal thread");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    signal_event_qnx_->SetSignalThread(kSignalNumber, kSignalEventValue, kThreadId);

    auto& raw_sigevent = const_cast<sigevent&>(signal_event_qnx_->GetSigevent());
    EXPECT_EQ(raw_sigevent.sigev_notify, SIGEV_SIGNAL_THREAD);
    EXPECT_EQ(raw_sigevent.sigev_signo, kSignalNumber);
    EXPECT_EQ(raw_sigevent.sigev_value.sival_int, kSignalEventValue);
}

TEST_F(SigEventQnxTest, SetSignalCode)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventQnxTest set signal code");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    signal_event_qnx_->SetSignalCode(kSignalNumber, kSignalEventValue, kSignalCode);

    const auto& raw_sigevent = signal_event_qnx_->GetSigevent();
    EXPECT_EQ(raw_sigevent.sigev_notify, SIGEV_SIGNAL_CODE);
    EXPECT_EQ(raw_sigevent.sigev_signo, kSignalNumber);
    EXPECT_EQ(raw_sigevent.sigev_code, kSignalCode);
    EXPECT_EQ(raw_sigevent.sigev_value.sival_int, kSignalEventValue);
}

TEST_F(SigEventQnxTest, SetMemory)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventQnxTest set memory");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    volatile std::uint32_t dummy_mem = 0;
    constexpr std::size_t kSize = 4;
    constexpr std::size_t kOffset = 0;

    signal_event_qnx_->SetMemory(&dummy_mem, kSize, kOffset);

    auto& raw_sigevent = const_cast<sigevent&>(signal_event_qnx_->GetSigevent());
    EXPECT_EQ(raw_sigevent.sigev_notify, SIGEV_MEMORY);
}

TEST_F(SigEventQnxTest, SetInterrupt)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventQnxTest set interrupt");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    signal_event_qnx_->SetInterrupt();

    const auto& raw_sigevent = signal_event_qnx_->GetSigevent();
    EXPECT_EQ(raw_sigevent.sigev_notify, SIGEV_INTR);
}

}  // namespace score::os
