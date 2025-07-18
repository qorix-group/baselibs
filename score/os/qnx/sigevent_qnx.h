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
#ifndef SCORE_LIB_OS_QNX_SIGEVENT_QNX_H
#define SCORE_LIB_OS_QNX_SIGEVENT_QNX_H

#include "score/os/sigevent.h"

namespace score::os
{

class SigEventQnx : public SigEvent
{
  public:
    virtual ~SigEventQnx() = default;

    // extend the SigEvent abstract class
    virtual ResultBlank SetNotificationType(const NotificationType notification_type) override = 0;
    virtual ResultBlank SetSignalNumber(const std::int32_t signal_number) override = 0;
    virtual ResultBlank SetSignalEventValue(const std::variant<int, void*> val) override = 0;
    virtual ResultBlank SetThreadCallback(const SigValCallback callback) override = 0;
    virtual ResultBlank SetThreadAttributes(pthread_attr_t& attr) override = 0;
    virtual const sigevent& GetSigevent() const override = 0;
    virtual sigevent& GetSigevent() override = 0;
    virtual void Reset() override = 0;

    // add QNX-specific functionality
    virtual void SetUnblock() = 0;
    virtual void SetPulse(const std::int32_t connection_id,
                          const std::int32_t priority,
                          const std::int32_t code,
                          const std::int32_t value) = 0;
    virtual void SetSignalThread(const std::int32_t signo, const std::int32_t value, const pid_t tid) = 0;
    virtual void SetSignalCode(const std::int32_t signo, const std::int32_t value, const std::int32_t code) = 0;
    virtual void SetMemory(volatile std::uint32_t* addr, std::size_t size, std::size_t offset) = 0;
    virtual void SetInterrupt() = 0;
};

}  // namespace score::os

#endif  // SCORE_LIB_OS_QNX_SIGEVENT_QNX_H
