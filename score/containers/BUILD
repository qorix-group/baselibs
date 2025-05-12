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

cc_gtest_unit_test(
    name = "intrusive_list_test",
    srcs = ["intrusive_list_test.cpp"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    visibility = [
        # "@ddad@score-baselibs//score/containers:__subpackages__",
    ],
    deps = [":intrusivelist"],
)

cc_gtest_unit_test(
    name = "dynamic_array_test",
    srcs = ["dynamic_array_test.cpp"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    visibility = [
        # "@ddad@score-baselibs//score/containers:__subpackages__",
    ],
    deps = [
        ":dynamic_array",
        "@score-baselibs//score/memory/shared:fake_memory_resources",
        "@score-baselibs//score/memory/shared:types",
        "@score-baselibs//score/mw/log",
    ],
)

cc_library(
    name = "dynamic_array",
    srcs = ["dynamic_array.cpp"],
    hdrs = ["dynamic_array.h"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
        # "@ddad@score-baselibs//score/containers:__subpackages__",
    ],
    deps = ["@score-baselibs//score/language/futurecpp"],
)

cc_library(
    name = "intrusivelist",
    srcs = ["intrusive_list.cpp"],
    hdrs = ["intrusive_list.h"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
        # "@ddad@score-baselibs//score/containers:__subpackages__",
    ],
    deps = ["@score-baselibs//score/language/futurecpp"],
)
