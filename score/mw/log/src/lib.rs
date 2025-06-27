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

use crate::mw_log_ffi::*;

use core::ffi::c_char;
use core::fmt::{self, Write};
use log::{Level, LevelFilter, Log, Metadata, Record};
use std::ffi::CString;

const MSG_SIZE: usize = 512;
const INFO_SIZE: usize = 64;

pub struct MwLoggerBuilder {
    level: LevelFilter,
    context: Option<CString>,
}

impl MwLoggerBuilder {
    pub fn new() -> Self {
        Self {
            level: LevelFilter::Info,
            context: None,
        }
    }

    pub fn context(mut self, context: &str) -> Self {
        self.context = Some(CString::new(context).expect(
            "Failed to create CString:
             input contains null bytes",
        ));
        self
    }

    pub fn level(mut self, level: LevelFilter) -> Self {
        self.level = level;
        self
    }

    pub fn build<const SHOW_MODULE: bool, const SHOW_FILE: bool, const SHOW_LINE: bool>(
        &self,
    ) -> MwLogger<SHOW_MODULE, SHOW_FILE, SHOW_LINE> {
        let context_cstr = self
            .context
            .as_ref()
            .cloned()
            .unwrap_or_else(|| CString::new("DFLT").unwrap());
        let c_logger_ptr = unsafe { mw_log_create_logger(context_cstr.as_ptr() as *const _) };
        MwLogger {
            level: self.level,
            ptr: c_logger_ptr,
        }
    }
    pub fn set_as_default_logger<
        const SHOW_MODULE: bool,
        const SHOW_FILE: bool,
        const SHOW_LINE: bool,
    >(
        self,
    ) {
        let logger = self.build::<SHOW_MODULE, SHOW_FILE, SHOW_LINE>();
        crate::set_default_logger(logger);
    }
}

// --- Global logger backend configuration ---
pub fn set_default_logger<const SHOW_MODULE: bool, const SHOW_FILE: bool, const SHOW_LINE: bool>(
    logger: MwLogger<SHOW_MODULE, SHOW_FILE, SHOW_LINE>,
) {
    log::set_max_level(logger.level);
    log::set_boxed_logger(Box::new(logger))
        .expect("Failed to initialize MwLogger as default logger - logger may already be set");
}

pub fn init(level: LevelFilter) {
    log::set_max_level(level);
}
// --- End of global logger backend configuration ---

struct BufWriter<const BUF_SIZE: usize> {
    buf: [u8; BUF_SIZE],
    pos: usize,
}

impl<const BUF_SIZE: usize> BufWriter<BUF_SIZE> {
    fn new() -> Self {
        Self {
            buf: [0; BUF_SIZE],
            pos: 0,
        }
    }

    /// Returns a pointer to a null-terminated C string view of the buffer.
    /// Assumes the buffer is already null-terminated after the last write.
    fn as_c_str_ptr(&self) -> *const c_char {
        self.buf.as_ptr() as *const c_char
    }
}

impl<const BUF_SIZE: usize> Write for BufWriter<BUF_SIZE> {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        let bytes = s.as_bytes();
        let remaining = self.buf.len() - self.pos;
        if bytes.len() > remaining {
            return Err(fmt::Error);
        }

        self.buf[self.pos..self.pos + bytes.len()].copy_from_slice(bytes);
        self.pos += bytes.len();
        // Always null-terminate at min(pos, BUF_SIZE-1)
        let len = self.pos.min(self.buf.len() - 1);
        self.buf[len] = 0;
        Ok(())
    }
}

pub struct MwLogger<const SHOW_MODULE: bool, const SHOW_FILE: bool, const SHOW_LINE: bool> {
    level: LevelFilter,
    ptr: *const Logger,
}

