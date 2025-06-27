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

    void mw_log_fatal_context(const char* message, const char* context) {
        LogFatal(context) << message;
    }

    void mw_log_error_context(const char* message, const char* context) {
        LogError(context) << message;
    }

    void mw_log_warn_context(const char* message, const char* context) {
        LogWarn(context) << message;
    }

    void mw_log_info_context(const char* message, const char* context) {
        LogInfo(context) << message;
    }

    void mw_log_debug_context(const char* message, const char* context) {
        LogDebug(context) << message;
    }

    void mw_log_verbose_context(const char* message, const char* context) {
        LogVerbose(context) << message;
    }


    void mw_log_fatal_additional(const char* info, const char* message) {
        LogFatal() << info << message;
    }

    void mw_log_error_additional(const char* info, const char* message) {
        LogError() << info << message;
    }

    void mw_log_warn_additional(const char* info, const char* message) {
        LogWarn() << info << message;
    }

    void mw_log_info_additional(const char* info, const char* message) {
        LogInfo() << info << message;
    }

    void mw_log_debug_additional(const char* info, const char* message) {
        LogDebug() << info << message;
    }

    void mw_log_verbose_additional(const char* info, const char* message) {
        LogVerbose() << info << message;
    }

    void mw_log_fatal_additional_context(const char* info, const char* message, const char* context) {
        LogFatal(context) << info << message;
    }

    void mw_log_error_additional_context(const char* info, const char* message, const char* context) {
        LogError(context) << info << message;
    }

    void mw_log_warn_additional_context(const char* info, const char* message, const char* context) {
        LogWarn(context) << info << message;
    }

    void mw_log_info_additional_context(const char* info, const char* message, const char* context) {
        LogInfo(context) << info << message;
    }

    void mw_log_debug_additional_context(const char* info, const char* message, const char* context) {
        LogDebug(context) << info << message;
    }

    void mw_log_verbose_additional_context(const char* info, const char* message, const char* context) {
        LogVerbose(context) << info << message;
    }
}

}  // namespace log
}  // namespace mw
}  // namespace score
