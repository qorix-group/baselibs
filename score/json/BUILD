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

load("@bazel_skylib//rules:common_settings.bzl", "string_flag")

COMMON_FEATURES = [
    "treat_warnings_as_errors",
    "strict_warnings",
    "additional_warnings",
]

string_flag(
    name = "base_library",
    build_setting_default = "vajson",
    values = [
        "vajson",
        "nlohmann",
    ],
)

config_setting(
    name = "base_library_vajson",
    flag_values = {":base_library": "vajson"},
    visibility = ["@score-baselibs//score/json/internal/parser:__subpackages__"],
)

config_setting(
    name = "base_library_nlohmann",
    flag_values = {":base_library": "nlohmann"},
    visibility = ["@score-baselibs//score/json/internal/parser:__subpackages__"],
)

cc_library(
    name = "interface",
    srcs = [
        "i_json_parser.cpp",
        "i_json_writer.cpp",
    ],
    hdrs = [
        "i_json_parser.h",
        "i_json_writer.h",
        "json_parser.h",
        "json_writer.h",
    ],
    features = COMMON_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
        # "@ddad@score-baselibs//score/json:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/filesystem/filestream",
        "@score-baselibs//score/json/internal/model",
        "@score-baselibs//score/result",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "json_parser",
    srcs = [
        "json_parser.cpp",
        "json_writer.cpp",
    ],
    features = COMMON_FEATURES,
    local_defines = select({
        ":base_library_nlohmann": ["NLOHMANN"],
        ":base_library_vajson": ["VAJSON"],
    }),
    tags = ["FUSA"],
    visibility = [
        "//visibility:public",
        # "@ddad@score-baselibs//score/json:__subpackages__",
    ],
    deps = [
        ":interface",
        "@score-baselibs//score/json/internal/parser",
        "@score-baselibs//score/json/internal/writer/json_serialize",
    ],
)

cc_library(
    name = "json_serializer",
    srcs = ["json_serializer.cpp"],
    hdrs = ["json_serializer.h"],
    features = COMMON_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
        # "@ddad@score-baselibs//score/json:__subpackages__",
    ],
    deps = [
        ":json_parser",
        "@score-baselibs//score/static_reflection_with_serialization/visitor",
        "@score-baselibs//score/result",
        "@score-baselibs//score/language/futurecpp",
    ],
)

alias(
    name = "json",
    actual = ":interface",
    visibility = [
        "@score-baselibs//score/json:__subpackages__",
        # "@ddad@score-baselibs//score/json:__subpackages__",
    ],
)

alias(
    name = "json_impl",
    actual = ":json_parser",
    visibility = [
        "@score-baselibs//score/json:__subpackages__",
        # "@ddad@score-baselibs//score/json:__subpackages__",
    ],
)

cc_library(
    name = "mock",
    testonly = True,
    hdrs = [
        "i_json_parser_mock.h",
        "i_json_writer_mock.h",
    ],
    features = COMMON_FEATURES,
    visibility = [
        "@score-baselibs//score/json:__subpackages__",
        # "@ddad@score-baselibs//score/json:__subpackages__",
    ],
    deps = [
        ":interface",
        "@googletest//:gtest",
    ],
)

cc_test(
    name = "json_test",
    srcs = [
        "json_test.cpp",
    ],
    features = COMMON_FEATURES + ["aborts_upon_exception"],
    tags = ["unit"],
    visibility = [
        "@score-baselibs//score/json:__pkg__",
        # "@ddad@score-baselibs//score/json:__pkg__",
    ],
    deps = [
        ":json_impl",
        "@googletest//:gtest_main",
    ],
)

test_suite(
    name = "unit_tests",
    tests = [
        ":json_test",
        ":json_writer_unit_test",  # workaround to include coverage for json_writer
        "@score-baselibs//score/json/internal/model:unit_test",
        "@score-baselibs//score/json/internal/parser:unit_tests",
        "@score-baselibs//score/json/internal/writer/json_serialize:json_serialize_unit_test",  # workaround to include coverage for json_serialize
    ],
    visibility = ["//visibility:public"],
)

# This rule was moved from json/internal/writer/BUILD file due to incorrect functionality of coverage tool.
# Was found a problem when unit_test_qnx didn't run tests from json_writer_test.cpp and json_serialize_test.cpp.
# Solution was move json_writer_test.cpp file to the same directory with json_writer.cpp.
# For json_serialize its BUILD file was modified according to other tests for json.
cc_test(
    name = "json_writer_unit_test",
    srcs = [
        "json_writer_test.cpp",
    ],
    features = COMMON_FEATURES + [
        "test_warnings",
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    visibility = [
        "@score-baselibs//score/json:__pkg__",
        # "@ddad@score-baselibs//score/json:__pkg__",
    ],
    deps = [
        "@score-baselibs//score/filesystem:mock",
        "@score-baselibs//score/json:json_impl",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "json_serializer_test",
    srcs = [
        "json_serializer_test.cpp",
    ],
    features = COMMON_FEATURES,
    tags = ["unit"],
    visibility = [
        # "@ddad@score-baselibs//score/json:__pkg__",
    ],
    deps = [
        ":json_serializer",
        "@googletest//:gtest_main",
    ],
)
