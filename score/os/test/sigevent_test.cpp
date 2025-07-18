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
#include "score/os/sigevent_error.h"
#include "score/os/sigevent_impl.h"

#include <gtest/gtest.h>

namespace score::os
{

class SigEventTest : public testing::Test
{
  public:
    void SetUp() override
    {
        signal_event_ = std::make_unique<SigEventImpl>();
    }

    std::unique_ptr<SigEvent> signal_event_;
};

TEST_F(SigEventTest, SetNotificationType)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventTest set notification types");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_TRUE(signal_event_->SetNotificationType(SigEvent::NotificationType::kNone).has_value());
    EXPECT_TRUE(signal_event_->SetNotificationType(SigEvent::NotificationType::kSignal).has_value());
    EXPECT_TRUE(signal_event_->SetNotificationType(SigEvent::NotificationType::kThread).has_value());
    EXPECT_FALSE(signal_event_->SetNotificationType(static_cast<SigEvent::NotificationType>(42U)).has_value());
}

TEST_F(SigEventTest, SetSignalNumber)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventTest set signal number");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto result = signal_event_->SetSignalNumber(0);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), SigEventErrorCode::kInvalidSignalNumber);

    result = signal_event_->SetSignalNumber(NSIG);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), SigEventErrorCode::kInvalidSignalNumber);

    result = signal_event_->SetSignalNumber(SIGUSR1);
    EXPECT_TRUE(result.has_value());
}

TEST_F(SigEventTest, SetSignalEventValue)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventTest set signal event value");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::variant<int, void*> signal_event_value;
    bool value = false;
    signal_event_value = reinterpret_cast<void*>(&value);

    auto result = signal_event_->SetNotificationType(SigEvent::NotificationType::kNone);
    EXPECT_TRUE(result.has_value());
    result = signal_event_->SetSignalEventValue(signal_event_value);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), SigEventErrorCode::kInvalidSignalEventNotificationType);

    result = signal_event_->SetNotificationType(SigEvent::NotificationType::kSignal);
    EXPECT_TRUE(result.has_value());
    result = signal_event_->SetSignalEventValue(signal_event_value);
    EXPECT_TRUE(result.has_value());

    signal_event_value = nullptr;
    result = signal_event_->SetSignalEventValue(signal_event_value);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), SigEventErrorCode::kInvalidSignalEventValue);

    signal_event_value = 42;
    result = signal_event_->SetSignalEventValue(signal_event_value);
    EXPECT_TRUE(result.has_value());
}

TEST_F(SigEventTest, SetThreadCallback)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventTest set thread callback");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto result = signal_event_->SetNotificationType(SigEvent::NotificationType::kSignal);
    EXPECT_TRUE(result.has_value());

    result = signal_event_->SetThreadCallback(nullptr);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), SigEventErrorCode::kInvalidThreadCallbackNotificationType);

    result = signal_event_->SetNotificationType(SigEvent::NotificationType::kThread);
    EXPECT_TRUE(result.has_value());

    result = signal_event_->SetThreadCallback(nullptr);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), SigEventErrorCode::kInvalidArgument);

    auto callback = [](sigval) {};
    result = signal_event_->SetThreadCallback(callback);
    EXPECT_TRUE(result.has_value());
}

TEST_F(SigEventTest, SetThreadAttributes)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventTest set thread attributes");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto result = signal_event_->SetNotificationType(SigEvent::NotificationType::kSignal);
    EXPECT_TRUE(result.has_value());

    pthread_attr_t attributes{};
    result = signal_event_->SetThreadAttributes(attributes);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), SigEventErrorCode::kInvalidThreadAttributesNotificationType);

    result = signal_event_->SetNotificationType(SigEvent::NotificationType::kThread);
    EXPECT_TRUE(result.has_value());
    result = signal_event_->SetThreadAttributes(attributes);
    EXPECT_TRUE(result.has_value());
}

TEST_F(SigEventTest, Reset)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventTest reset sigevent");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto result = signal_event_->SetNotificationType(SigEvent::NotificationType::kThread);
    EXPECT_TRUE(result.has_value());
    pthread_attr_t attributes{};
    result = signal_event_->SetThreadAttributes(attributes);
    EXPECT_TRUE(result.has_value());

    auto callback = [](sigval) {};
    signal_event_->SetThreadCallback(callback);
    EXPECT_TRUE(result.has_value());

    signal_event_->SetSignalNumber(SIGUSR1);
    EXPECT_TRUE(result.has_value());

    signal_event_->Reset();
    const auto& signal_event = signal_event_->GetSigevent();
    EXPECT_NE(signal_event.sigev_signo, SIGUSR1);
    EXPECT_NE(signal_event.sigev_notify, SIGEV_THREAD);
    EXPECT_NE(signal_event.sigev_notify_function, callback);
}

}  // namespace score::os
