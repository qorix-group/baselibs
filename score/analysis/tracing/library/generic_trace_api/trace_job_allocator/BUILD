cc_library(
    name = "trace_job_allocator_interface",
    srcs = [
        "i_trace_job_allocator.cpp",
    ],
    hdrs = [
        "i_trace_job_allocator.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    tags = ["FUSA"],
    visibility = ["@score-baselibs//score/analysis/tracing:__subpackages__"],
    deps = [
        "@score-baselibs//score/analysis/tracing/common/interface_types:generic_trace_api_types",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/chunk_list",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_container",
        "@score-baselibs//score/analysis/tracing/library/interface:meta_info",
    ],
)

cc_library(
    name = "trace_job_allocator",
    srcs = [
        "trace_job_allocator.cpp",
    ],
    hdrs = [
        "trace_job_allocator.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    tags = ["FUSA"],
    visibility = ["@score-baselibs//score/analysis/tracing:__subpackages__"],
    deps = [
        ":trace_job_allocator_interface",
        "@score-baselibs//score/analysis/tracing/common/interface_types:generic_trace_api_types",
        "@score-baselibs//score/analysis/tracing/common/interface_types:shared_memory_location_helpers",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/chunk_list",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/trace_job_container",
        "@score-baselibs//score/analysis/tracing/library/interface:meta_info",
        "@score-baselibs//score/analysis/tracing/plugin/ipc_trace_plugin/interface:ara_com_meta_info_trace_format",
        "@score-baselibs//score/analysis/tracing/shm_ring_buffer",
        "//platform/aas/mw/time/HWLoggerTime",
        "//platform/aas/mw/time/HighPrecisionLocalSteadyClock",
    ],
)
