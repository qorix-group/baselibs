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
#ifndef SCORE_LIB_OS_SIGEVENT_H
#define SCORE_LIB_OS_SIGEVENT_H

#include "score/os/ObjectSeam.h"
#include "score/result/result.h"

#include <cstdint>
#include <variant>

struct sigevent;
union sigval;

namespace score::os
{

using SigValCallback = void (*)(sigval);

class SigEvent : public ObjectSeam<SigEvent>
{
  public:
    enum class NotificationType
    {
        kNone,
        kSignal,
        kThread,
    };

    virtual ~SigEvent() = default;
    virtual ResultBlank SetNotificationType(const NotificationType notification_type) = 0;
    virtual ResultBlank SetSignalNumber(const std::int32_t signal_number) = 0;
    virtual ResultBlank SetSignalEventValue(const std::variant<int32_t, void*> val) = 0;
    virtual ResultBlank SetThreadCallback(const SigValCallback callback) = 0;
    virtual ResultBlank SetThreadAttributes(pthread_attr_t& attr) = 0;
    virtual const sigevent& GetSigevent() const = 0;
    virtual sigevent& GetSigevent() = 0;
    virtual void Reset() = 0;
};

}  // namespace score::os

#endif
