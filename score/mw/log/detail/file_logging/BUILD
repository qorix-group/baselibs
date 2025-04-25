# *******************************************************************************
# Copyright (c) 2025 Contributors to the Eclipse Foundation
#
# See the NOTICE file(s) distributed with this work for additional
# information regarding copyright ownership.
#
# This program and the accompanying materials are made available under the
# terms of the Apache License Version 2.0 which is available at
# https://www.apache.org/licenses/LICENSE-2.0
#
# SPDX-License-Identifier: Apache-2.0
# *******************************************************************************
COMPILER_WARNING_FEATURES = [
    "treat_warnings_as_errors",
    "additional_warnings",
    "strict_warnings",
]

cc_library(
    name = "file_recorder",
    srcs = [
        "dlt_message_builder.cpp",
        "dlt_message_builder.h",
        "dlt_message_builder_types.h",
        "file_recorder.cpp",
    ],
    hdrs = [
        "file_recorder.h",
        "svp_time.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/mw/log/detail:__pkg__",
        # "@ddad//score/mw/log/detail:__pkg__",
    ],
    deps = [
        ":file_output_backend",
        ":message_builder_interface",
        "//score/static_reflection_with_serialization/serialization",
        "//score/os/utils:high_resolution_steady_clock",
        "//score/mw/log:recorder",
        "//score/mw/log:shared_types",
        "//score/mw/log/configuration",
        "//score/mw/log/detail:backend_interface",
        "//score/mw/log/detail:dlt_content_formatting",
        "//score/mw/log/detail:statistics_reporter",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "text_recorder",
    srcs = [
        "text_message_builder.cpp",
        "text_message_builder.h",
        "text_recorder.cpp",
    ],
    hdrs = [
        "text_recorder.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/mw/log/detail:__pkg__",
        # "@ddad//score/mw/log/detail:__pkg__",
    ],
    deps = [
        ":file_output_backend",
        ":message_builder_interface",
        ":text_content_formatting",
        "//score/os/utils:high_resolution_steady_clock",
        "//score/mw/log:recorder",
        "//score/mw/log:shared_types",
        "//score/mw/log/configuration",
        "//score/mw/log/detail:backend_interface",
        "//score/mw/log/detail:dlt_argument_counter",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "text_content_formatting",
    srcs = [
        "text_format.cpp",
    ],
    hdrs = [
        "text_format.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    deps = [
        "//score/memory:string_literal",
        "//score/mw/log:shared_types",
        "//score/mw/log/detail:dlt_argument_counter",
        "//score/mw/log/detail:integer_representation",
        "//score/mw/log/detail:log_data_types",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "non_blocking_writer",
    srcs = ["non_blocking_writer.cpp"],
    hdrs = ["non_blocking_writer.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/mw/log/detail:__pkg__",
    ],
    deps = [
        "//score/os:unistd",
        "//score/mw/log/detail:types_and_errors",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "message_builder_interface",
    srcs = [
        "imessage_builder.cpp",
    ],
    hdrs = [
        "imessage_builder.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/mw/log:__pkg__",
    ],
    deps = [
        "//score/mw/log/detail:log_data_types",
    ],
)

cc_library(
    name = "file_output_backend",
    srcs = [
        "file_output_backend.cpp",
        "slot_drainer.cpp",
        "slot_drainer.h",
    ],
    hdrs = [
        "file_output_backend.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//score/mw/log:__pkg__"],
    deps = [
        ":message_builder_interface",
        ":non_blocking_writer",
        "//score/os:fcntl",
        "//score/mw/log/detail:backend_interface",
        "//score/mw/log/detail:circular_allocator",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_test(
    name = "unit_test",
    srcs = [
        "dlt_message_builder_test.cpp",
        "file_output_backend_test.cpp",
        "file_recorder_test.cpp",
        "slot_drainer_test.cpp",
        "text_format_test.cpp",
        "text_message_builder_test.cpp",
        "text_recorder_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    visibility = [
        # "@ddad//score/mw/log/detail/file_logging:__pkg__",
    ],
    deps = [
        ":file_output_backend",
        ":file_output_backend_mocks",
        ":file_recorder",
        ":text_recorder",
        "//score/os/mocklib:fcntl_mock",
        "//score/os/mocklib:unistd_mock",
        "//score/os/utils/mocklib:path_mock",
        "//score/mw/log/detail:backend_mock",
        "//score/mw/log/detail:circular_allocator",
        "//score/mw/log/test/console_logging_environment",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "file_output_backend_mocks",
    testonly = True,
    hdrs = ["mock/message_builder_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":non_blocking_writer",
        "@googletest//:gtest",
        "@score-baselibs//score/language/futurecpp",
    ],
)

test_suite(
    name = "unit_tests",
    tests = [
        ":non_blocking_writer_test",
        ":unit_test",
    ],
    visibility = ["//score/mw/log/detail:__pkg__"],
)

cc_test(
    name = "non_blocking_writer_test",
    srcs = ["non_blocking_writer_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    visibility = [
        # "@ddad//score/mw/log/detail/file_logging:__pkg__",
    ],
    deps = [
        ":non_blocking_writer",
        "//score/os/mocklib:unistd_mock",
        "//score/mw/log/test/console_logging_environment",
        "@googletest//:gtest_main",
    ],
)
