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
load("@score-baselibs//third_party/itf:py_unittest_qnx_test.bzl", "py_unittest_qnx_test")

FEAT_COMPILER_WARNINGS_AS_ERRORS = [
    "additional_warnings",
    "strict_warnings",
    "treat_warnings_as_errors",
]

cc_test(
    name = "generic_trace_api_stub_test",
    srcs = [
        "generic_trace_api_stub_test.cpp",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    tags = [
        "manual",
        "unit",
    ],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/mocks:trace_library_mock",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api_stub",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "error_code_test",
    srcs = [
        "error_code_test.cpp",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    tags = [
        "unit",
    ],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "daemon_communicator_test",
    srcs = [
        "daemon_communicator_test.cpp",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@score-baselibs//score/analysis/tracing/common/testing_utils/notification:promise_notifier",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api:object_factory",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/daemon_communicator",
        "@score-baselibs//score/language/safecpp/scoped_function:move_only_scoped_function",
        "@score-baselibs//score/memory/shared:shared_memory_factory_mock",
        "@score-baselibs//score/memory/shared:shared_memory_resource_mock",
        "@score-baselibs//score/os/mocklib:unistd_mock",
        "@score-baselibs//score/os/mocklib/qnx:channel_mock",
        "@score-baselibs//score/os/mocklib/qnx:dispatch_mock",
        "@score-baselibs//score/os/mocklib/qnx:mman_mock",
        "@score-baselibs//score/os/mocklib/qnx:neutrino_qnx_mock",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "daemon_communicator_factory_test",
    srcs = [
        "daemon_communicator_factory_test.cpp",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api:object_factory",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/daemon_communicator",
        "@score-baselibs//score/analysis/tracing/library/test/unit_test/mocks:daemon_communicator_mock",
        "@score-baselibs//score/language/safecpp/scoped_function:move_only_scoped_function",
        "@score-baselibs//score/memory/shared:shared_memory_factory_mock",
        "@score-baselibs//score/memory/shared:shared_memory_resource_mock",
        "@score-baselibs//score/os/mocklib/qnx:channel_mock",
        "@score-baselibs//score/os/mocklib/qnx:dispatch_mock",
        "@score-baselibs//score/os/mocklib/qnx:mman_mock",
        "@score-baselibs//score/os/mocklib/qnx:neutrino_qnx_mock",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "object_factory_test",
    srcs = [
        "object_factory_test.cpp",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    tags = [
        "unit",
    ],
    deps = [
        ":local_memory_resource",
        "@score-baselibs//score/analysis/tracing/common/flexible_circular_allocator:lockless_flexible_circular_allocator",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api:object_factory",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/daemon_communicator:daemon_communicator_interface",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_allocator",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_container",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_processor:trace_job_processor_interface",
        "@score-baselibs//score/analysis/tracing/library/test/unit_test/mocks:daemon_communicator_factory_mock",
        "@score-baselibs//score/analysis/tracing/library/test/unit_test/mocks:daemon_communicator_mock",
        "@score-baselibs//score/analysis/tracing/library/test/unit_test/mocks:mock_trace_job_allocator_constructor",
        "@score-baselibs//score/analysis/tracing/shm_ring_buffer:mock_shm_ring_buffer",
        "@score-baselibs//score/memory/shared",
        "@score-baselibs//score/memory/shared:shared_memory_factory_mock",
        "@score-baselibs//score/memory/shared:shared_memory_resource_mock",
        "//platform/aas/mw/time/HWLoggerTime:utest_mock",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "trace_job_allocator_test",
    srcs = [
        "trace_job_allocator_test.cpp",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = [
        "unit",
    ],
    deps = [
        ":local_memory_resource",
        "@score-baselibs//score/analysis/tracing/common/flexible_circular_allocator/test/mocks:flexible_circular_allocator_mock",
        "@score-baselibs//score/analysis/tracing/common/interface_types:shared_memory_location_helpers",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_allocator",
        "@score-baselibs//score/analysis/tracing/plugin/ipc_trace_plugin/interface:ara_com_meta_info_trace_format",
        "@score-baselibs//score/analysis/tracing/shm_ring_buffer:mock_shm_ring_buffer",
        "@score-baselibs//score/memory/shared:shared_memory_resource_mock",
        "//platform/aas/mw/time/HWLoggerTime:utest_mock",
        "//platform/aas/mw/time/HighPrecisionLocalSteadyClock",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "containers_test",
    srcs = [
        "containers_test.cpp",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = [
        "unit",
    ],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api",
        "@score-baselibs//score/analysis/tracing/library/test/unit_test/mocks:daemon_communicator_mock",
        "@score-baselibs//score/analysis/tracing/library/test/unit_test/mocks:object_factory_mock",
        "@score-baselibs//score/analysis/tracing/library/test/unit_test/mocks:trace_job_allocator_mock",
        "@score-baselibs//score/analysis/tracing/library/test/unit_test/mocks:trace_job_processor_mock",
        "@score-baselibs//score/memory/shared:shared_memory_factory_mock",
        "@score-baselibs//score/memory/shared:shared_memory_resource_mock",
        "@score-baselibs//score/os/mocklib:unistd_mock",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "generic_trace_api_test",
    srcs = [
        "generic_trace_api_test.cpp",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    tags = [
        "unit",
    ],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/mocks:trace_library_mock",
        "@score-baselibs//score/analysis/tracing/library/test/unit_test/mocks:daemon_communicator_mock",
        "@score-baselibs//score/analysis/tracing/library/test/unit_test/mocks:object_factory_mock",
        "@score-baselibs//score/analysis/tracing/library/test/unit_test/mocks:trace_job_allocator_mock",
        "@score-baselibs//score/analysis/tracing/library/test/unit_test/mocks:trace_job_processor_mock",
        "@score-baselibs//score/memory/shared:shared_memory_factory_mock",
        "@score-baselibs//score/memory/shared:shared_memory_resource_mock",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "local_data_chunk_list_test",
    srcs = [
        "local_data_chunk_list_test.cpp",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    tags = ["unit"],
    deps = [
        ":local_memory_resource",
        "@score-baselibs//score/analysis/tracing/common/flexible_circular_allocator:lockless_flexible_circular_allocator",
        "@score-baselibs//score/analysis/tracing/common/flexible_circular_allocator/test/mocks:flexible_circular_allocator_mock",
        "@score-baselibs//score/analysis/tracing/common/interface_types:shared_memory_location_helpers",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/chunk_list",
        "@score-baselibs//score/mw/log",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "shm_data_chunk_list_test",
    srcs = [
        "shm_data_chunk_list_test.cpp",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    tags = ["unit"],
    deps = [
        ":local_memory_resource",
        "@score-baselibs//score/analysis/tracing/common/flexible_circular_allocator/test/mocks:flexible_circular_allocator_mock",
        "@score-baselibs//score/analysis/tracing/common/interface_types:shared_memory_location_helpers",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/chunk_list",
        "@score-baselibs//score/mw/log",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "trace_job_container_test",
    srcs = [
        "trace_job_container_test.cpp",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_container",
        "@score-baselibs//score/mw/log",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "trace_job_processor_test",
    srcs = [
        "trace_job_processor_test.cpp",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    deps = [
        ":local_memory_resource",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/chunk_list",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_processor",
        "@score-baselibs//score/analysis/tracing/library/test/unit_test/mocks:trace_job_container_mock",
        "@score-baselibs//score/memory/shared:atomic_indirector_mock_binding",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "ara_com_properities_test",
    srcs = [
        "ara_com_properities_test.cpp",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/interface:meta_info",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "qnx_memory_validator_test",
    srcs = [
        "qnx_memory_validator_test.cpp",
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
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/memory_validator/qnx",
        "@score-baselibs//score/os/mocklib/qnx:mman_mock",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "local_memory_resource",
    srcs = ["local_memory_resource.cpp"],
    hdrs = ["local_memory_resource.h"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        "@score-baselibs//score/memory/shared",
    ],
)

# For usual cc_tests
common_tests = [
    ":ara_com_properities_test",
    ":containers_test",
    ":generic_trace_api_test",
    ":local_data_chunk_list_test",
    ":object_factory_test",
    ":shm_data_chunk_list_test",
    ":trace_job_allocator_test",
    ":trace_job_container_test",
    ":trace_job_processor_test",
    "@score-baselibs//score/analysis/tracing/library/test/unit_test:error_code_test",
    "@score-baselibs//score/analysis/tracing/library/test/unit_test:generic_trace_api_stub_test",
]

# For usual cc_tests
qnx_only_tests = [
    ":qnx_memory_validator_test",
    ":daemon_communicator_factory_test",
    ":daemon_communicator_test",
]

# For test suites generated using cc_unit_test_suites_for_host_and_qnx
host_only_test_suites = [
    "@score-baselibs//score/analysis/tracing/library/test/unit_test/generic_trace_api_impl:unit_tests_host",
]

# For test suites generated using cc_unit_test_suites_for_host_and_qnx
qnx_only_test_suites = [
    "@score-baselibs//score/analysis/tracing/library/test/unit_test/generic_trace_api_impl:unit_tests_qnx",
]

py_unittest_qnx_test(
    name = "unit_tests_qnx",
    test_cases = common_tests + qnx_only_tests,
    test_suites = qnx_only_test_suites,
    visibility = ["@score-baselibs//score/analysis/tracing:__subpackages__"],
)

test_suite(
    name = "unit_tests_host",
    tests = common_tests + host_only_test_suites,
    visibility = ["@score-baselibs//score/analysis/tracing/library:__subpackages__"],
)
