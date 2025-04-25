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
test_suite(
    name = "unit_tests",
    tests = [
        ":network_interface_test",
    ],
)

cc_test(
    name = "network_interface_test",
    srcs = [
        "network_interface_test.cpp",
    ],
    tags = ["unit"],
    deps = [
        "//score/os/linux/utils/mocklib:network_interface_mock",
        "//score/os/mocklib:nonposixwrapper_mock",
        "//score/os/mocklib:stat_mock",
        "@googletest//:gtest_main",
    ] + ["//score/os/linux/utils:network_interface"],
)
