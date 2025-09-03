load("@score-baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")

FEAT_COMPILER_WARNINGS_AS_ERRORS = [
    "additional_warnings",
    "strict_warnings",
    "treat_warnings_as_errors",
]

cc_library(
    name = "generic_trace_api_impl_test_library",
    testonly = True,
    srcs = [
        "generic_trace_api_impl_test.cpp",
    ],
    hdrs = [
        "generic_trace_api_impl_test.h",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    deps = [
        "@score-baselibs//score/analysis/tracing/common/testing_utils/notification:promise_notifier",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api",
        "@score-baselibs//score/analysis/tracing/library/test/unit_test/mocks:daemon_communicator_mock",
        "@score-baselibs//score/analysis/tracing/library/test/unit_test/mocks:memory_validator_mock",
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
    name = "generic_trace_api_impl_general_test_suite",
    srcs = [
        "generic_trace_api_impl_general_test_suite.cpp",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    tags = [
        "manual",
        "unit",
    ],
    deps = [
        ":generic_trace_api_impl_test_library",
    ],
)

cc_test(
    name = "generic_trace_api_impl_cached_data_test_suite",
    srcs = [
        "generic_trace_api_impl_cached_data_test_suite.cpp",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    tags = [
        "manual",
        "unit",
    ],
    deps = [
        ":generic_trace_api_impl_test_library",
    ],
)

cc_test(
    name = "generic_trace_api_impl_register_clients_test_suite",
    srcs = [
        "generic_trace_api_impl_register_clients_test_suite.cpp",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    tags = [
        "manual",
        "unit",
    ],
    deps = [
        ":generic_trace_api_impl_test_library",
    ],
)

cc_test(
    name = "generic_trace_api_impl_register_shm_objects_test_suite",
    srcs = [
        "generic_trace_api_impl_register_shm_objects_test_suite.cpp",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    tags = [
        "manual",
        "unit",
    ],
    deps = [
        ":generic_trace_api_impl_test_library",
    ],
)

cc_test(
    name = "generic_trace_api_impl_unregister_shm_objects_test_suite",
    srcs = [
        "generic_trace_api_impl_unregister_shm_objects_test_suite.cpp",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    tags = [
        "manual",
        "unit",
    ],
    deps = [
        ":generic_trace_api_impl_test_library",
    ],
)

cc_test(
    name = "generic_trace_api_impl_register_trace_callback_test_suite",
    srcs = [
        "generic_trace_api_impl_register_trace_callback_test_suite.cpp",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    tags = [
        "manual",
        "unit",
    ],
    deps = [
        ":generic_trace_api_impl_test_library",
    ],
)

cc_test(
    name = "generic_trace_api_impl_trace_test_suite",
    srcs = [
        "generic_trace_api_impl_trace_test_suite.cpp",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    tags = [
        "manual",
        "unit",
    ],
    deps = [
        ":generic_trace_api_impl_test_library",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":generic_trace_api_impl_cached_data_test_suite",
        ":generic_trace_api_impl_general_test_suite",
        ":generic_trace_api_impl_register_clients_test_suite",
        ":generic_trace_api_impl_register_shm_objects_test_suite",
        ":generic_trace_api_impl_register_trace_callback_test_suite",
        ":generic_trace_api_impl_trace_test_suite",
        ":generic_trace_api_impl_unregister_shm_objects_test_suite",
    ],
    visibility = ["@score-baselibs//score/analysis/tracing/library/test/unit_test:__subpackages__"],
)
