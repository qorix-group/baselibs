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

extern "C" {
    #[allow(dead_code)]
    pub(crate) fn mw_log_fatal(message: *const c_char);
    pub(crate) fn mw_log_error(message: *const c_char);
    pub(crate) fn mw_log_warn(message: *const c_char);
    pub(crate) fn mw_log_info(message: *const c_char);
    pub(crate) fn mw_log_debug(message: *const c_char);
    pub(crate) fn mw_log_verbose(message: *const c_char);

    pub(crate) fn mw_log_fatal_context(message: *const c_char, context: *const c_char);
    pub(crate) fn mw_log_error_context(message: *const c_char, context: *const c_char);
    pub(crate) fn mw_log_warn_context(message: *const c_char, context: *const c_char);
    pub(crate) fn mw_log_info_context(message: *const c_char, context: *const c_char);
    pub(crate) fn mw_log_debug_context(message: *const c_char, context: *const c_char);
    pub(crate) fn mw_log_verbose_context(message: *const c_char, context: *const c_char);

    #[allow(dead_code)]
    pub(crate) fn mw_log_fatal_additional(info: *const c_char, message: *const c_char);
    pub(crate) fn mw_log_error_additional(info: *const c_char, message: *const c_char);
    pub(crate) fn mw_log_warn_additional(info: *const c_char, message: *const c_char);
    pub(crate) fn mw_log_info_additional(info: *const c_char, message: *const c_char);
    pub(crate) fn mw_log_debug_additional(info: *const c_char, message: *const c_char);
    pub(crate) fn mw_log_verbose_additional(info: *const c_char, message: *const c_char);

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
}
