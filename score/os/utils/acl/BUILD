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
cc_library(
    name = "acl",
    srcs = [
        "access_control_list.cpp",
        "i_access_control_list.cpp",
    ],
    hdrs = [
        "access_control_list.h",
        "i_access_control_list.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
        # "@ddad//visibility:public",
    ],
    deps = [
        "//score/os:acl",
        "//score/os:errno",
        "//score/result",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "mock",
    srcs = ["access_control_list_mock.cpp"],
    hdrs = [
        "access_control_list_mock.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    visibility = [
        "//visibility:public",
        # "@ddad//visibility:public",
    ],
    deps = [
        ":acl",
        "@googletest//:gtest",
    ],
)

cc_test(
    name = "unit_test",
    srcs = ["access_control_list_test.cpp"],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    tags = ["unit"],
    visibility = [
        "//score/os/utils/test:__pkg__",
        # "@ddad//score/os/utils:__subpackages__",
    ],
    deps = [
        ":acl",
        "//score/os/mocklib:acl_mock",
        "@googletest//:gtest_main",
    ],
)
