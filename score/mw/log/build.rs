// FFI custom build script.
fn main() {
    // Set LD_LIBRARY_PATH env var for cargo run, so that users do not need to manually export this
    // later on
    println!("cargo:rustc-env=LD_LIBRARY_PATH=bazel-bin/score/mw/log:bazel-bin/score/mw/log/configuration:bazel-bin/score/mw/log/detail:bazel-bin/score/mw/log/detail/file_logging:bazel-bin/score/result:bazel-bin/score/json:bazel-bin/score/json/internal/parser/nlohmann:bazel-bin/score/json/internal/writer/json_serialize:bazel-bin/score/json/internal/model:bazel-bin/score/filesystem:bazel-bin/score/os:bazel-bin/score/os/utils:bazel-bin/score/memory:bazel-bin/score/language/futurecpp");

    // The `rustc-link-lib` instruction tells `Cargo` to link the
    // given library using the compiler's `-l` flag. This is typically
    // used to link a native library using FFI.
    //
    // If you've already add a `#[link(name = "demo"]` in the `extern`
    // block, then you don't need to provide this.
    //
    println!("cargo:rustc-link-lib=dylib=libmw_log_ffi");
    println!("cargo:rustc-link-lib=dylib=frontend");
    println!("cargo:rustc-link-lib=dylib=json_serializer");
    println!("cargo:rustc-link-lib=dylib=recorder_factory");
    println!("cargo:rustc-link-lib=dylib=recorder");
    println!("cargo:rustc-link-lib=dylib=log_stream");
    println!("cargo:rustc-link-lib=dylib=thread_local_guard");
    println!("cargo:rustc-link-lib=dylib=logging_identifier");
    println!("cargo:rustc-link-lib=dylib=futurecpp");
    println!("cargo:rustc-link-lib=dylib=console_recorder_factory");
    println!("cargo:rustc-link-lib=dylib=empty_recorder");
    println!("cargo:rustc-link-lib=dylib=empty_recorder_factory");
    println!("cargo:rustc-link-lib=dylib=configuration_interface");
    println!("cargo:rustc-link-lib=dylib=unistd");
    println!("cargo:rustc-link-lib=dylib=backend_interface");
    println!("cargo:rustc-link-lib=dylib=text_recorder");
    println!("cargo:rustc-link-lib=dylib=text_content_formatting");
    println!("cargo:rustc-link-lib=dylib=dlt_argument_counter");
    println!("cargo:rustc-link-lib=dylib=log_data_types");
    println!("cargo:rustc-link-lib=dylib=configuration_parser");
    println!("cargo:rustc-link-lib=dylib=configuration");
    println!("cargo:rustc-link-lib=dylib=stdlib");
    println!("cargo:rustc-link-lib=dylib=fcntl");
    println!("cargo:rustc-link-lib=dylib=split_string_view");
    println!("cargo:rustc-link-lib=dylib=path");
    println!("cargo:rustc-link-lib=dylib=composite_recorder");
    println!("cargo:rustc-link-lib=dylib=json");
    println!("cargo:rustc-link-lib=dylib=file_output_backend");
    println!("cargo:rustc-link-lib=dylib=non_blocking_writer");
    println!("cargo:rustc-link-lib=dylib=stat");
    println!("cargo:rustc-link-lib=dylib=nlohmann_parser");
    println!("cargo:rustc-link-lib=dylib=message_builder_interface");
    println!("cargo:rustc-link-lib=dylib=shared_types");
    println!("cargo:rustc-link-lib=dylib=json_builder");
    println!("cargo:rustc-link-lib=dylib=json_serialize");
    println!("cargo:rustc-link-lib=dylib=string_comparison_adaptor");
    println!("cargo:rustc-link-lib=dylib=model");
    println!("cargo:rustc-link-lib=dylib=math");
    println!("cargo:rustc-link-lib=dylib=interface");
    println!("cargo:rustc-link-lib=dylib=os_path"); // modified to os_path due to identical name
    println!("cargo:rustc-link-lib=dylib=path");
    println!("cargo:rustc-link-lib=dylib=initialization_reporter");
    println!("cargo:rustc-link-lib=dylib=errno");
    println!("cargo:rustc-link-lib=dylib=error");
    println!("cargo:rustc-link-lib=dylib=file_recorder_factory");
    println!("cargo:rustc-link-lib=dylib=file_recorder");
    println!("cargo:rustc-link-lib=dylib=dlt_content_formatting");
    println!("cargo:rustc-link-lib=dylib=types_and_errors");
    println!("cargo:rustc-link-lib=dylib=libgen");

    // The `rustc-link-search` instruction tells Cargo to pass the `-L`
    // flag to the compiler to add a directory to the library search path.
    //
    // The optional `KIND` may be one of the values below:
    //
    // - `dependency`: Only search for transitive dependencies in this directory.
    // - `crate`: Only search for this crate's direct dependencies in this directory.
    // - `native`: Only search for native libraries in this directory.
    // - `framework`: Only search for macOS frameworks in this directory.
    // - `all`: Search for all library kinds in this directory. This is the default
    //          if KIND is not specified.
    //
    println!("cargo:rustc-link-search=native=bazel-bin/score/mw/log");
    println!("cargo:rustc-link-search=native=bazel-bin/score/mw/log/configuration");
    println!("cargo:rustc-link-search=native=bazel-bin/score/mw/log/detail");
    println!("cargo:rustc-link-search=native=bazel-bin/score/mw/log/detail/file_logging");
    println!("cargo:rustc-link-search=native=bazel-bin/score/result");
    println!("cargo:rustc-link-search=native=bazel-bin/score/json");
    println!("cargo:rustc-link-search=native=bazel-bin/score/json/internal/parser/nlohmann");
    println!("cargo:rustc-link-search=native=bazel-bin/score/json/internal/writer/json_serialize");
    println!("cargo:rustc-link-search=native=bazel-bin/score/json/internal/model");
    println!("cargo:rustc-link-search=native=bazel-bin/score/filesystem");
    println!("cargo:rustc-link-search=native=bazel-bin/score/os");
    println!("cargo:rustc-link-search=native=bazel-bin/score/os/utils");
    println!("cargo:rustc-link-search=native=bazel-bin/score/memory");
    println!("cargo:rustc-link-search=native=bazel-bin/score/language/futurecpp");

    // build.rs
    println!("cargo:rustc-link-lib=stdc++");

    // Rerun if the facade changed
    println!("cargo:rerun-if-changed=src/mw_log_ffi.cpp");

}
