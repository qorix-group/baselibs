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
load("//bazel:unit_tests.bzl", "cc_gtest_unit_test")
load("//score/language/safecpp/safe_math:compiler_features.bzl", "COMPILER_FEATURES")

cc_library(
    name = "type_traits",
    srcs = ["type_traits.cpp"],
    hdrs = ["type_traits.h"],
    features = COMPILER_FEATURES,
    tags = ["FFI"],
    visibility = ["//score/language/safecpp/safe_math:__subpackages__"],
)

cc_gtest_unit_test(
    name = "unit_test",
    srcs = ["type_traits_test.cpp"],
    features = COMPILER_FEATURES,
    visibility = [
        # "@ddad//score/language/safecpp/safe_math:__pkg__",
    ],
    deps = [":type_traits"],
)
