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
_COMPILER_FEATURES = [
    "treat_warnings_as_errors",
    "additional_warnings",
    "strict_warnings",
]

cc_library(
    name = "coverage_termination_handler",
    testonly = True,
    srcs = ["coverage_termination_handler.cpp"],
    hdrs = ["coverage_termination_handler.h"],
    features = _COMPILER_FEATURES,
    linkopts = ["-lgcov"],
    visibility = [
        "//visibility:public",
        # "@ddad//visibility:public",
    ],
    deps = ["@score-baselibs//score/language/futurecpp"],
    alwayslink = True,
)
