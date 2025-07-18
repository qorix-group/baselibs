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
#include "score/os/sigevent_impl.h"
#include "score/mw/log/logging.h"

#include <sys/siginfo.h>

namespace score::os
{

SigEventQnxImpl::SigEventQnxImpl(std::unique_ptr<SigEvent> signal_event)
    : signal_event_{signal_event != nullptr ? std::move(signal_event) : std::make_unique<SigEventImpl>()}
{
}

ResultBlank SigEventQnxImpl::SetNotificationType(const NotificationType notification_type)
{
    return signal_event_->SetNotificationType(notification_type);
}

ResultBlank SigEventQnxImpl::SetSignalNumber(const std::int32_t signal_number)
{
    return signal_event_->SetSignalNumber(signal_number);
}

ResultBlank SigEventQnxImpl::SetSignalEventValue(const std::variant<std::int32_t, void*> val)
{
    const auto& raw_sigevent = signal_event_->GetSigevent();
    if (raw_sigevent.sigev_notify == SIGEV_UNBLOCK)
    {
        mw::log::LogError() << __func__ << "Invalid signal event notification type";
        return MakeUnexpected(SigEventErrorCode::kInvalidSignalEventNotificationType);
    }
    return signal_event_->SetSignalEventValue(val);
}

ResultBlank SigEventQnxImpl::SetThreadCallback(const SigValCallback callback)
{
    return signal_event_->SetThreadCallback(callback);
}

ResultBlank SigEventQnxImpl::SetThreadAttributes(pthread_attr_t& attr)
{
    return signal_event_->SetThreadAttributes(attr);
}

const sigevent& SigEventQnxImpl::GetSigevent() const
{
    return signal_event_->GetSigevent();
}

sigevent& SigEventQnxImpl::GetSigevent()
{
    return signal_event_->GetSigevent();
}

void SigEventQnxImpl::Reset()
{
    signal_event_->Reset();
}

void SigEventQnxImpl::SetUnblock()
{
    mw::log::LogDebug() << __func__ << "called";
    auto& raw_signal_event = signal_event_->GetSigevent();
    SIGEV_UNBLOCK_INIT(&raw_signal_event);
}

void SigEventQnxImpl::SetPulse(const std::int32_t connection_id,
                               const std::int32_t priority,
                               const std::int32_t code,
                               const std::int32_t value)
{
    mw::log::LogDebug() << __func__ << "called";
    auto& raw_signal_event = signal_event_->GetSigevent();
    SIGEV_PULSE_INIT(&raw_signal_event, connection_id, priority, code, value);
}

void SigEventQnxImpl::SetSignalThread(const std::int32_t signal_number, const std::int32_t value, const pid_t tid)
{
    mw::log::LogDebug() << __func__ << " called";
    auto& raw_signal_event = signal_event_->GetSigevent();
    SIGEV_SIGNAL_THREAD_INIT(&raw_signal_event, signal_number, value, tid);
}

void SigEventQnxImpl::SetSignalCode(const std::int32_t signal_number, const std::int32_t value, const std::int32_t code)
{
    mw::log::LogDebug() << __func__ << " called";
    auto& raw_signal_event = signal_event_->GetSigevent();
    SIGEV_SIGNAL_CODE_INIT(&raw_signal_event, signal_number, value, code);
}

void SigEventQnxImpl::SetMemory(volatile std::uint32_t* addr, std::size_t size, std::size_t offset)
{
    mw::log::LogDebug() << __func__ << " called";
    auto& raw_signal_event = signal_event_->GetSigevent();
    SIGEV_MEMORY_INIT(&raw_signal_event, addr, size, offset);
}

void SigEventQnxImpl::SetInterrupt()
{
    mw::log::LogDebug() << __func__ << " called";
    auto& raw_signal_event = signal_event_->GetSigevent();
    SIGEV_INTR_INIT(&raw_signal_event);
}

}  // namespace score::os
