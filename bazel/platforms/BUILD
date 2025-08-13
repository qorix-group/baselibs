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

constraint_setting(name = "tgt")

constraint_value(
    name = "xpad",
    constraint_setting = ":tgt",
)

constraint_value(
    name = "host",
    constraint_setting = ":tgt",
)

constraint_setting(name = "cpu")

constraint_value(
    name = "aurix",
    constraint_setting = ":cpu",
)

constraint_value(
    name = "dnv",
    constraint_setting = ":cpu",
)

constraint_setting(name = "compiler")

constraint_value(
    name = "gcc",
    constraint_setting = ":compiler",
)

xpad_constraints = [
    ":xpad",
    "//bazel/platforms/libraries:systemd",
]

xpad_dnv_constraints = xpad_constraints + [
    ":dnv",
]

linux_constraints = [
    "@platforms//cpu:x86_64",
    "@platforms//os:linux",
]

osx_constraints = [
    "@platforms//cpu:x86_64",
    "@platforms//os:osx",
]

windows_constraints = [
    "@platforms//cpu:x86_64",
    "@platforms//os:windows",
]

wasm32_constraints = [
    "@platforms//cpu:wasm32",
    "@platforms//os:none",
]

platform(
    name = "linux_host",
    constraint_values = linux_constraints,
)

platform(
    name = "windows_host",
    constraint_values = windows_constraints,
)

platform(
    name = "wasm32",
    constraint_values = wasm32_constraints,
)

config_setting(
    name = "config_linux_host",
    constraint_values = linux_constraints,
)

config_setting(
    name = "config_windows_host",
    constraint_values = windows_constraints,
)

config_setting(
    name = "config_macos_host",
    constraint_values = osx_constraints,
)

config_setting(
    name = "config_xpad_dnv",
    constraint_values = xpad_dnv_constraints,
)

config_setting(
    name = "config_xpad",
    constraint_values = xpad_constraints,
)

host_constraints = [
    ":host",
]

host_gcc8_constraints = host_constraints + [
    ":gcc",
    "@platforms//cpu:x86_64",
    "@platforms//os:linux",
]

platform(
    name = "host_gcc8",
    constraint_values = host_gcc8_constraints,
)

config_setting(
    name = "config_host_gcc8",
    constraint_values = host_gcc8_constraints,
)

host_gcc9_constraints = host_constraints + [
    "@bazel_tools//tools/cpp:gcc",
    "@platforms//cpu:x86_64",
    "@platforms//os:linux",
    "//bazel/platforms/libraries:systemd",
]

platform(
    name = "host_gcc9",
    constraint_values = host_gcc9_constraints,
)

config_setting(
    name = "config_host_gcc9",
    constraint_values = host_gcc9_constraints,
)

platform(
    name = "host_platform",
    constraint_values = [
        "@platforms//cpu:x86_64",
    ],
)

platform(
    name = "host_linux_x86_64_gcc",
    constraint_values = [
        "@bazel_tools//tools/cpp:gcc",
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
    ],
)

platform(
    name = "host_linux_x86_64_gcc_container_platform",
    constraint_values = [
        "@io_bazel_rules_docker//platforms:run_in_container",
    ],
    parents = [
        ":host_linux_x86_64_gcc",
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

platform(
    name = "x86_64-qnx7",
    constraint_values = [
        "//bazel/platforms/qnx_version:qnx7",
    ],
    parents = [
        "@swf_platforms//:x86_64-qnx-adaptive",
    ],
)

platform(
    name = "arm64-qnx7-hardware",
    constraint_values = [
        "//bazel/platforms/environment:hardware",
        "//bazel/platforms/qnx_version:qnx7",
    ],
    parents = [
        "@swf_platforms//:arm64-qnx-adaptive",
    ],
)

platform(
    name = "arm64-qnx8-hardware",
    constraint_values = [
        "//bazel/platforms/environment:hardware",
        "//bazel/platforms/qnx_version:qnx8",
    ],
    parents = [
        "@swf_platforms//:arm64-qnx-adaptive",
    ],
)

platform(
    name = "arm64-qnx-hardware",
    constraint_values = [
        "//bazel/platforms/environment:hardware",
        "//bazel/platforms/qnx_version:qnx7",
    ],
    parents = [
        "@swf_platforms//:arm64-qnx-adaptive",
    ],
)

platform(
    name = "arm64-qnx7-virtual",
    constraint_values = [
        "//bazel/platforms/environment:virtual",
        "//bazel/platforms/qnx_version:qnx7",
    ],
    parents = [
        "@swf_platforms//:arm64-qnx-adaptive",
    ],
)

platform(
    name = "arm64-qnx8-virtual",
    constraint_values = [
        "//bazel/platforms/environment:virtual",
        "//bazel/platforms/qnx_version:qnx8",
    ],
    parents = [
        "@swf_platforms//:arm64-qnx-adaptive",
    ],
)

platform(
    name = "arm64-qnx-virtual",
    constraint_values = [
        "//bazel/platforms/environment:virtual",
        "//bazel/platforms/qnx_version:qnx7",
    ],
    parents = [
        "@swf_platforms//:arm64-qnx-adaptive",
    ],
)

alias(
    name = "hardware",
    actual = "//bazel/platforms/environment:hardware",
)

alias(
    name = "virtual",
    actual = "//bazel/platforms/environment:virtual",
)
