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
    name = "json_builder",
    srcs = ["json_builder.cpp"],
    hdrs = ["json_builder.h"],
    tags = ["FFI"],
    deps = [
        "//score/json/internal/model",
        "//score/result",
        "@nlohmann_json//:json",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "nlohmann_parser",
    srcs = ["nlohmann_parser.cpp"],
    hdrs = ["nlohmann_parser.h"],
    features = COMMON_FEATURES,
    tags = ["FFI"],
    visibility = ["//score/json:__subpackages__"],
    deps = [
        ":json_builder",
        "//score/json/internal/model",
        "//score/result",
        "@nlohmann_json//:json",
    ],
)

cc_test(
    name = "nlohmann_parser_test",
    srcs = ["nlohmann_parser_test.cpp"],
    features = COMMON_FEATURES + ["aborts_upon_exception"],
    local_defines = ["NLOHMANN"],
    tags = ["unit"],
    visibility = [
        "//score/json:__subpackages__",
        # "@ddad//score/json:__subpackages__",
    ],
    deps = [
        "//score/json/internal/parser:parser_test_suite",
        "//score/json/internal/parser/nlohmann:nlohmann_parser",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "number_parser_test",
    srcs = ["number_parser_test.cpp"],
    features = COMMON_FEATURES + ["aborts_upon_exception"],
    local_defines = ["NLOHMANN"],
    tags = ["unit"],
    visibility = [
        "//score/json:__subpackages__",
        # "@ddad//score/json:__subpackages__",
    ],
    deps = [
        ":nlohmann_parser",
        "//score/language/safecpp/coverage_termination_handler",
        "//score/json:json_impl",
        "//score/json/internal/parser:number_parser_test_suite",
        "@googletest//:gtest_main",
    ],
)
