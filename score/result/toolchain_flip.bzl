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

"""Utilities to rebuild deps with the opposite C++ stdlib toolchain.

This uses a configuration transition to flip between the gcc-based platform
(`//bazel/platforms:host_linux_x86_64_gcc`) and the clang-based platform
(`//bazel/platforms:host_linux_x86_64_clang`) and explicitly choose to use
clang's stdlib.
The primary intent is to produce an alternate build of
`@score_baselibs//score/result:result` that is compiled with a different stdlib
than the rest of the build, so the Rust-side OnceLock check can observe a
real mismatch at runtime.
"""

_CLANG_PLATFORM = "//bazel/platforms:host_linux_x86_64_clang"
_CLANG_TOOLCHAIN = "//bazel/toolchains/ubuntu_20_04_clang:clang"
_CLANG_VERSION_FLAG = "//bazel/toolchains:clang_version"
_EXTRA_COPTS = [
    "-Wno-return-type-c-linkage",
]
_EXTRA_CXXOPTS = [
    "-stdlib=libc++",
]

# Auxiliary function to add extra parameters to a list
def _add_extra_param(original, extra):
    result = original
    for flag in extra:
        if flag not in result:
            result = result + [flag]
    return result

# The transition always rebuilds the dependency with the clang/libc++ toolchain.
# This is sufficient for the mismatch test, which is executed from the gcc/libstdc++
# configuration (`--config=spp_host_gcc`).
def _flip_stdlib_transition_impl(settings, _):
    copts = settings.get("//command_line_option:copt", [])
    copts_out = _add_extra_param(copts, _EXTRA_COPTS)

    cxxopts = settings.get("//command_line_option:cxxopt", [])

    # ensure deterministic ordering; append if not present
    cxxopts_out = _add_extra_param(cxxopts, _EXTRA_CXXOPTS)

    return {
        "//command_line_option:copt": copts_out,
        "//command_line_option:cxxopt": cxxopts_out,
        "//command_line_option:extra_execution_platforms": _CLANG_PLATFORM,
        "//command_line_option:extra_toolchains": _CLANG_TOOLCHAIN,
        _CLANG_VERSION_FLAG: "19",
        "//command_line_option:host_platform": _CLANG_PLATFORM,
        "//command_line_option:platforms": _CLANG_PLATFORM,
    }

_flip_stdlib_transition = transition(
    implementation = _flip_stdlib_transition_impl,
    inputs = [
        "//command_line_option:platforms",
        "//command_line_option:host_platform",
        "//command_line_option:extra_execution_platforms",
        "//command_line_option:extra_toolchains",
        _CLANG_VERSION_FLAG,
        "//command_line_option:copt",
        "//command_line_option:cxxopt",
    ],
    outputs = [
        "//command_line_option:platforms",
        "//command_line_option:host_platform",
        "//command_line_option:extra_execution_platforms",
        "//command_line_option:extra_toolchains",
        _CLANG_VERSION_FLAG,
        "//command_line_option:copt",
        "//command_line_option:cxxopt",
    ],
)

def _flip_stdlib_cc_library_impl(ctx):
    dep = ctx.attr.dep
    if type(dep) == type([]):
        dep = dep[0]
    return [dep[CcInfo], dep[DefaultInfo]]

flip_stdlib_cc_library = rule(
    implementation = _flip_stdlib_cc_library_impl,
    attrs = {
        "dep": attr.label(
            cfg = _flip_stdlib_transition,
            providers = [CcInfo],
            allow_files = False,
        ),
    },
    outputs = {},
    fragments = ["cpp"],
    doc = "Rebuilds a cc_library with the opposite toolchain and re-exports its CcInfo.",
)
