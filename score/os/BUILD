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

cc_library(
    name = "acl",
    srcs = [
        "acl.cpp",
        "acl_impl.cpp",
    ],
    hdrs = [
        "acl.h",
        "acl_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/os:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
    ] + select(
        {
            "@platforms//os:linux": [
                "//third_party/acl",
            ],
            "//conditions:default": [],
        },
    ),
)

cc_library(
    name = "dirent",
    srcs = [
        "dirent.cpp",
        "dirent_impl.cpp",
    ],
    hdrs = [
        "dirent.h",
        "dirent_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/filesystem:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/mw/com/impl/bindings/lola:__subpackages__",
        "//platform/aas/pas/hw_reg_info:__subpackages__",
        "//platform/aas/pas/std_frame:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
    ],
)

cc_library(
    name = "stat",
    srcs = [
        "stat.cpp",
        "stat_impl.cpp",
    ],
    hdrs = [
        "stat.h",
        "stat_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/analysis/tracing/daemon:__subpackages__",
        "//platform/aas/ara/tsync/bmw:__subpackages__",
        "//platform/aas/intc/typedmemd:__subpackages__",
        "//score/filesystem:__subpackages__",
        "//score/memory/shared:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/mw/com/message_passing:__subpackages__",
        "//platform/aas/mw/com/test/common_test_resources:__subpackages__",
        "//score/mw/log:__subpackages__",
        "//platform/aas/pas/logging:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
        "//score/bitmanipulation:bitmask_operators",
    ],
)

cc_library(
    name = "fcntl",
    srcs = [
        "fcntl.cpp",
        "fcntl_impl.cpp",
    ],
    hdrs = [
        "fcntl.h",
        "fcntl_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/abac/abac/core/BasicSecurity/generic:__subpackages__",
        "//score/analysis/tracing/daemon/code:__subpackages__",
        "//platform/aas/ara/tsync/ptp:__subpackages__",
        "//platform/aas/intc/jc_launcher:__subpackages__",
        "//platform/aas/intc/typedmemd:__subpackages__",
        "//score/filesystem:__subpackages__",
        "//platform/aas/lib/hal/gpio:__subpackages__",
        "//score/memory/shared:__subpackages__",
        "//platform/aas/lib/message_passing:__subpackages__",
        "//platform/aas/lib/network:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/mw/com/impl/bindings/lola:__subpackages__",
        "//platform/aas/mw/com/message_passing:__subpackages__",
        "//platform/aas/mw/com/test:__subpackages__",
        "//score/mw/log:__subpackages__",
        "//platform/aas/mw/storage:__subpackages__",
        "//platform/aas/pas/crash_handler:__subpackages__",
        "//platform/aas/pas/crash_reporter_ara_proxy:__subpackages__",
        "//platform/aas/pas/hw_reg_info:__subpackages__",
        "//platform/aas/pas/lib/zlib_util:__subpackages__",
        "//platform/aas/pas/logging:__subpackages__",
        "//platform/aas/sec/secured:__subpackages__",
        "//platform/aas/test/pas/crash_reporter:__subpackages__",
        "//platform/aas/test/pas/crash_reporter_ara_proxy:__subpackages__",
        "//platform/aas/tools/proxy_app:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
        ":stat",
        "//score/bitmanipulation:bitmask_operators",
    ],
)

cc_library(
    name = "getopt",
    srcs = [
        "getopt.cpp",
        "getopt_impl.cpp",
    ],
    hdrs = [
        "getopt.h",
        "getopt_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/intc/conditional_launcher:__subpackages__",
        "//platform/aas/intc/configure_persistency:__subpackages__",
        "//platform/aas/intc/typedmemd:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/pas/dtc_status_sender:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
    ],
)

cc_library(
    name = "inotify",
    srcs = [
        "inotify.cpp",
        "inotify_impl.cpp",
    ],
    hdrs = [
        "inotify.h",
        "inotify_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/os:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
        "//score/bitmanipulation:bitmask_operators",
    ],
)

