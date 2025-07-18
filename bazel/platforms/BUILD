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

constraint_setting(name = "ib_vip_diag_compiler")

constraint_value(
    name = "ib_vip_diag",
    constraint_setting = ":ib_vip_diag_compiler",
)

constraint_value(
    name = "xpad",
    constraint_setting = ":tgt",
)

constraint_value(
    name = "ib_vip",
    constraint_setting = ":tgt",
)

constraint_value(
    name = "host",
    constraint_setting = ":tgt",
)

constraint_value(
    name = "adcamlow",
    constraint_setting = ":tgt",
)

constraint_value(
    name = "cvc",
    constraint_setting = ":tgt",
)

constraint_value(
    name = "cvc2",
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

constraint_value(
    name = "eyeq5",
    constraint_setting = ":cpu",
)

constraint_setting(name = "compiler")

constraint_value(
    name = "gcc",
    constraint_setting = ":compiler",
)

constraint_value(
    name = "mingw",
    constraint_setting = ":compiler",
)

xpad_constraints = [
    ":xpad",
    "//bazel/platforms/libraries:systemd",
]

xpad_dnv_constraints = xpad_constraints + [
    ":dnv",
]

adcamlow_constraints = [
    ":adcamlow",
    ":aurix",
]

platform(
    name = "ib_vip_target",
    constraint_values = [
        ":ib_vip",
        ":aurix",
    ],
)

platform(
    name = "ib_vip_target_diag",
    constraint_values = [
        ":ib_vip_diag",
    ],
    parents = [
        ":ib_vip_target",
    ],
)

platform(
    name = "ib_vip_host",
    constraint_values = [
        ":ib_vip",
        "@platforms//cpu:x86_64",
    ],
)

platform(
    name = "cvc_host",
    constraint_values = [
        ":cvc",
        "@platforms//cpu:x86_64",
    ],
)

platform(
    name = "cvc2_host",
    constraint_values = [
        ":cvc2",
        "@platforms//cpu:x86_64",
    ],
)

linux_constraints = [
    "@platforms//cpu:x86_64",
    "@platforms//os:linux",
]

osx_constraints = [
    "@platforms//cpu:x86_64",
    "@platforms//os:osx",
]

osx_arm_constraints = [
    "@platforms//cpu:arm64",
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

platform(
    name = "adcamlow-tasking-win",
    constraint_values = [
        ":adcamlow",
        ":aurix",
        "@platforms//cpu:x86_64",
        "@platforms//os:windows",
    ],
)

platform(
    name = "adcamlow-tasking-linux",
    constraint_values = [
        ":adcamlow",
        ":aurix",
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
    ],
)

config_setting(
    name = "config_adcamlow_aurix",
    constraint_values = adcamlow_constraints,
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
    name = "host_windows_x86_64_msvc",
    constraint_values = [
        "@bazel_tools//tools/cpp:msvc",
    ],
    parents = [":windows_host"],
)

platform(
    name = "host_windows_x86_64_mingw",
    constraint_values = ["@bazel_tools//tools/cpp:mingw"],
    parents = [":windows_host"],
)

platform(
    name = "cvc_target",
    constraint_values = [
        ":cvc",
        ":aurix",
    ],
)

platform(
    name = "cvc2_target",
    constraint_values = [
        ":cvc2",
        ":aurix",
    ],
)

##
# host platforms which have matlab installed
platform(
    name = "host_linux_matlab_2021b",
    constraint_values = linux_constraints + ["@//bazel/platforms/matlab:2021b"],
)

platform(
    name = "host_linux_matlab_2023b",
    constraint_values = linux_constraints + ["@//bazel/platforms/matlab:2023b"],
)

platform(
    name = "host_osx_matlab_2021b",
    constraint_values = osx_constraints + ["@//bazel/platforms/matlab:2021b"],
)

platform(
    name = "host_osx_matlab_2023b",
    constraint_values = osx_constraints + ["@//bazel/platforms/matlab:2023b"],
)

platform(
    name = "host_osx_arm_matlab_2021b",
    constraint_values = osx_arm_constraints + ["@//bazel/platforms/matlab:2021b"],
)

platform(
    name = "host_osx_arm_matlab_2023b",
    constraint_values = osx_arm_constraints + ["@//bazel/platforms/matlab:2023b"],
)

platform(
    name = "host_windows_matlab_2021b",
    constraint_values = windows_constraints + ["@//bazel/platforms/matlab:2021b"],
)

platform(
    name = "host_windows_matlab_2023b",
    constraint_values = windows_constraints + ["@//bazel/platforms/matlab:2023b"],
)

platform(
    name = "host_windows_matlab_2021b_mingw",
    constraint_values = ["@bazel_tools//tools/cpp:mingw"],
    parents = [":host_windows_matlab_2021b"],
)

platform(
    name = "host_windows_matlab_2023b_mingw",
    constraint_values = ["@bazel_tools//tools/cpp:mingw"],
    parents = [":host_windows_matlab_2023b"],
)

platform(
    name = "host_windows_matlab_2021b_clangcl",
    constraint_values = ["@bazel_tools//tools/cpp:clang-cl"],
    parents = [":host_windows_matlab_2021b"],
)

platform(
    name = "host_windows_matlab_2023b_clangcl",
    constraint_values = ["@bazel_tools//tools/cpp:clang-cl"],
    parents = [":host_windows_matlab_2023b"],
)

platform(
    name = "host_linux_matlab_2021b_clang",
    constraint_values = ["@bazel_tools//tools/cpp:clang"],
    parents = [":host_linux_matlab_2021b"],
)

platform(
    name = "host_linux_matlab_2023b_clang",
    constraint_values = ["@bazel_tools//tools/cpp:clang"],
    parents = [":host_linux_matlab_2023b"],
)

# IPNext related Bazel platforms
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

alias(
    name = "hardware",
    actual = "//bazel/platforms/environment:hardware",
)

alias(
    name = "virtual",
    actual = "//bazel/platforms/environment:virtual",
)
