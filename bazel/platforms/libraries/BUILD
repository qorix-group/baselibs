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

# Some targets might expect that a toolchain (or platform)
# comes with specific libraries. If such a library would not
# be available, the target could not build
constraint_setting(name = "libraries")

# dev-package of https://github.com/systemd/systemd
constraint_value(
    name = "systemd",
    constraint_setting = ":libraries",
    visibility = ["//visibility:public"],
)
