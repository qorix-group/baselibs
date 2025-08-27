cc_library(
    name = "i_trace_job_container",
    hdrs = [
        "i_trace_job_container.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    tags = ["FFI"],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        ":trace_job_container_element",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/containers/atomic_ring_buffer",
    ],
)

cc_library(
    name = "trace_job_container",
    srcs = [
        "trace_job_container.cpp",
    ],
    hdrs = [
        "trace_job_container.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    tags = ["FFI"],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        ":i_trace_job_container",
    ],
)

cc_library(
    name = "trace_job_container_element",
    srcs = [
        "trace_job_container_element.cpp",
    ],
    hdrs = [
        "trace_job_container_element.h",
        "trace_job_type.h",
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
        "@score-baselibs//score/analysis/tracing/shm_ring_buffer:shared_memory_ring_buffer_element",
    ],
)
