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
cc_library(
    name = "generic_trace_api_impl",
    srcs = [
        "generic_trace_api_impl.cpp",
    ],
    hdrs = [
        "generic_trace_api_impl.h",
        "library_state.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = [
        "@score-baselibs//score/analysis:__subpackages__",
        "//platform/aas/ara:__subpackages__",
        "//platform/aas/mw/com:__subpackages__",
    ],
    deps = [
        ":object_factory",
        "@score-baselibs//score/analysis/tracing/common/interface_types:generic_trace_api_types",
        "@score-baselibs//score/analysis/tracing/common/interface_types:shared_memory_chunk",
        "@score-baselibs//score/analysis/tracing/common/interface_types:shared_memory_location",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/chunk_list",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/containers/client_id",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/containers/shm_object_handle",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/memory_validator",
        "@score-baselibs//score/analysis/tracing/library/interface:meta_info",
        "@score-baselibs//score/analysis/tracing/library/interface:trace_library_interface",
        "@score-baselibs//score/analysis/tracing/shm_ring_buffer",
        "@score-baselibs//score/concurrency",
        "@score-baselibs//score/os/utils:thread",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "generic_trace_api",
    srcs = [
        "generic_trace_api.cpp",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = [
        "@score-baselibs//score/analysis:__subpackages__",
        "//platform/aas/ara:__subpackages__",
        "//platform/aas/mw/com:__subpackages__",
    ],
    deps = [
        ":generic_trace_api_impl",
        "@score-baselibs//score/analysis/tracing/library/interface:generic_trace_api_header",
    ],
)

cc_library(
    name = "object_factory_interface",
    hdrs = [
        "i_object_factory.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    tags = ["FFI"],
    visibility = ["@score-baselibs//score/analysis/tracing/library:__subpackages__"],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/daemon_communicator:daemon_communicator_interface",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_allocator:trace_job_allocator_interface",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_container",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_processor:trace_job_processor_interface",
        "@score-baselibs//score/analysis/tracing/shm_ring_buffer",
        "//platform/aas/mw/time/HWLoggerTime",
    ],
)

cc_library(
    name = "object_factory",
    srcs = [
        "object_factory.cpp",
    ],
    hdrs = [
        "object_factory.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    tags = ["FFI"],
    visibility = ["@score-baselibs//score/analysis/tracing/library:__subpackages__"],
    deps = [
        ":object_factory_interface",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/daemon_communicator",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_allocator",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_container",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_processor",
    ],
)
