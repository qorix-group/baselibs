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

use core::fmt::{self, Write};
use log::{Level, LevelFilter, Log, Metadata, Record};

use crate::mw_log_ffi::{
    mw_log_debug, mw_log_debug_additional, mw_log_error, mw_log_error_additional, mw_log_fatal,
    mw_log_fatal_additional, mw_log_info, mw_log_info_additional, mw_log_verbose,
    mw_log_verbose_additional, mw_log_warn, mw_log_warn_additional,
};

pub struct MwLoggerBuilder {
    level: LevelFilter,
}

impl MwLoggerBuilder {
    pub fn new() -> Self {
        Self {
            level: LevelFilter::Info,
        }
    }

    pub fn filter_level(mut self, level: LevelFilter) -> Self {
        self.level = level;
        self
    }

    pub fn init<const SHOW_MODULE: bool, const SHOW_FILE: bool, const SHOW_LINE: bool>(&self) {
        let logger: MwLogger<SHOW_MODULE, SHOW_FILE, SHOW_LINE> = MwLogger { level: self.level };
        log::set_max_level(self.level);
        log::set_boxed_logger(Box::new(logger)).expect("failed to set logger");
    }
}

struct BufWriter {
    buf: [u8; 256],
    pos: usize,
}

impl BufWriter {
    fn new() -> Self {
        Self {
            buf: [0; 256],
            pos: 0,
        }
    }
}

impl Write for BufWriter {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        let bytes = s.as_bytes();
        let remaining = self.buf.len() - self.pos;
        if bytes.len() > remaining {
            return Err(fmt::Error);
        }

        self.buf[self.pos..self.pos + bytes.len()].copy_from_slice(bytes);
        self.pos += bytes.len();
        Ok(())
    }
}

struct MwLogger<const SHOW_MODULE: bool, const SHOW_FILE: bool, const SHOW_LINE: bool> {
    level: LevelFilter,
}

impl<const SHOW_MODULE: bool, const SHOW_FILE: bool, const SHOW_LINE: bool> Log
    for MwLogger<SHOW_MODULE, SHOW_FILE, SHOW_LINE>
{
    fn enabled(&self, metadata: &Metadata) -> bool {
        metadata.level() <= self.level
    }

    fn log(&self, record: &Record) {
        if self.enabled(record.metadata()) {
            let message = record.args().to_string();
            if SHOW_FILE || SHOW_LINE || SHOW_MODULE {
                let mut info = BufWriter::new();
                let _ = write!(info, "[");

                if SHOW_MODULE {
                    if let Some(module) = record.module_path() {
                        let _ = write!(info, "{}", module);
                        if SHOW_FILE || SHOW_LINE {
                            let _ = write!(info, ":");
                        }
                    }
                }

                if SHOW_FILE {
                    if let Some(file) = record.file() {
                        let _ = write!(info, "{}", file);
                        if SHOW_LINE {
                            let _ = write!(info, ":");
                        }
                    }
                }

                if SHOW_LINE {
                    if let Some(line) = record.line() {
                        let _ = write!(info, "{}", line);
                    }
                }
                let _ = write!(info, "]");
                match record.level() {
                    Level::Fatal => unsafe {
                        mw_log_fatal_additional(
                            info.buf.as_ptr() as *const _,
                            message.as_ptr() as *const _,
                        )
                    },
                    Level::Error => unsafe {
                        mw_log_error_additional(
                            info.buf.as_ptr() as *const _,
                            message.as_ptr() as *const _,
                        )
                    },
                    Level::Warn => unsafe {
                        mw_log_warn_additional(
                            info.buf.as_ptr() as *const _,
                            message.as_ptr() as *const _,
                        )
                    },
                    Level::Info => unsafe {
                        mw_log_info_additional(
                            info.buf.as_ptr() as *const _,
                            message.as_ptr() as *const _,
                        )
                    },
                    Level::Debug => unsafe {
                        mw_log_debug_additional(
                            info.buf.as_ptr() as *const _,
                            message.as_ptr() as *const _,
                        )
                    },
                    Level::Trace => unsafe {
                        mw_log_verbose_additional(
                            info.buf.as_ptr() as *const _,
                            message.as_ptr() as *const _,
                        )
                    },
                }
            } else {
                match record.level() {
                    Level::Fatal => unsafe { mw_log_fatal(message.as_ptr() as *const _) },
                    Level::Error => unsafe { mw_log_error(message.as_ptr() as *const _) },
                    Level::Warn => unsafe { mw_log_warn(message.as_ptr() as *const _) },
                    Level::Info => unsafe { mw_log_info(message.as_ptr() as *const _) },
                    Level::Debug => unsafe { mw_log_debug(message.as_ptr() as *const _) },
                    Level::Trace => unsafe { mw_log_verbose(message.as_ptr() as *const _) },
                }
            }
        }
    }

    fn flush(&self) {}
}
