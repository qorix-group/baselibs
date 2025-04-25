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
    name = "vajson_parser",
    srcs = ["vajson_parser.cpp"],
    hdrs = ["vajson_parser.h"],
    features = COMMON_FEATURES,
    tags = ["FFI"],
    target_compatible_with = select({
        "//score/json:base_library_nlohmann": ["@platforms//:incompatible"],
        "//score/json:base_library_vajson": [],
    }),
    visibility = ["//score/json:__subpackages__"],
    deps = [
        "//score/json/internal/model",
        "//score/result",
        # "@ddad//ecu/xpad/ara/vector:vajson",
    ],
)

cc_test(
    name = "vajson_parser_test",
    srcs = ["vajson_parser_test.cpp"],
    features = COMMON_FEATURES + ["aborts_upon_exception"],
    local_defines = ["VAJSON"],
    tags = ["unit"],
    visibility = ["//score/json:__subpackages__"],
    deps = [
        "//score/json/internal/parser:parser_test_suite",
        "//score/json/internal/parser/vajson:vajson_parser",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "number_parser_test",
    srcs = ["number_parser_test.cpp"],
    features = COMMON_FEATURES + ["aborts_upon_exception"],
    local_defines = ["VAJSON"],
    tags = ["unit"],
    visibility = ["//score/json:__subpackages__"],
    deps = [
        ":vajson_parser",
        "//score/language/safecpp/coverage_termination_handler",
        "//score/json:json_impl",
        "//score/json/internal/parser:number_parser_test_suite",
        "@googletest//:gtest_main",
    ],
)
