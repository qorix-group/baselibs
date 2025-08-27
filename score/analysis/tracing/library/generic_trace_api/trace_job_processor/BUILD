cc_library(
    name = "trace_job_processor_interface",
    srcs = [
        "i_trace_job_processor.cpp",
    ],
    hdrs = [
        "i_trace_job_processor.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    tags = ["FFI"],
    visibility = ["@score-baselibs//score/analysis/tracing:__subpackages__"],
    deps = [
        "@score-baselibs//score/analysis/tracing/common/interface_types:generic_trace_api_types",
        "@score-baselibs//score/analysis/tracing/common/interface_types:shared_memory_location",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/containers/client_id",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_container",
        "@score-baselibs//score/result",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "trace_job_processor",
    srcs = [
        "trace_job_processor.cpp",
    ],
    hdrs = [
        "trace_job_processor.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    tags = ["FFI"],
    visibility = ["@score-baselibs//score/analysis/tracing:__subpackages__"],
    deps = [
        ":trace_job_processor_interface",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/containers/atomic_ring_buffer",
    ],
)
