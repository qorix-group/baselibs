cc_library(
    name = "trace_library_itf_mock",
    srcs = ["trace_library_itf_mock.cpp"],
    hdrs = ["trace_library_itf_mock.h"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    visibility = [
        "@score-baselibs//score/analysis:__subpackages__",
        "//platform/aas/mw/com:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/analysis/tracing/common/flexible_circular_allocator",
        "@score-baselibs//score/analysis/tracing/common/interface_types:shared_memory_chunk",
        "@score-baselibs//score/analysis/tracing/common/interface_types:shared_memory_location",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/chunk_list",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_container",
        "@score-baselibs//score/analysis/tracing/shm_ring_buffer",
    ],
)
