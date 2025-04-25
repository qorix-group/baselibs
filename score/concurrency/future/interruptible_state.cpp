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
#include "score/concurrency/future/interruptible_state.h"

bool score::concurrency::InterruptibleState<void>::SetValue()
{
    if ((this->TestAndMarkValueAsSet()) == true)
    {
        return false;
    }

    MakeReady();
    TriggerContinuations();
    return true;
}

bool score::concurrency::InterruptibleState<void>::SetError(score::result::Error error)
{
    if ((this->TestAndMarkValueAsSet()) == true)
    {
        return false;
    }

    // Use the constructor instead of assignment operator to circumvent issue with types that are not assignable
    // NOLINTNEXTLINE(score-no-dynamic-raw-memory): Non-assignable types workaround
    new (&value_) score::ResultBlank{MakeUnexpected<score::Blank>(error)};

    MakeReady();
    TriggerContinuations();
    return true;
}

score::ResultBlank& score::concurrency::InterruptibleState<void>::GetValue() noexcept
{
    // Suppress "AUTOSAR C++14 A9-3-1" rule finding: "Member functions shall not return non-const “raw” pointers or
    // references to private or protected data owned by the class."
    // Justification: Intended by implementation, caller e.i. InterruptibleSharedFuture requires a reference.
    // coverity[autosar_cpp14_a9_3_1_violation]
    return value_;
}

void score::concurrency::InterruptibleState<void>::AddContinuationCallback(ScopedContinuationCallback callback)
{
    RegisterFuture();
    std::lock_guard<std::mutex> lock{continuation_callback_mutex_};
    if ((this->TestIfValueIsSet()) == true)
    {
        score::cpp::ignore = callback(value_);
    }
    else
    {
        continuation_callbacks_.push_back(std::move(callback));
    }
}

void score::concurrency::InterruptibleState<void>::TriggerContinuations()
{
    std::lock_guard<std::mutex> lock{continuation_callback_mutex_};
    for (auto& callback : continuation_callbacks_)
    {
        score::cpp::ignore = callback(value_);
    }
    continuation_callbacks_.clear();
}

const score::safecpp::Scope<>& score::concurrency::InterruptibleState<void>::GetScope() const noexcept
{
    return scope_;
}
