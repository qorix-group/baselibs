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
 use cc::Build;
 use std::env;
 use std::fs;
 use std::path::Path;

 fn main() {
    println!("cargo:rerun-if-changed=build.rs");

    let out_dir = env::var("OUT_DIR").unwrap();
    // Write OUT_DIR in a file that mw_log can read
    let marker_path = Path::new(&out_dir).join("logging_identifier_outdir.txt");
    fs::write(&marker_path, &out_dir).unwrap();

    let mut base = Build::new();
    base.cpp(true)
        .warnings(true)
        .flag_if_supported("-Wall")
        .flag_if_supported("-Wextra")
        .flag_if_supported("-Werror");

    base
        .include("../../../..")
        .include("../../../static_reflection_with_serialization/visitor/include")
        .file("logging_identifier.cpp")
        .file("logging_identifier.h")
        .compile("logging_identifier");


 }