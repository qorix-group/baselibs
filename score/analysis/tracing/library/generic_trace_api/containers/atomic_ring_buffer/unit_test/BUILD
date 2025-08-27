load("@score-baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")

cc_test(
    name = "atomic_ring_buffer_indirector_test",
    srcs = [
        "atomic_ring_buffer_indirector_test.cpp",
    ],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    tags = ["FFI"],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        "@score-baselibs//score/analysis/tracing/common/interface_types:generic_trace_api_types",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/containers/atomic_ring_buffer",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_container:trace_job_container_element",
        "@score-baselibs//score/memory/shared:atomic_indirector_mock_binding",
        "@score-baselibs//score/mw/log",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "atomic_ring_buffer_multithreaded_test",
    srcs = [
        "atomic_ring_buffer_multithreaded_test.cpp",
    ],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    tags = ["FFI"],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        "@score-baselibs//score/analysis/tracing/common/interface_types:generic_trace_api_types",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/containers/atomic_ring_buffer",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
        "@score-baselibs//score/mw/log",
        "@googletest//:gtest_main",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":atomic_ring_buffer_indirector_test",
        ":atomic_ring_buffer_multithreaded_test",
    ],
    visibility = ["@score-baselibs//score/analysis/tracing:__subpackages__"],
)