cc_library(
    name = "ioctl",
    srcs = [
        "ioctl.cpp",
        "ioctl_impl.cpp",
    ],
    hdrs = [
        "ioctl.h",
        "ioctl_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/analysis/tracing/daemon/code:__subpackages__",
        "//platform/aas/ara/tsync/ptp:__subpackages__",
        "//platform/aas/lib/network:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/sysfunc/BasicSecurity/code/pdr_eraser/details:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
    ],
)

cc_library(
    name = "grp",
    srcs = [
        "grp.cpp",
        "grp_impl.cpp",
    ],
    hdrs = [
        "grp.h",
        "grp_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/filesystem:__subpackages__",
        "//score/os:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
    ],
)

cc_library(
    name = "libgen",
    srcs = [
        "libgen.cpp",
        "libgen_impl.cpp",
    ],
    hdrs = [
        "libgen.h",
        "libgen_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/os:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
    ],
)

cc_library(
    name = "select",
    srcs = [
        "select.cpp",
        "select_impl.cpp",
    ],
    hdrs = [
        "select.h",
        "select_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/os:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
    ],
)

cc_library(
    name = "mqueue",
    srcs = ["mqueue.cpp"],
    hdrs = ["mqueue.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/os:__subpackages__",
        "//platform/aas/mw/com/message_passing:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
        ":static_destruction_guard",
        "//score/bitmanipulation:bitmask_operators",
    ],
)

cc_library(
    name = "semaphore",
    srcs = [
        "semaphore.cpp",
        "semaphore_impl.cpp",
    ],
    hdrs = [
        "semaphore.h",
        "semaphore_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/analysis/tracing/daemon/code/plugin/perf:__subpackages__",
        "//platform/aas/ara/tsync/bmw:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/mw/time/HWLoggerTime:__subpackages__",
        "//platform/aas/network/xnm/src/ifconfig_wrapper:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
        "//score/bitmanipulation:bitmask_operators",
    ],
)

cc_library(
    name = "statvfs",
    srcs = [
        "statvfs.cpp",
        "statvfs_impl.cpp",
    ],
    hdrs = [
        "statvfs.h",
        "statvfs_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/os:__subpackages__",
        "//platform/aas/pas/crash_reporter:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
        "//score/bitmanipulation:bitmask_operators",
    ],
)

cc_library(
    name = "unistd",
    srcs = ["unistd.cpp"],
    hdrs = ["unistd.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//application/adp/aas/lifecycle/qnx_migration:__subpackages__",  # Temporary workaround, see Ticket-84191
        "//platform/aas/abac/abac/core/BasicSecurity/generic:__subpackages__",
        "//score/analysis/tracing:__subpackages__",
        "//platform/aas/ara/tsync/bmw:__subpackages__",
        "//platform/aas/ara/tsync/ptp:__subpackages__",
        "//platform/aas/intc/conditional_launcher:__subpackages__",
        "//platform/aas/intc/configure_persistency:__subpackages__",
        "//platform/aas/intc/jc_launcher:__subpackages__",
        "//platform/aas/intc/lifecycle_state_machine:__subpackages__",
        "//platform/aas/intc/typedmemd:__subpackages__",
        "//score/filesystem:__subpackages__",
        "//platform/aas/lib/hal/gpio:__subpackages__",
        "//score/json:__subpackages__",
        "//score/memory/shared:__subpackages__",
        "//platform/aas/lib/message_passing:__subpackages__",
        "//platform/aas/lib/network:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/mw/com/impl/bindings/lola:__subpackages__",
        "//platform/aas/mw/com/message_passing:__subpackages__",
        "//platform/aas/mw/com/test/common_test_resources:__subpackages__",
        "//score/mw/log:__subpackages__",
        "//platform/aas/mw/storage:__subpackages__",
        "//platform/aas/network/xnm/src/osif:__subpackages__",
        "//platform/aas/pas/crash_handler:__subpackages__",
        "//platform/aas/pas/crash_reporter_ara_proxy:__subpackages__",
        "//platform/aas/pas/datarouterconf:__subpackages__",
        "//platform/aas/pas/dtc_status_sender:__subpackages__",
        "//platform/aas/pas/hw_reg_info:__subpackages__",
        "//platform/aas/pas/hw_reg_res_mgrs:__subpackages__",
        "//platform/aas/pas/instrumentation:__subpackages__",
        "//platform/aas/pas/lib/zlib_util:__subpackages__",
        "//platform/aas/pas/logging:__subpackages__",
        "//platform/aas/sec/secured:__subpackages__",
        "//platform/aas/sysfunc/SoftwareIdProvider/code/lib_swid_handler:__subpackages__",
        "//platform/aas/test/os/linux/example_application:__subpackages__",
        "//platform/aas/test/pas/crash_reporter:__subpackages__",
        "//platform/aas/test/pas/crash_reporter_ara_proxy:__subpackages__",
        "//platform/aas/tools/elk_persistent_backup:__subpackages__",
        "//platform/aas/tools/proxy_app:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
        ":static_destruction_guard",
        "//score/bitmanipulation:bitmask_operators",
    ],
)

