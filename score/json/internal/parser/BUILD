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

alias(
    name = "parser",
    actual = select({
        "//score/json:base_library_nlohmann": "//score/json/internal/parser/nlohmann:nlohmann_parser",
        "//score/json:base_library_vajson": "//score/json/internal/parser/vajson:vajson_parser",
    }),
    visibility = ["//score/json:__pkg__"],
)

cc_library(
    name = "parser_test_suite",
    testonly = True,
    srcs = ["parsers_test_suite.cpp"],
    hdrs = ["parsers_test_suite.h"],
    features = COMMON_FEATURES + ["aborts_upon_exception"],
    tags = ["unit"],
    visibility = ["//score/json:__subpackages__"],
    deps = [
        "//score/language/safecpp/coverage_termination_handler",
        "//score/json:json_impl",
        "@googletest//:gtest",
    ],
)

cc_test(
    name = "json_parser_test",
    srcs = ["json_parser_test.cpp"],
    features = COMMON_FEATURES + ["aborts_upon_exception"],
    tags = ["unit"],
    visibility = ["//score/json:__pkg__"],
    deps = [
        "//score/language/safecpp/coverage_termination_handler",
        "//score/json:json_impl",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "number_parser_test_suite",
    testonly = True,
    srcs = ["number_parser_test_suite.cpp"],
    hdrs = ["number_parser_test_suite.h"],
    features = COMMON_FEATURES + ["aborts_upon_exception"],
    tags = ["unit"],
    visibility = ["//score/json:__subpackages__"],
    deps = [
        "//score/language/safecpp/coverage_termination_handler",
        "//score/json:json_impl",
        "//score/json/internal/parser/vajson:vajson_parser",
        "@googletest//:gtest_main",
    ],
)

test_suite(
    name = "unit_tests",
    tests = [
        ":json_parser_test",
        "//score/json/internal/parser/nlohmann:nlohmann_parser_test",
        "//score/json/internal/parser/nlohmann:number_parser_test",
        "//score/json/internal/parser/vajson:number_parser_test",
        "//score/json/internal/parser/vajson:vajson_parser_test",
    ],
    visibility = [
        "//score/json:__pkg__",
        # "@ddad//score/json:__pkg__",
    ],
)
