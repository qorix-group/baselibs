# *******************************************************************************
# Copyright (c) 2025 Contributors to the Eclipse Foundation
#
# See the NOTICE file(s) distributed with this work for additional
# information regarding copyright ownership.
#
# This program and the accompanying materials are made available under the
# terms of the Apache License Version 2.0 which is available at https://www.apache.org/licenses/LICENSE-2.0
#
# SPDX-License-Identifier: Apache-2.0
# *******************************************************************************

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_test(
    name = "vajson_unit_tests",
    srcs = [
        "ut_depth_counter.cpp",
        "ut_number.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + ["aborts_upon_exception"],
    local_defines = ["VAJSON"],
    tags = ["unit"],
    visibility = [
        "@score_baselibs//score/json:__subpackages__",
    ],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/json/internal/parser/vajson:vajson_parser",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_test_suite",
    cc_unit_tests = [
        ":vajson_parser_test",
        ":number_parser_test",
    ],
    visibility = [
        "@score_baselibs//score/json/internal/parser:__pkg__",
    ],
)