cc_library(
    name = "arpa_inet",
    srcs = ["arpa_inet.cpp"],
    hdrs = [
        "arpa_inet.h",
        "netinet_in.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//score/os:__subpackages__",
        "//platform/aas/network/tunneling/npdu_daemon:__subpackages__",
        "//platform/aas/pas/dtc_status_sender:__subpackages__",
        "//platform/aas/sysfunc/SafeCarTime/code/apps/TimeSyncValidationNonFFI/utils/impl:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
    ],
)

cc_library(
    name = "netdb",
    srcs = ["netdb.cpp"],
    hdrs = ["netdb.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//score/os:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
        "//score/bitmanipulation:bitmask_operators",
    ],
)

cc_library(
    name = "net_if",
    srcs = ["net_if.cpp"],
    hdrs = ["net_if.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//score/analysis/tracing/daemon:__subpackages__",
        "//platform/aas/ara/tsync/ptp:__subpackages__",
        "//score/os:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
        "//score/bitmanipulation:bitmask_operators",
    ],
)

cc_library(
    name = "cpuid",
    srcs = ["cpuid.cpp"],
    hdrs = ["cpuid.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//score/os:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
    ],
)

cc_library(
    name = "socket",
    srcs = [
        "socket.cpp",
        "socket_impl.cpp",
    ],
    hdrs = [
        "socket.h",
        "socket_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//platform/aas/ara/tsync/ptp:__subpackages__",
        "//platform/aas/lib/message_passing:__subpackages__",
        "//platform/aas/lib/network:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/pas/calibration_server:__subpackages__",
        "//platform/aas/pas/datarouterconf:__subpackages__",
        "//platform/aas/pas/dtc_status_sender:__subpackages__",
        "//platform/aas/pas/logging:__subpackages__",
        "//platform/aas/sysfunc/SafeCarTime/code/apps/TimeSyncValidationNonFFI/utils/impl:__subpackages__",
        "//platform/aas/test/pas/perf_tests:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
        "//score/bitmanipulation:bitmask_operators",
    ] + select({
        "@platforms//os:qnx": [
            "//third_party/libsocket",
        ],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "make_dev",
    srcs = ["makedev.cpp"],
    hdrs = ["makedev.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//score/os:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
    ],
)

cc_library(
    name = "file_stream",
    srcs = ["file_stream.cpp"],
    hdrs = ["file_stream.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//score/os:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
    ],
)

cc_library(
    name = "ifaddrs",
    srcs = [
        "ifaddrs.cpp",
        "ifaddrs_impl.cpp",
    ],
    hdrs = [
        "ifaddrs.h",
        "ifaddrs_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//score/analysis/tracing/daemon:__subpackages__",
        "//platform/aas/lib/network:__subpackages__",
        "//score/os:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
    ],
)

cc_library(
    name = "sys_poll",
    srcs = [
        "sys_poll.cpp",
        "sys_poll_impl.cpp",
    ],
    hdrs = [
        "sys_poll.h",
        "sys_poll_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/ara/tsync/ptp:__subpackages__",
        "//platform/aas/lib/message_passing:__subpackages__",
        "//platform/aas/lib/network:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/network/xnm/src/osif:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
    ],
)

cc_library(
    name = "sys_uio",
    srcs = [
        "sys_uio.cpp",
        "sys_uio_impl.cpp",
    ],
    hdrs = [
        "sys_uio.h",
        "sys_uio_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/analysis/tracing/daemon:__subpackages__",
        "//platform/aas/lib/message_passing:__subpackages__",
        "//score/os:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
    ],
)

cc_library(
    name = "capability",
    srcs = select({
        "@platforms//os:linux": [
            "//score/os/linux:capability.cpp",
            "//score/os/linux:capability.h",
        ],
        "@platforms//os:qnx": [
            "//score/os/qnx:capability.h",
        ],
    }) + [
        "capability.cpp",
        "capability.h",
    ],
    defines = ["ALLOW_QM"],
    features = COMPILER_WARNING_FEATURES,
    linkopts = select({
        "//bazel/platforms:config_xpad_dnv": [
            "-lcap",
        ],
        "//conditions:default": [],
    }) + [],
    visibility = [
        "//application/adp/aas/applications:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/pas/sysmon:__subpackages__",
        "//platform/aas/pas/temperature_sender:__subpackages__",
        "//platform/aas/test/os/linux/example_application:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    #if QNX or DNV-GCC Toolchain dont link with explicit TP/libpcap,
    #if its a host build for linux do link with TP/libpcap
    deps = select({
        "//bazel/platforms:config_xpad_dnv": [],
        "@platforms//os:qnx": [],
        "//conditions:default": [
            "//third_party/libcap2",
        ],
    }) + [
        ":errno_logging",
        ":object_seam",
        "//score/mw/log",
    ],
)

cc_library(
    name = "mount",
    srcs = [
        "mount.cpp",
        "mount_impl.cpp",
    ],
    hdrs = [
        "mount.h",
        "mount_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/intc/configure_persistency:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/sysfunc/ISOCMinion/code/installer/installer_factory:__subpackages__",
        "//platform/aas/sysfunc/ISOCMinion/code/partition_mounty/details:__subpackages__",
        "//platform/aas/sysfunc/SoftwareIdProvider:__subpackages__",
        "//platform/aas/tools/elk_persistent_backup:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
        "//score/bitmanipulation:bitmask_operators",
    ],
)

cc_library(
    name = "stdio",
    srcs = [
        "stdio.cpp",
        "stdio_impl.cpp",
    ],
    hdrs = [
        "stdio.h",
        "stdio_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//application/adp/aas/lifecycle/qnx_migration:__subpackages__",  # Temporary workaround, see Ticket-84191
        "//score/filesystem:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/network/xnm/src/common:__subpackages__",
        "//platform/aas/network/xnm/src/osif:__subpackages__",
        "//platform/aas/pas/crash_reporter:__subpackages__",
        "//platform/aas/pas/hw_reg_info:__subpackages__",
        "//platform/aas/pas/logging:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
        "//score/bitmanipulation:bitmask_operators",
    ],
)

cc_library(
    name = "stdlib",
    srcs = [
        "stdlib.cpp",
        "stdlib_impl.cpp",
    ],
    hdrs = [
        "stdlib.h",
        "stdlib_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/intc/configure_persistency:__subpackages__",
        "//platform/aas/intc/typedmemd:__subpackages__",
        "//score/filesystem:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/mw/diag:__subpackages__",
        "//platform/aas/mw/lifecycle:__subpackages__",
        "//score/mw/log:__subpackages__",
        "//platform/aas/network/xnm:__subpackages__",
        "//platform/aas/pas/crash_reporter:__subpackages__",
        "//platform/aas/pas/logging:__subpackages__",
        "//platform/aas/sysfunc/ISOCMinion/code:__subpackages__",
        "//platform/aas/tools/elk_persistent_backup:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
    ],
)

cc_library(
    name = "string",
    srcs = [
        "string.cpp",
        "string_impl.cpp",
    ],
    hdrs = [
        "string.h",
        "string_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/intc/jc_launcher:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/pas/hw_reg_info:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
    ],
)

cc_library(
    name = "time",
    srcs = [
        "time.cpp",
        "time_impl.cpp",
    ],
    hdrs = [
        "time.h",
        "time_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/analysis/tracing/daemon/code/plugin/common/mock:__subpackages__",
        "//score/analysis/tracing/daemon/code/plugin/common/plugin_statistics:__subpackages__",
        "//score/analysis/tracing/daemon/code/plugin/ipc_trace/ipc_trace_statistics:__subpackages__",
        "//platform/aas/ara/SDaT2Sys/code/app/sdat_sync:__subpackages__",
        "//platform/aas/ara/iam/code:__subpackages__",
        "//platform/aas/ara/iam/test/PDPHostSimulation:__subpackages__",
        "//platform/aas/ara/tsync/ptp:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/network/xnm/src/ifconfig_wrapper:__subpackages__",
        "//platform/aas/pas/crash_reporter:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
    ],
)

cc_library(
    name = "static_destruction_guard",
    hdrs = ["static_destruction_guard.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
)

cc_library(
    name = "mman",
    srcs = ["mman.cpp"],
    hdrs = ["mman.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/ara/tsync/bmw:__subpackages__",
        "//platform/aas/intc/typedmemd:__subpackages__",
        "//score/memory/shared:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/mw/com/test/common_test_resources:__subpackages__",
        "//score/mw/log:__subpackages__",
        "//platform/aas/pas/hw_reg_info:__subpackages__",
        "//platform/aas/pas/hw_reg_res_mgrs:__subpackages__",
        "//platform/aas/pas/logging:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":fcntl",
        ":object_seam",
        ":rt",
        ":static_destruction_guard",
        "//score/bitmanipulation:bitmask_operators",
    ],
)

cc_library(
    name = "errno",
    srcs = ["errno.cpp"],
    hdrs = [
        "errno.h",
        "errno_decl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/analysis/tracing/daemon:__subpackages__",
        "//platform/aas/ara/log:__subpackages__",
        "//platform/aas/ara/log/test:__subpackages__",
        "//platform/aas/ara/tsync/bmw:__subpackages__",
        "//score/filesystem:__subpackages__",
        "//platform/aas/lib/message_passing:__subpackages__",
        "//platform/aas/lib/network:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/mw/com/message_passing:__subpackages__",
        "//platform/aas/pas/hw_reg_info:__subpackages__",
        "//platform/aas/pas/hw_reg_res_mgrs:__subpackages__",
        "//platform/aas/pas/lib/ddad_version_reader/test/component:__subpackages__",
        "//platform/aas/sysfunc/BasicSecurity/code/pdr_eraser:__subpackages__",
        "//platform/aas/sysfunc/ISOCMinion:__subpackages__",
        "//platform/aas/sysfunc/SafeCarTime:__subpackages__",
        "//platform/aas/sysfunc/SoftwareIdProvider:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    # TODO: (Ticket-75170) Clarify if amp is allowed to be used in ASIL B context
    deps = ["@score-baselibs//score/language/futurecpp"],
)

cc_library(
    name = "errno_logging",
    srcs = ["errno_logging.cpp"],
    hdrs = ["errno_logging.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/lib/hal/gpio:__subpackages__",
        "//score/memory/shared:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/mw/com:__subpackages__",
        "//platform/aas/network/tunneling/npdu_daemon:__subpackages__",
        "//platform/aas/network/xnm:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        "//score/mw/log:log_stream",
    ],
)

alias(
    name = "pthread",
    actual = select({
        "@platforms//os:qnx": ":pthread_qnx",
        "//conditions:default": ":pthread_default",
    }),
    visibility = [
        "//application/adp/activities/mic/someip_message_receiver:__subpackages__",
        "//platform/aas/abac/mw/diag:__subpackages__",
        "//score/analysis/tracing:__subpackages__",
        "//platform/aas/ara/log/example:__subpackages__",
        "//score/concurrency:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/mw/diag:__subpackages__",
        "//score/mw/log:__subpackages__",
        "//platform/aas/pas/logging:__subpackages__",
        "//platform/aas/sec/sfa_parser:__subpackages__",
        "//platform/aas/sec/w_hsmlib:__subpackages__",
        "//platform/aas/sysfunc/BasicSecurity:__subpackages__",
        "//platform/aas/test/os/linux/example_application:__subpackages__",
        "//platform/aas/test/pas/perf_tests/non_functional_tests/malloc:__subpackages__",
        "//platform/aas/tools/itf/external/vsomeip:__subpackages__",
        "//platform/aas/tools/itf/itf_cpp/mdf4_reprocessing:__subpackages__",
        "//platform/aas/tools/testing/thread_benchmark:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
)

cc_library(
    name = "pthread_qnx",
    srcs = [
        "pthread.cpp",
        "qnx_pthread.cpp",
        "qnx_pthread.h",
    ],
    hdrs = [
        "pthread.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:private"],  # only to be used via above alias
    deps = [
        ":errno",
        ":object_seam",
    ],
)

cc_library(
    name = "pthread_default",
    srcs = [
        "linux_pthread.cpp",
        "linux_pthread.h",
        "pthread.cpp",
    ],
    hdrs = [
        "pthread.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    linkopts = ["-lpthread"],
    target_compatible_with = select({
        "@platforms//os:qnx": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:private"],  # only to be used via above alias
    deps = [
        ":errno",
        ":object_seam",
    ],
)

alias(
    name = "rt",
    actual = select({
        "@platforms//os:qnx": ":rt_qnx",
        "//conditions:default": ":rt_default",
    }),
    tags = ["FFI"],
    visibility = [
        "//platform/aas/ara/tsync/bmw:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
)

cc_library(
    name = "rt_qnx",
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:private"],  # only to be used via above alias
)

cc_library(
    name = "rt_default",
    features = COMPILER_WARNING_FEATURES,
    linkopts = ["-lrt"],
    target_compatible_with = select({
        "@platforms//os:qnx": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:private"],  # only to be used via above alias
)

cc_library(
    name = "object_seam",
    hdrs = ["ObjectSeam.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/filesystem:__subpackages__",
        "//platform/aas/lib/network:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/sysfunc/BasicSecurity/code/pdr_eraser:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
)

cc_library(
    name = "sched",
    srcs = [
        "sched.cpp",
        "sched_impl.cpp",
    ],
    hdrs = [
        "sched.h",
        "sched_impl.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = [
        "//platform/aas/intc/configure_persistency/code/persistency_encryption:__subpackages__",
        "//platform/aas/intc/jc_launcher:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/pas/sysmon:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        "//score/os:errno",
        "//score/os:object_seam",
    ],
)

cc_library(
    name = "spawn",
    srcs = [
        "spawn.cpp",
        "spawn_impl.cpp",
    ],
    hdrs = [
        "spawn.h",
        "spawn_impl.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = [
        "//platform/aas/intc/configure_persistency/code/persistency_encryption:__subpackages__",
        "//platform/aas/intc/jc_launcher:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/network/xnm/src/common:__subpackages__",
        "//platform/aas/pas/crash_reporter:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        "//score/os:errno",
        "//score/os:object_seam",
    ],
)

cc_library(
    name = "sys_wait",
    srcs = [
        "sys_wait.cpp",
        "sys_wait_impl.cpp",
    ],
    hdrs = [
        "sys_wait.h",
        "sys_wait_impl.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = [
        "//platform/aas/intc/jc_launcher:__subpackages__",
        "//score/os:__subpackages__",
        "//platform/aas/network/xnm/src/common:__subpackages__",
        "//platform/aas/pas/crash_reporter:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        "//score/os:errno",
        "//score/os:object_seam",
    ],
)

cc_library(
    name = "uname",
    srcs = [
        "uname.cpp",
        "uname_impl.cpp",
    ],
    hdrs = [
        "uname.h",
        "uname_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/os:__subpackages__",
        "//platform/aas/network/xnm/src/common:__subpackages__",
        "//platform/aas/pas/crash_reporter:__subpackages__",
        # "@ddad//score/os:__subpackages__",
    ],
    deps = [
        ":errno",
        ":object_seam",
    ],
)

cc_library(
    name = "glob",
    srcs = [
        "glob.cpp",
        "glob_impl.cpp",
    ],
    hdrs = [
        "glob.h",
        "glob_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
        # "@ddad//visibility:public",
    ],
    deps = [
        ":errno",
        ":object_seam",
        "//score/bitmanipulation:bitmask_operators",
        "@score-baselibs//score/language/futurecpp",
    ],
)