// SAFETY: Logger pointer is only used for FFI calls and is never mutated from Rust.
// The underlying C++ logger is assumed to be thread-safe.
unsafe impl<const SHOW_MODULE: bool, const SHOW_FILE: bool, const SHOW_LINE: bool> Send
    for MwLogger<SHOW_MODULE, SHOW_FILE, SHOW_LINE>
{
}
unsafe impl<const SHOW_MODULE: bool, const SHOW_FILE: bool, const SHOW_LINE: bool> Sync
    for MwLogger<SHOW_MODULE, SHOW_FILE, SHOW_LINE>
{
}

impl<const SHOW_MODULE: bool, const SHOW_FILE: bool, const SHOW_LINE: bool>
    MwLogger<SHOW_MODULE, SHOW_FILE, SHOW_LINE>
{
    fn dispatch_with_info_and_context(
        &self,
        level: Level,
        info: &BufWriter<INFO_SIZE>,
        msg: &BufWriter<MSG_SIZE>,
        context: &CString,
    ) {
        match level {
            Level::Fatal => unsafe {
                mw_log_fatal_additional_context(
                    info.as_c_str_ptr(),
                    msg.as_c_str_ptr(),
                    context.as_ptr() as *const _,
                )
            },
            Level::Error => unsafe {
                mw_log_error_additional_context(
                    info.as_c_str_ptr(),
                    msg.as_c_str_ptr(),
                    context.as_ptr() as *const _,
                )
            },
            Level::Warn => unsafe {
                mw_log_warn_additional_context(
                    info.as_c_str_ptr(),
                    msg.as_c_str_ptr(),
                    context.as_ptr() as *const _,
                )
            },
            Level::Info => unsafe {
                mw_log_info_additional_context(
                    info.as_c_str_ptr(),
                    msg.as_c_str_ptr(),
                    context.as_ptr() as *const _,
                )
            },
            Level::Debug => unsafe {
                mw_log_debug_additional_context(
                    info.as_c_str_ptr(),
                    msg.as_c_str_ptr(),
                    context.as_ptr() as *const _,
                )
            },
            Level::Trace => unsafe {
                mw_log_verbose_additional_context(
                    info.as_c_str_ptr(),
                    msg.as_c_str_ptr(),
                    context.as_ptr() as *const _,
                )
            },
        }
    }

    fn dispatch_with_logger_info(
        &self,
        level: Level,
        info: &BufWriter<INFO_SIZE>,
        msg: &BufWriter<MSG_SIZE>,
    ) {
        unsafe {
            match level {
                Level::Fatal => mw_log_fatal_logger_additional(
                    self.ptr,
                    info.as_c_str_ptr(),
                    msg.as_c_str_ptr(),
                ),
                Level::Error => mw_log_error_logger_additional(
                    self.ptr,
                    info.as_c_str_ptr(),
                    msg.as_c_str_ptr(),
                ),
                Level::Warn => {
                    mw_log_warn_logger_additional(self.ptr, info.as_c_str_ptr(), msg.as_c_str_ptr())
                }
                Level::Info => {
                    mw_log_info_logger_additional(self.ptr, info.as_c_str_ptr(), msg.as_c_str_ptr())
                }
                Level::Debug => mw_log_debug_logger_additional(
                    self.ptr,
                    info.as_c_str_ptr(),
                    msg.as_c_str_ptr(),
                ),
                Level::Trace => mw_log_verbose_logger_additional(
                    self.ptr,
                    info.as_c_str_ptr(),
                    msg.as_c_str_ptr(),
                ),
            }
        }
    }

    fn dispatch_with_context(&self, level: Level, msg: &BufWriter<MSG_SIZE>, context: &CString) {
        unsafe {
            match level {
                Level::Fatal => {
                    mw_log_fatal_context(msg.as_c_str_ptr(), context.as_ptr() as *const _)
                }
                Level::Error => {
                    mw_log_error_context(msg.as_c_str_ptr(), context.as_ptr() as *const _)
                }
                Level::Warn => {
                    mw_log_warn_context(msg.as_c_str_ptr(), context.as_ptr() as *const _)
                }
                Level::Info => {
                    mw_log_info_context(msg.as_c_str_ptr(), context.as_ptr() as *const _)
                }
                Level::Debug => {
                    mw_log_debug_context(msg.as_c_str_ptr(), context.as_ptr() as *const _)
                }
                Level::Trace => {
                    mw_log_verbose_context(msg.as_c_str_ptr(), context.as_ptr() as *const _)
                }
            }
        }
    }

    fn dispatch_simple(&self, level: Level, msg: &BufWriter<MSG_SIZE>) {
        unsafe {
            match level {
                Level::Fatal => mw_log_fatal_logger(self.ptr, msg.as_c_str_ptr()),
                Level::Error => mw_log_error_logger(self.ptr, msg.as_c_str_ptr()),
                Level::Warn => mw_log_warn_logger(self.ptr, msg.as_c_str_ptr()),
                Level::Info => mw_log_info_logger(self.ptr, msg.as_c_str_ptr()),
                Level::Debug => mw_log_debug_logger(self.ptr, msg.as_c_str_ptr()),
                Level::Trace => mw_log_verbose_logger(self.ptr, msg.as_c_str_ptr()),
            }
        }
    }
}

