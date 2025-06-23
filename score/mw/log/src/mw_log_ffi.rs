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
     pub(crate) fn mw_log_fatal(message: *const c_char);
     pub(crate) fn mw_log_error(message: *const c_char);
     pub(crate) fn mw_log_warn(message: *const c_char);
     pub(crate) fn mw_log_info(message: *const c_char);
     pub(crate) fn mw_log_debug(message: *const c_char);
     pub(crate) fn mw_log_verbose(message: *const c_char);
 }
