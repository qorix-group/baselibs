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

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "vajson_parser",
    srcs = ["vajson_parser.cpp"],
    hdrs = ["vajson_parser.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = select({
        "@score_baselibs//score/json:base_library_nlohmann": ["@platforms//:incompatible"],
        "@score_baselibs//score/json:base_library_vajson": [],
    }),
    visibility = ["@score_baselibs//score/json:__subpackages__"],
    deps = [
        "@score_baselibs//score/json:parser_interface",
        "@score_baselibs//score/json/internal/model",
        "@score_baselibs//score/json/internal/parser/vajson/vajson_impl",
        "@score_baselibs//score/result",
    ],
)

cc_test(
    name = "vajson_parser_test",
    srcs = ["vajson_parser_test.cpp"],
    features = COMPILER_WARNING_FEATURES + ["aborts_upon_exception"],
    local_defines = ["VAJSON"],
    tags = ["unit"],
    visibility = [
        "@score_baselibs//score/json:__subpackages__",
    ],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/json/internal/parser:parser_test_suite",
        "@score_baselibs//score/json/internal/parser/vajson:vajson_parser",
    ],
)

cc_test(
    name = "number_parser_test",
    srcs = ["number_parser_test.cpp"],
    features = COMPILER_WARNING_FEATURES + ["aborts_upon_exception"],
    local_defines = ["VAJSON"],
    tags = ["unit"],
    visibility = [
        "@score_baselibs//score/json:__subpackages__",
    ],
    deps = [
        ":vajson_parser",
        "@googletest//:gtest_main",
        "@score_baselibs//score/json",
        "@score_baselibs//score/json/internal/parser:number_parser_test_suite",
        "@score_baselibs//score/language/safecpp/coverage_termination_handler",
    ],
)

cc_test(
    name = "vajson_impl_unit_tests",
    srcs = [
        "unit_test/ct_vajson_v2_parsers.cpp",
        "unit_test/parser/ut_array.cpp",
        "unit_test/parser/ut_binary.cpp",
        "unit_test/parser/ut_bool.cpp",
        "unit_test/parser/ut_invalid.cpp",
        "unit_test/parser/ut_key.cpp",
        "unit_test/parser/ut_level_validator.cpp",
        "unit_test/parser/ut_null.cpp",
        "unit_test/parser/ut_number.cpp",
        "unit_test/parser/ut_object.cpp",
        "unit_test/parser/ut_parser.cpp",
        "unit_test/parser/ut_single_array_parser.cpp",
        "unit_test/parser/ut_single_object_parser.cpp",
        "unit_test/parser/ut_string.cpp",
        "unit_test/parser/ut_valid.cpp",
        "unit_test/ut_depth_counter.cpp",
        "unit_test/ut_error_domain.cpp",
        "unit_test/ut_json_data.cpp",
        "unit_test/ut_json_ops.cpp",
        "unit_test/ut_number.cpp",
        "unit_test/ut_opt_char.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + ["aborts_upon_exception"],
    local_defines = ["VAJSON"],
    tags = ["unit"],
    visibility = [
        "@score_baselibs//score/json:__subpackages__",
    ],
    deps = [
        ":vajson_parser",
        "@googletest//:gtest_main",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_test_suite",
    cc_unit_tests = [
        ":vajson_parser_test",
        ":number_parser_test",
        ":vajson_impl_unit_tests",
    ],
    visibility = [
        "@score_baselibs//score/json/internal/parser:__pkg__",
    ],
)
