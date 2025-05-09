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
load("@score-baselibs//bazel:unit_tests.bzl", "cc_gtest_unit_test")

_COMPILER_WARNING_FEATURES = [
    "treat_warnings_as_errors",
    "additional_warnings",
    "strict_warnings",
]

cc_library(
    name = "error",
    srcs = [
        "error.cpp",
        "error_code.cpp",
        "error_domain.cpp",
    ],
    hdrs = [
        "error.h",
        "error_code.h",
        "error_domain.h",
    ],
    features = _COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/ara/log:__pkg__",
        "@score-baselibs//score/mw/log:__pkg__",
        # "@ddad//platform/aas/ara/log:__pkg__",
        # "@ddad@score-baselibs//score/result:__pkg__",
        # "@ddad@score-baselibs//score/mw/log:__pkg__",
    ],
)

cc_library(
    name = "result",
    srcs = ["result.cpp"],
    hdrs = ["result.h"],
    features = _COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
        # "@ddad//visibility:public",
    ],
    deps = [
        ":error",
        "@score-baselibs//score/result/details/expected",
        "@score-baselibs//score/result/details/expected:extensions",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "tests",
    srcs = [
        "error_test.cpp",
        "result_test.cpp",
    ],
    features = _COMPILER_WARNING_FEATURES,
    visibility = [
        # "@ddad@score-baselibs//score/result:__pkg__",
    ],
    deps = [
        ":error",
        ":result",
    ],
)
