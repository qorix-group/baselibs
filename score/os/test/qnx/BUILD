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
load("//score/os:common_features.bzl", "COMPILER_WARNING_FEATURES")

package(
    default_visibility = [
        # "@ddad//score/os/test/qnx:__subpackages__",
    ],
)

cc_test(
    name = "dispatch_test",
    srcs = ["dispatch_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os/mocklib/qnx:dispatch_mock",
        "//score/os/qnx:channel",
        "//score/os/qnx:dispatch",
        "//score/os/qnx:iofunc",
        "//score/os/qnx:procmgr",
        "//score/os/qnx:timer",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "pcap_test",
    srcs = ["pcap_test.cpp"],
    linkopts = [
    ] + select({
        "@platforms//os:qnx": ["-lsocket"],
        "//conditions:default": [],
    }),
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os/qnx:pcap",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "dispatch_impl_test",
    srcs = ["dispatch_impl_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os/qnx:channel",
        "//score/os/qnx:dispatch",
        "//score/os/qnx:iofunc",
        "//score/os/qnx:procmgr",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "channel_test",
    srcs = ["channel_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os/mocklib/qnx:channel_mock",
        "//score/os/qnx:channel",
        "//score/os/qnx:dispatch",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "channel_impl_test",
    srcs = ["channel_impl_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os/qnx:channel",
        "//score/os/qnx:dispatch",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "devctl_test",
    srcs = ["devctl_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os:fcntl",
        "//score/os/mocklib/qnx:devctl_mock",
        "//score/os/qnx:devctl",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "iofunc_test",
    srcs = ["iofunc_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os/mocklib/qnx:iofunc_mock",
        "//score/os/qnx:iofunc",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "neutrino_test",
    srcs = [
        "neutrino_impl_test.cpp",
        "neutrino_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os/mocklib/qnx:neutrino_qnx_mock",
        "//score/os/qnx:channel",
        "//score/os/qnx:dispatch",
        "//score/os/qnx:neutrino",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "procmgr_test",
    srcs = ["procmgr_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os/mocklib/qnx:procmgr_mock",
        "//score/os/qnx:procmgr",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "resmgr_test",
    srcs = [
        "resmgr_test.cpp",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os/qnx:resmgr",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "secpol_test",
    srcs = [
        "secpol_test.cpp",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os/qnx:secpol",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "slog2_test",
    srcs = [
        "slog2_test.cpp",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os/qnx:slog2",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "pci_test",
    srcs = [
        "pci_test.cpp",
    ],
    tags = [
        "manual",
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os/qnx:neutrino",
        "//score/os/qnx:pci",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "pci_safety_test",
    srcs = [
        "pci_safety_test.cpp",
    ],
    tags = ["manual"],
    deps = [
        "//score/os/qnx:pci_safety",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "sysctl_test",
    srcs = ["sysctl_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os/mocklib/qnx:sysctl_mock",
        "//score/os/qnx:sysctl",
        "@googletest//:gtest_main",
        "//third_party/libsocket",
    ],
)

cc_test(
    name = "unistd_test",
    srcs = ["unistd_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os:unistd",
        "//score/os/qnx:unistd",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "pthread_test",
    srcs = [
        "pthread_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    deps = [
        "//score/os:pthread",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "timer_test",
    srcs = ["timer_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os/mocklib/qnx:timer_mock",
        "//score/os/qnx:timer",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "thread_ctl_test",
    srcs = ["thread_ctl_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os/mocklib/qnx:thread_ctl_mock",
        "//score/os/qnx:thread_ctl",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "inout_test",
    srcs = ["inout_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os/qnx:inout",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "mman_test",
    srcs = ["mman_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os/qnx:mman",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "fs_crypto_test",
    srcs = ["fs_crypto_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os/mocklib/qnx:fs_crypto_mock",
        "//score/os/qnx:fs_crypto",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "fs_crypto_impl_test",
    srcs = ["fs_crypto_impl_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "//score/os/qnx:fs_crypto",
        "@googletest//:gtest_main",
    ],
)
