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
#ifndef SCORE_LIB_OS_SIGEVENT_QNX_IMPL_H
#define SCORE_LIB_OS_SIGEVENT_QNX_IMPL_H

#include "score/os/qnx/sigevent_qnx.h"

namespace score::os
{

class SigEventQnxImpl final : public SigEventQnx
{
  public:
    SigEventQnxImpl(std::unique_ptr<SigEvent> signal_event = nullptr);
    ~SigEventQnxImpl() override = default;

    // override the common SigEvent functions
    ResultBlank SetNotificationType(const NotificationType notification_type) override;
    ResultBlank SetSignalNumber(const std::int32_t signal_number) override;
    ResultBlank SetSignalEventValue(const std::variant<int32_t, void*> val) override;
    ResultBlank SetThreadCallback(const SigValCallback callback) override;
    ResultBlank SetThreadAttributes(pthread_attr_t& attr) override;
    const sigevent& GetSigevent() const override;
    void Reset() override;

    // override the QNX-specific SigEventQnx functions
    void SetUnblock() override;
    void SetPulse(const std::int32_t connection_id,
                  const std::int32_t priority,
                  const std::int32_t code,
                  const std::int32_t value) override;
    void SetSignalThread(const std::int32_t signal_number, const std::int32_t value, const pid_t tid) override;
    void SetSignalCode(const std::int32_t signal_number, const std::int32_t value, const std::int32_t code) override;
    void SetMemory(volatile std::uint32_t* addr, std::size_t size, std::size_t offset) override;
    void SetInterrupt() override;

  private:
    sigevent& GetSigevent() override;

    std::unique_ptr<SigEvent> signal_event_;
};

}  // namespace score::os

#endif  // SCORE_LIB_OS_SIGEVENT_QNX_IMPL_H
