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
cc_test(
    name = "flexible_circular_allocator_test",
    srcs = [
        "flexible_circular_allocator_test.cpp",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    visibility = [
        # "@ddad//score/analysis/tracing/common/flexible_circular_allocator:__subpackages__",
    ],
    deps = [
        "//score/analysis/tracing/common/flexible_circular_allocator",
        "//score/mw/log",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "flexible_circular_allocator_factory_test",
    srcs = [
        "flexible_circular_allocator_factory_test.cpp",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    visibility = [
        # "@ddad//score/analysis/tracing/common/flexible_circular_allocator:__subpackages__",
    ],
    deps = [
        "//score/analysis/tracing/common/flexible_circular_allocator:flexible_circular_allocator_factory",
        "//score/mw/log",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "lockless_flexible_circular_allocator_factory_test",
    srcs = [
        "lockless_flexible_circular_allocator_factory_test.cpp",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    visibility = [
        # "@ddad//score/analysis/tracing/common/flexible_circular_allocator:__subpackages__",
    ],
    deps = [
        "//score/analysis/tracing/common/flexible_circular_allocator:lockless_flexible_circular_allocator_factory",
        "//score/mw/log",
        "@googletest//:gtest_main",
    ],
)
