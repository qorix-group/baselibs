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
use core::ffi::c_char;

// This is just a fake Rust struct to store the class we get from c++ side
// This type is empty.
#[repr(C)]
pub(crate) struct Logger {
    _private: [u8; 0], // Opaque
}

extern "C" {

    pub(crate) fn mw_log_fatal_context(message: *const c_char, context: *const c_char);
    pub(crate) fn mw_log_error_context(message: *const c_char, context: *const c_char);
    pub(crate) fn mw_log_warn_context(message: *const c_char, context: *const c_char);
    pub(crate) fn mw_log_info_context(message: *const c_char, context: *const c_char);
    pub(crate) fn mw_log_debug_context(message: *const c_char, context: *const c_char);
    pub(crate) fn mw_log_verbose_context(message: *const c_char, context: *const c_char);

    pub(crate) fn mw_log_fatal_additional_context(
        info: *const c_char,
        message: *const c_char,
        context: *const c_char,
    );
    pub(crate) fn mw_log_error_additional_context(
        info: *const c_char,
        message: *const c_char,
        context: *const c_char,
    );
    pub(crate) fn mw_log_warn_additional_context(
        info: *const c_char,
        message: *const c_char,
        context: *const c_char,
    );
    pub(crate) fn mw_log_info_additional_context(
        info: *const c_char,
        message: *const c_char,
        context: *const c_char,
    );
    pub(crate) fn mw_log_debug_additional_context(
        info: *const c_char,
        message: *const c_char,
        context: *const c_char,
    );
    pub(crate) fn mw_log_verbose_additional_context(
        info: *const c_char,
        message: *const c_char,
        context: *const c_char,
    );

    pub(crate) fn mw_log_create_logger(context: *const c_char) -> *const Logger;

    pub(crate) fn mw_log_fatal_logger(logger: *const Logger, message: *const c_char);
    pub(crate) fn mw_log_error_logger(logger: *const Logger, message: *const c_char);
    pub(crate) fn mw_log_warn_logger(logger: *const Logger, message: *const c_char);
    pub(crate) fn mw_log_info_logger(logger: *const Logger, message: *const c_char);
    pub(crate) fn mw_log_debug_logger(logger: *const Logger, message: *const c_char);
    pub(crate) fn mw_log_verbose_logger(logger: *const Logger, message: *const c_char);

    pub(crate) fn mw_log_fatal_logger_additional(
        logger: *const Logger,
        info: *const c_char,
        message: *const c_char,
    );
    pub(crate) fn mw_log_error_logger_additional(
        logger: *const Logger,
        info: *const c_char,
        message: *const c_char,
    );
    pub(crate) fn mw_log_warn_logger_additional(
        logger: *const Logger,
        info: *const c_char,
        message: *const c_char,
    );
    pub(crate) fn mw_log_info_logger_additional(
        logger: *const Logger,
        info: *const c_char,
        message: *const c_char,
    );
    pub(crate) fn mw_log_debug_logger_additional(
        logger: *const Logger,
        info: *const c_char,
        message: *const c_char,
    );
    pub(crate) fn mw_log_verbose_logger_additional(
        logger: *const Logger,
        info: *const c_char,
        message: *const c_char,
    );
    pub fn mw_get_max_log_level() -> u8;

}
