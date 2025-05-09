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
load("@score-baselibs//score/language/safecpp/safe_math:compiler_features.bzl", "COMPILER_FEATURES")

cc_library(
    name = "safe_math",
    srcs = ["safe_math.cpp"],
    hdrs = ["safe_math.h"],
    features = COMPILER_FEATURES,
    tags = ["FFI"],
    visibility = ["//visibility:public"],
    deps = [
        ":error",
        "@score-baselibs//score/language/safecpp/safe_math/details/absolute",
        "@score-baselibs//score/language/safecpp/safe_math/details/addition_subtraction",
        "@score-baselibs//score/language/safecpp/safe_math/details/cast",
        "@score-baselibs//score/language/safecpp/safe_math/details/comparison",
        "@score-baselibs//score/language/safecpp/safe_math/details/division",
        "@score-baselibs//score/language/safecpp/safe_math/details/multiplication",
        "@score-baselibs//score/language/safecpp/safe_math/details/negate",
        "@score-baselibs//score/language/safecpp/safe_math/details/type_traits",
        "@score-baselibs//score/result",
    ],
)

cc_library(
    name = "error",
    srcs = ["error.cpp"],
    hdrs = ["error.h"],
    features = COMPILER_FEATURES,
    tags = ["FFI"],
    visibility = ["@score-baselibs//score/language/safecpp/safe_math:__subpackages__"],
    deps = ["@score-baselibs//score/result"],
)

cc_gtest_unit_test(
    name = "error_test",
    srcs = ["error_test.cpp"],
    features = COMPILER_FEATURES,
    visibility = [
        # "@ddad@score-baselibs//score/language/safecpp/safe_math:__pkg__",
    ],
    deps = [":error"],
)
