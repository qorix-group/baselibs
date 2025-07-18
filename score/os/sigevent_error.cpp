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

namespace score::os
{

std::string_view SigEventErrorCodeDomain::MessageFor(const score::result::ErrorCode& code) const noexcept
{
    switch (static_cast<SigEventErrorCode>(code))
    {
        case SigEventErrorCode::kInvalidArgument:
        {
            return "Invalid argument";
        }
        case SigEventErrorCode::kInvalidNotificationType:
        {
            return "Invalid notification type";
        }
        case SigEventErrorCode::kInvalidConnectionIdNotificationType:
        {
            return "Invalid notification type for the connection id";
        }
        case SigEventErrorCode::kInvalidThreadAttributesNotificationType:
        {
            return "Invalid notification type for the thread attributes";
        }
        case SigEventErrorCode::kInvalidThreadIdNotificationType:
        {
            return "Invalid notification type for the thread id";
        }
        case SigEventErrorCode::kInvalidThreadCallbackNotificationType:
        {
            return "Invalid notification type for the thread callback";
        }
        case SigEventErrorCode::kInvalidSignalEventNotificationType:
        {
            return "Invalid signal event notification type";
        }
        case SigEventErrorCode::kInvalidSignalEventValue:
        {
            return "Invalid signal event value";
        }
        case SigEventErrorCode::kInvalidSignalNumber:
        {
            return "Invalid signal number";
        }
        default:
        {
            return "Unknown error";
        }
    }
}

namespace
{
constexpr SigEventErrorCodeDomain sig_event_error_code_domain;
}

result::Error MakeError(const SigEventErrorCode code, const std::string_view user_message) noexcept
{
    return {static_cast<score::result::ErrorCode>(code), sig_event_error_code_domain, user_message};
}

}  // namespace score::os
