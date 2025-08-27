cc_library(
    name = "trace_job_container_mock",
    testonly = True,
    srcs = [
        "mock_trace_job_container.cpp",
    ],
    hdrs = [
        "mock_trace_job_container.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_container:i_trace_job_container",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "daemon_communicator_factory_mock",
    testonly = True,
    srcs = [
        "mock_daemon_communicator_factory.cpp",
    ],
    hdrs = [
        "mock_daemon_communicator_factory.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/daemon_communicator:daemon_communicator_interface",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "memory_validator_mock",
    testonly = True,
    srcs = [
        "mock_memory_validator.cpp",
    ],
    hdrs = [
        "mock_memory_validator.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/memory_validator:interface",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "daemon_communicator_mock",
    testonly = True,
    srcs = [
        "mock_daemon_communicator.cpp",
    ],
    hdrs = [
        "mock_daemon_communicator.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/daemon_communicator",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/daemon_communicator:daemon_communicator_interface",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "object_factory_mock",
    testonly = True,
    srcs = [
        "mock_object_factory.cpp",
    ],
    hdrs = [
        "mock_object_factory.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api:object_factory_interface",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "trace_job_allocator_mock",
    srcs = ["mock_trace_job_allocator.cpp"],
    hdrs = ["mock_trace_job_allocator.h"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_allocator:trace_job_allocator_interface",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "mock_trace_job_allocator_constructor",
    srcs = ["mock_trace_job_allocator_constructor.cpp"],
    hdrs = ["mock_trace_job_allocator_constructor.h"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_allocator",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "trace_job_processor_mock",
    srcs = ["mock_trace_job_processor.cpp"],
    hdrs = ["mock_trace_job_processor.h"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_processor:trace_job_processor_interface",
        "@googletest//:gtest_main",
    ],
)
