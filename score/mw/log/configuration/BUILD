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
    name = "configuration",
    srcs = [
        "configuration.cpp",
    ],
    hdrs = [
        "configuration.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/mw/log:__subpackages__",
        # "@ddad//score/mw/log:__subpackages__",
    ],
    deps = [
        "//score/mw/log:shared_types",
        "//score/mw/log/detail:logging_identifier",
    ],
)

cc_library(
    name = "configuration_interface",
    srcs = [
        "iconfiguration_file_discoverer.cpp",
        "itarget_config_reader.cpp",
    ],
    hdrs = [
        "iconfiguration_file_discoverer.h",
        "itarget_config_reader.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/mw/log/detail:__pkg__",
        # "@ddad//score/mw/log/configuration:__pkg__",
    ],
    deps = [
        ":configuration",
        "//score/result",
    ],
)

cc_library(
    name = "configuration_parser",
    srcs = [
        "configuration_file_discoverer.cpp",
        "target_config_reader.cpp",
    ],
    hdrs = [
        "configuration_file_discoverer.h",
        "target_config_reader.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/mw/log/detail:__pkg__",
        # "@ddad//score/mw/log:__subpackages__",
    ],
    deps = [
        ":configuration",
        ":configuration_interface",
        "//score/json:json_parser",
        "//score/memory:split_string_view",
        "//score/os:stdlib",
        "//score/os:unistd",
        "//score/os/utils:path",
        "//score/mw/log/detail:initialization_reporter",
        "//score/mw/log/detail:types_and_errors",
    ],
)

cc_library(
    name = "target_config_reader_mock",
    testonly = True,
    srcs = ["target_config_reader_mock.cpp"],
    hdrs = ["target_config_reader_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//score/mw/log:__pkg__",
        "//score/mw/log/detail:__pkg__",
        "//score/mw/log/detail/data_router:__pkg__",
        "//score/mw/log/detail/slog:__pkg__",
        # "@ddad//score/mw/log:__subpackages__",
    ],
    deps = [
        ":configuration_interface",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "configuration_file_discoverer_mock",
    testonly = True,
    srcs = ["configuration_file_discoverer_mock.cpp"],
    hdrs = ["configuration_file_discoverer_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//score/mw/log/detail:__pkg__",
        # "@ddad//score/mw/log:__subpackages__",
    ],
    deps = [
        ":configuration_interface",
        "@googletest//:gtest",
    ],
)

filegroup(
    name = "tests_config_files",
    srcs = [
        "test/data/app_config.json",
        "test/data/ecu_config.json",
        "test/data/error-json-structure.json",
        "test/data/invalid_app_config.json",
        "test/data/syntax_error.json",
        "test/data/wrong-context-config-value.json",
        "test/data/wrong-loglevel-value.json",
    ],
    visibility = [
        "//score/mw/log:__pkg__",
        # "@ddad//score/mw/log:__subpackages__",
    ],
)

cc_test(
    name = "unit_test",
    srcs = [
        "configuration_file_discoverer_test.cpp",
        "configuration_test.cpp",
        "target_config_reader_test.cpp",
    ],
    data = [
        ":tests_config_files",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    visibility = [
        # "@ddad//score/mw/log:__subpackages__",
    ],
    deps = [
        ":configuration_file_discoverer_mock",
        ":configuration_parser",
        ":target_config_reader_mock",
        "//score/os/mocklib:fcntl_mock",
        "//score/os/mocklib:stdlib_mock",
        "//score/os/mocklib:unistd_mock",
        "//score/os/utils/mocklib:path_mock",
        "//score/mw/log/test/console_logging_environment",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "nvconfig",
    srcs = [
        "nvconfig.cpp",
    ],
    hdrs = [
        "nvconfig.h",
        "nvmsgdescriptor.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    implementation_deps = [
        "//score/json:json_parser",
    ],
    tags = ["FFI"],
    visibility = [
        "//score/mw/log:__subpackages__",
        # "@ddad//score/mw/log:__subpackages__",
    ],
    deps = [
        "//score/mw/log:shared_types",
        "//score/mw/log/detail:logging_identifier",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_test(
    name = "nvconfig_test",
    srcs = [
        "nvconfig_test.cpp",
    ],
    data = [
        "test/data/empty-class-id.json",
        "test/data/empty-json-class-id.json",
        "test/data/error-content-1-json-class-id.json",
        "test/data/error-content-2-json-class-id.json",
        "test/data/error-content-3-json-class-id.json",
        "test/data/error-content-wrong-id-value.json",
        "test/data/error-parse-1-json-class-id.json",
        "test/data/error-parse-empty-json-object.json",
        "test/data/second-test-class-id.json",
        "test/data/test-class-id.json",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    visibility = [
        # "@ddad//score/mw/log:__subpackages__",
    ],
    deps = [
        ":nvconfig",
        "@googletest//:gtest_main",
    ],
)

filegroup(
    name = "nvconfig_test_config_files",
    srcs = [
        "test/data/empty-class-id.json",
        "test/data/empty-json-class-id.json",
        "test/data/error-content-1-json-class-id.json",
        "test/data/error-content-2-json-class-id.json",
        "test/data/error-content-3-json-class-id.json",
        "test/data/error-content-wrong-id-value.json",
        "test/data/error-parse-1-json-class-id.json",
        "test/data/error-parse-empty-json-object.json",
        "test/data/second-test-class-id.json",
        "test/data/test-class-id.json",
    ],
    visibility = [
        "//score/mw/log:__pkg__",
        # "@ddad//score/mw/log:__subpackages__",
    ],
)

test_suite(
    name = "unit_tests",
    tests = [
        ":nvconfig_test",
        ":unit_test",
    ],
    visibility = [
        "//score/mw/log:__pkg__",
        # "@ddad//score/mw/log:__subpackages__",
    ],
)