impl<const SHOW_MODULE: bool, const SHOW_FILE: bool, const SHOW_LINE: bool> Log
    for MwLogger<SHOW_MODULE, SHOW_FILE, SHOW_LINE>
{
    fn enabled(&self, metadata: &Metadata) -> bool {
        metadata.level() <= self.level
    }

    fn log(&self, record: &Record) {
        if !self.enabled(record.metadata()) {
            return;
        }
        let mut msg_writer = BufWriter::<MSG_SIZE>::new();
        let _ = write!(msg_writer, "{}", record.args());

        if SHOW_FILE || SHOW_LINE || SHOW_MODULE {
            let mut info = BufWriter::<INFO_SIZE>::new();
            let _ = write!(info, "[");
            if SHOW_MODULE {
                if let Some(module) = record.module_path() {
                    let _ = write!(info, "{}:", module);
                }
            }
            if SHOW_FILE {
                if let Some(file) = record.file() {
                    let _ = write!(info, "{}:", file);
                }
            }
            if SHOW_LINE {
                if let Some(line) = record.line() {
                    let _ = write!(info, "{}", line);
                }
            }
            // Remove trailing ':' if present
            if info.pos > 1 && info.buf[info.pos - 1] == b':' {
                info.pos -= 1;
            }
            let _ = write!(info, "]");

            if let Some(context) = record.context_id() {
                let c_context = CString::new(context).unwrap();
                self.dispatch_with_info_and_context(record.level(), &info, &msg_writer, &c_context);
            } else {
                self.dispatch_with_logger_info(record.level(), &info, &msg_writer);
            }
        } else {
            if let Some(context) = record.context_id() {
                let c_context = CString::new(context).unwrap();
                self.dispatch_with_context(record.level(), &msg_writer, &c_context);
            } else {
                self.dispatch_simple(record.level(), &msg_writer);
            }
        }
    }

    fn flush(&self) {}
}

/// Get the max log level from C++ as a LevelFilter directly
pub fn max_level_filter_from_cpp() -> LevelFilter {
    match unsafe { mw_log_ffi::mw_get_max_log_level() } {
        0x00 => LevelFilter::Off,
        0x01 => LevelFilter::Fatal,
        0x02 => LevelFilter::Error,
        0x03 => LevelFilter::Warn,
        0x04 => LevelFilter::Info,
        0x05 => LevelFilter::Debug,
        0x06 => LevelFilter::Trace, // Verbose is Trace
        _ => LevelFilter::Info,     // fallback
    }
}

impl MwLoggerBuilder {
    pub fn filter_max_level_from_cpp(mut self) -> Self {
        self.level = max_level_filter_from_cpp();
        self
    }
}
