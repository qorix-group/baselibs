load("//platform/aas/bazel/packaging:pkg_adaptive_app.bzl", "pkg_adaptive_app")

test_suite(
    name = "unit_tests",
    tests = [
        "@score-baselibs//score/analysis/tracing/shm_ring_buffer/test/unit_test:main_unit_test",
    ],
    visibility = ["@score-baselibs//score/analysis/tracing:__pkg__"],
)

cc_library(
    name = "shm_ring_buffer",
    srcs = [
        "shm_ring_buffer.cpp",
    ],
    hdrs = [
        "i_shm_ring_buffer.h",
        "shm_data_segment.h",
        "shm_ring_buffer.h",
        "shm_ring_buffer_state.h",
        "shm_ring_buffer_statistics.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        ":shared_memory_ring_buffer_element",
        ":trace_job_status",
        "@score-baselibs//score/analysis/tracing/common/interface_types:generic_trace_api_types",
        "@score-baselibs//score/analysis/tracing/common/interface_types:shared_memory_location",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
        "@score-baselibs//score/memory/shared",
        "@score-baselibs//score/memory/shared:types",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "mock_shm_ring_buffer",
    srcs = [
    ],
    hdrs = [
        "i_shm_ring_buffer.h",
        "mock_shm_ring_buffer.h",
        "shm_data_segment.h",
        "shm_ring_buffer_state.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        ":shared_memory_ring_buffer_element",
        ":trace_job_status",
        "@score-baselibs//score/analysis/tracing/common/interface_types:generic_trace_api_types",
        "@score-baselibs//score/analysis/tracing/common/interface_types:shared_memory_location",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
        "@score-baselibs//score/memory/shared",
        "@score-baselibs//score/memory/shared:types",
        "@googletest//:gtest_main",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "shared_memory_ring_buffer_element",
    srcs = [
        "shm_ring_buffer_element.cpp",
    ],
    hdrs = [
        "shm_ring_buffer_element.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        ":trace_job_status",
        "@score-baselibs//score/analysis/tracing/common/interface_types:generic_trace_api_types",
        "@score-baselibs//score/analysis/tracing/common/interface_types:shared_memory_location",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "trace_job_status",
    srcs = [
        "trace_job_status.cpp",
    ],
    hdrs = [
        "trace_job_status.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        "@score-baselibs//score/analysis/tracing/common/interface_types:generic_trace_api_types",
        "@score-baselibs//score/analysis/tracing/common/interface_types:shared_memory_location",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
        "@score-baselibs//score/language/futurecpp",
    ],
)

# TODO Test apps should be transformed into SCTF/ITF tests later
pkg_adaptive_app(
    name = "shm_test_apps",
    app_name = "shm_test_apps",
    bin = [
        "@score-baselibs//score/analysis/tracing/shm_ring_buffer/test/apps:shm_ring_buffer_producer",
        "@score-baselibs//score/analysis/tracing/shm_ring_buffer/test/apps:shm_ring_buffer_consumer",
    ],
    etc = [
        "@score-baselibs//score/analysis/tracing/shm_ring_buffer/test/apps:config_files",
    ],
    visibility = [
        "//ecu/xpad/xpad-shared/packaging/ipnext/isoc/swe/swfl_apps:__subpackages__",
        # "@ddad//ecu/xpad/xpad-shared/packaging/ipnext/isoc/swe/swfl_apps:__subpackages__",
    ],
)
