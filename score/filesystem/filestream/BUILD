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
    name = "filestream",
    srcs = [
        "file_factory.cpp",
        "i_file_factory.cpp",
    ],
    hdrs = [
        "file_factory.h",
        "i_file_factory.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score-baselibs//score/filesystem:__subpackages__",
        "@score-baselibs//score/json:__subpackages__",
        # "@ddad@score-baselibs//score/filesystem/filestream:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/filesystem:error",
        "@score-baselibs//score/filesystem:path",
        "@score-baselibs//score/os:object_seam",
        "@score-baselibs//score/result",
    ],
)

cc_library(
    name = "fake",
    testonly = True,
    srcs = [
        "bad_string_stream_collection.cpp",
        "file_factory_fake.cpp",
        "i_string_stream_collection.cpp",
        "simple_string_stream_collection.cpp",
    ],
    hdrs = [
        "bad_string_stream_collection.h",
        "file_factory_fake.h",
        "file_factory_mock.h",
        "i_string_stream_collection.h",
        "simple_string_stream_collection.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["@score-baselibs//score/filesystem:__subpackages__"],
    deps = [
        ":filestream",
        "@googletest//:gtest",
    ],
)

cc_test(
    name = "unit_test",
    srcs = [
        "file_factory_fake_test.cpp",
        "file_factory_test.cpp",
        "i_file_factory_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    visibility = [
        "@score-baselibs//score/filesystem:__pkg__",
        # "@ddad@score-baselibs//score/filesystem:__pkg__",
    ],
    deps = [
        ":fake",
        ":filestream",
        "@googletest//:gtest_main",
    ],
)
