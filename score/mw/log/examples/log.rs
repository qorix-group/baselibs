// Copyright 2025 Accenture.
//
// SPDX-License-Identifier: Apache-2.0

use std::{thread, time::Duration};

use log::{debug, error, info, trace, warn, LevelFilter};
use mw_log::init_logging;

fn main() {
    init_logging(LevelFilter::Debug);

    trace!("This is a trace log, and won't show at the current level");
    debug!("This is a debug log");
    info!("This is an info log");
    warn!("This is a warn log");
    error!("This is an error log");

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
