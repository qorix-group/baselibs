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
#include "score/mw/log/logging.h"
#include "score/mw/log/configuration/configuration.h"

namespace score
{
namespace mw
{
namespace log
{

extern "C" {
    void mw_log_fatal(const char* message) {
        LogFatal() << message;
    }

    void mw_log_error(const char* message) {
        LogError() << message;
    }

    void mw_log_warn(const char* message) {
        LogWarn() << message;
    }

    void mw_log_info(const char* message) {
        LogInfo() << message;
    }

    void mw_log_debug(const char* message) {
        LogDebug() << message;
    }

    void mw_log_verbose(const char* message) {
        LogVerbose() << message;
    }
}

}  // namespace log
}  // namespace mw
}  // namespace score
