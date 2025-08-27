load("@score-baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")

cc_test(
    name = "atomic_container_test",
    srcs = [
        "atomic_container_test.cpp",
        "common_atomic_container_test.h",
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
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/containers/atomic_container",
        "@score-baselibs//score/mw/log",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "atomic_container_multithreaded_test",
    srcs = [
        "atomic_container_multithreaded_test.cpp",
        "common_atomic_container_test.h",
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
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/containers/atomic_container",
        "@score-baselibs//score/mw/log",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "atomic_container_indirector_test",
    srcs = [
        "atomic_container_indirector_test.cpp",
        "common_atomic_container_test.h",
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
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/containers/atomic_container",
        "@score-baselibs//score/memory/shared:atomic_indirector_mock_binding",
        "@score-baselibs//score/mw/log",
        "@googletest//:gtest_main",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":atomic_container_test",
        ":atomic_container_indirector_test",
        ":atomic_container_multithreaded_test",
    ],
    visibility = ["@score-baselibs//score/analysis/tracing:__subpackages__"],
)
