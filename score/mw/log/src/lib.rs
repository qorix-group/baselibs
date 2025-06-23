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
mod mw_log_ffi;

use log::{Level, LevelFilter, Log, Metadata, Record};

use crate::mw_log_ffi::{
    mw_log_debug, mw_log_error, mw_log_info, mw_log_verbose, mw_log_warn,
};

pub fn init_logging(level: LevelFilter) {
    let logger = MwLogger{level};
    log::set_max_level(level);
    log::set_boxed_logger(Box::new(logger)).expect("failed to set logger");
}

struct MwLogger{
    level: LevelFilter,
}

impl Log for MwLogger {
       fn enabled(&self, metadata: &Metadata) -> bool {
           metadata.level() <= self.level
       }

       fn log(&self, record: &Record) {
           if self.enabled(record.metadata()) {
               let message = record.args().to_string();
               match record.level() {
                   //Level::Fatal => unsafe { mw_log_fatal(message.as_ptr() as *const _) },
                   Level::Error => unsafe { mw_log_error(message.as_ptr() as *const _) },
                   Level::Warn => unsafe { mw_log_warn(message.as_ptr() as *const _) },
                   Level::Info => unsafe { mw_log_info(message.as_ptr() as *const _) },
                   Level::Debug => unsafe { mw_log_debug(message.as_ptr() as *const _) },
                   Level::Trace => unsafe { mw_log_verbose(message.as_ptr() as *const _) },
               }
           }
       }

       fn flush(&self) {}
}
