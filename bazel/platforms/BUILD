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
package(default_visibility = ["//visibility:public"])

constraint_setting(name = "cpu")

constraint_setting(name = "tgt")

constraint_value(
    name = "dnv",
    constraint_setting = ":cpu",
)

constraint_value(
    name = "xpad",
    constraint_setting = ":tgt",
)

xpad_constraints = [
    ":xpad",
    "//bazel/platforms/libraries:systemd",
]

xpad_dnv_constraints = xpad_constraints + [
    ":dnv",
]

config_setting(
    name = "config_xpad_dnv",
    constraint_values = xpad_dnv_constraints,
)

platform(
    name = "arm64-qnx-hardware",
    constraint_values = [
        "//bazel/platforms/environment:hardware",
    ],
    parents = [
        "@swf_platforms//:arm64-qnx-adaptive",
    ],
)

platform(
    name = "arm64-qnx-virtual",
    constraint_values = [
        "//bazel/platforms/environment:virtual",
    ],
    parents = [
        "@swf_platforms//:arm64-qnx-adaptive",
    ],
)

platform(
    name = "x86_64-linux-clang",
    constraint_values = [
        "@bazel_tools//tools/cpp:clang",
    ],
    parents = [
        "@swf_platforms//:x86_64-linux",
    ],
)

platform(
    name = "x86_64-linux-gcc",
    constraint_values = [
        "@bazel_tools//tools/cpp:gcc",
    ],
    parents = [
        "@swf_platforms//:x86_64-linux",
    ],
)

platform(
    name = "host_linux_x86_64_clang",
    constraint_values = [
        "@bazel_tools//tools/cpp:clang",
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
    ],
)

platform(
    name = "host_linux_x86_64_clang_container_platform",
    constraint_values = [
        "@io_bazel_rules_docker//platforms:run_in_container",
    ],
    parents = [
        ":host_linux_x86_64_clang",
    ],
)
