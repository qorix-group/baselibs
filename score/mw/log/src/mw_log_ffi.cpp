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
#include "score/mw/log/logger.h"
#include "score/mw/log/log_level.h"

namespace score
{
namespace mw
{
namespace log
{

extern "C" {

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

    Logger* mw_log_create_logger(const char* context) {
	return &CreateLogger(context);
    }

    void mw_log_fatal_logger(const Logger* logger, const char* message) {
        logger->LogFatal() << message;
    }

    void mw_log_error_logger(const Logger* logger, const char* message) {
        logger->LogError() << message;
    }

    void mw_log_warn_logger(const Logger* logger, const char* message) {
        logger->LogWarn() << message;
    }

    void mw_log_info_logger(const Logger* logger, const char* message) {
        logger->LogInfo() << message;
    }

    void mw_log_debug_logger(const Logger* logger, const char* message) {
        logger->LogDebug() << message;
    }

    void mw_log_verbose_logger(const Logger* logger, const char* message) {
        logger->LogVerbose() << message;
    }

    void mw_log_fatal_logger_additional(const Logger* logger, const char* info, const char* message) {
        logger->LogFatal() << info  << message;
    }

    void mw_log_error_logger_additional(const Logger* logger, const char* info, const char* message) {
        logger->LogError() << info << message;
    }

    void mw_log_warn_logger_additional(const Logger* logger, const char* info, const char* message) {
        logger->LogWarn() << info << message;
    }

    void mw_log_info_logger_additional(const Logger* logger, const char* info, const char* message) {
        logger->LogInfo() << info << message;
    }

    void mw_log_debug_logger_additional(const Logger* logger, const char* info, const char* message) {
        logger->LogDebug() << info << message;
    }

    void mw_log_verbose_logger_additional(const Logger* logger, const char* info, const char* message) {
        logger->LogVerbose() << info << message;
    }
    // Returns the max log level as uint8_t
    uint8_t mw_get_max_log_level() {
        return static_cast<uint8_t>(score::mw::log::GetMaxLogLevelValue());
    }
}

}  // namespace log
}  // namespace mw
}  // namespace score
