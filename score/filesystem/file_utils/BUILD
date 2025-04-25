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
    "strict_warnings",
    "additional_warnings",
]

cc_library(
    name = "file_utils",
    srcs = [
        "file_utils.cpp",
        "i_file_utils.cpp",
    ],
    hdrs = [
        "file_utils.h",
        "i_file_utils.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/filesystem:__subpackages__",
        # "@ddad//score/filesystem:__subpackages__",
    ],
    deps = [
        "//score/filesystem:error",
        "//score/filesystem:path",
        "//score/filesystem:standard_filesystem",
        "//score/os:fcntl",
        "//score/os:grp",
        "//score/os:object_seam",
        "//score/os:stat",
        "//score/os:stdlib",
        "//score/result",
    ],
)

cc_library(
    name = "fake",
    testonly = True,
    srcs = ["file_utils_fake.cpp"],
    hdrs = ["file_utils_fake.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["//score/filesystem:__subpackages__"],
    deps = [
        ":mock",
    ],
)

cc_library(
    name = "mock",
    testonly = True,
    srcs = [
    ],
    hdrs = [
        "file_utils_mock.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//score/filesystem:__subpackages__",
    ],
    deps = [
        ":file_utils",
        "@googletest//:gtest",
    ],
)

cc_test(
    name = "unit_test",
    srcs = [
        "file_utils_mock_test.cpp",
        "file_utils_test.cpp",
        "i_file_utils_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    visibility = [
        "//score/filesystem:__pkg__",
        # "@ddad//score/filesystem:__pkg__",
    ],
    deps = [
        ":file_utils",
        ":mock",
        "//score/filesystem:standard_filesystem_fake",
        "//score/os/mocklib:fcntl_mock",
        "//score/os/mocklib:grp_mock",
        "//score/os/mocklib:stat_mock",
        "//score/os/mocklib:unistd_mock",
        "@googletest//:gtest_main",
    ],
)
