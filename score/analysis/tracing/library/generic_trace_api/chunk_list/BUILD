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
    name = "chunk_list",
    srcs = [
        "local_data_chunk_list.cpp",
        "shm_data_chunk_list.cpp",
    ],
    hdrs = [
        "local_data_chunk_list.h",
        "shm_data_chunk_list.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = [
        "@score-baselibs//score/analysis:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/analysis/tracing/common/flexible_circular_allocator:lockless_flexible_circular_allocator_factory",
        "@score-baselibs//score/analysis/tracing/common/interface_types:generic_trace_api_types",
        "@score-baselibs//score/analysis/tracing/common/interface_types:shared_memory_chunk",
        "@score-baselibs//score/analysis/tracing/common/interface_types:shared_memory_location",
        "@score-baselibs//score/analysis/tracing/common/interface_types:shared_memory_location_helpers",
        "@score-baselibs//score/analysis/tracing/common/shared_list",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
        "@score-baselibs//score/memory/shared:types",
        "@score-baselibs//score/language/futurecpp",
    ],
)
