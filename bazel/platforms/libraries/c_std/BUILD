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

# Our different toolchains are only "semi" hermetic.
# They are fully hermetic on build time but cannot be
# fully hermetic on runtime, since the are depending
# on the _to be run on host_.
# In order to enable automatic correct toolchain selection
# based on Bazel platforms, we need to define which
# C Standard Library is used by which platform and toolchain.
# e.g. glibc-3.44 or bsd-libc.5.11
constraint_setting(name = "c_std")

constraint_value(
    name = "glibc-2.28",
    constraint_setting = ":c_std",
    visibility = [
        "//bazel/platforms:__subpackages__",
        "//bazel/toolchains:__subpackages__",
        # "@ddad//bazel/platforms/libraries/c_std:__subpackages__",
    ],
)
