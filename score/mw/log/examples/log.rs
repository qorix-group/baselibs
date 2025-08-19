// Copyright 2025 Accenture.
//
// SPDX-License-Identifier: Apache-2.0

use std::{thread, time::Duration};

use log::{debug, error, fatal, info, trace, warn};
use mw_log::MwLoggerBuilder;

fn main() {
    // Example 1: Set up the default logger using the builder's convenience method
    MwLoggerBuilder::new()
        .filter_max_level_from_cpp()
        .set_as_default_logger::<false, true, true>();

    // Example 2: Set up the default logger by building and calling set_default_logger separately
    // Uncomment the following lines to use this method instead:
    // let default_logger = MwLoggerBuilder::new()
    //     .filter_max_level_from_cpp()
    //     .build::<false, true, true>();
    // mw_log::set_default_logger(default_logger);

    let other_logger = MwLoggerBuilder::new()
        .filter_max_level_from_cpp()
        .context("TST2")
        .build::<false, false, false>();

    trace!("This is a trace log, and won't show at the current level");
    debug!("This is a debug log");
    info!(context: "TST1", "This is an info log with context");
    warn!(logger: other_logger, "This is a warn log with context through the specified logger");
    error!("This is an error log");
    fatal!("This is a fatal log");

    println!("\nRunning parallel logs\n");

    let mut threads = Vec::with_capacity(10);
    for msg in [
        "Short message",
        "This is a bit of a longer log message but still, it does not go over the buffer",
        "I hope I won't be mixed with other messages",
    ] {
        let msg = msg.to_owned();
        threads.push(thread::spawn(move || {
            // Messages will intermix in the output for e.g. Duration::ZERO
            log_in_a_loop(&msg, 10, Duration::from_nanos(100))
        }))
    }

    for th in threads {
        th.join().unwrap();
    }
}

fn log_in_a_loop(msg: &str, number_of_times: usize, timeout: Duration) {
    for _ in 0..number_of_times {
        info!("{msg}");
        thread::sleep(timeout);
    }
}
