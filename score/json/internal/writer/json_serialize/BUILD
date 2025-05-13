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

COMMON_FEATURES = [
    "treat_warnings_as_errors",
    "strict_warnings",
    "additional_warnings",
]

cc_library(
    name = "json_serialize",
    srcs = [
        "json_serialize.cpp",
    ],
    hdrs = [
        "json_serialize.h",
    ],
    features = COMMON_FEATURES,
    tags = ["FFI"],
    visibility = ["@score-baselibs//score/json:__subpackages__"],
    deps = [
        "@score-baselibs//score/json/internal/model",
        "@score-baselibs//score/result",
    ],
)

cc_test(
    name = "json_serialize_unit_test",
    srcs = [
        "json_serialize_test.cpp",
    ],
    features = COMMON_FEATURES + ["aborts_upon_exception"],
    tags = ["unit"],
    visibility = [
        "@score-baselibs//score/json:__pkg__",
        # "@ddad@score-baselibs//score/json:__pkg__",
    ],
    deps = [
        ":json_serialize",
        "@googletest//:gtest_main",
    ],
)
