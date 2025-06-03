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
load("@score-baselibs//bazel:unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")

FEAT_COMPILER_WARNINGS_AS_ERRORS = [
    "additional_warnings",
    "strict_warnings",
    "treat_warnings_as_errors",
]

cc_test(
    name = "generic_trace_api_stub_test",
    srcs = [
        "generic_trace_api_stub_test.cpp",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    tags = [
        "manual",
        "unit",
    ],
    visibility = [
        # "@ddad@score-baselibs//score/analysis/tracing/library:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api:generic_trace_api_stub",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/mocks:trace_library_mock",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "error_code_test",
    srcs = [
        "error_code_test.cpp",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    tags = [
        "unit",
    ],
    visibility = [
        # "@ddad@score-baselibs//score/analysis/tracing:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
        "@googletest//:gtest_main",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":generic_trace_api_stub_test",
        ":error_code_test",
    ],
    visibility = [
        "//visibility:public",
    ],
)
