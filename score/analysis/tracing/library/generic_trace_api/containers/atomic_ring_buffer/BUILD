cc_library(
    name = "atomic_ring_buffer",
    srcs = [
        "atomic_ring_buffer.cpp",
    ],
    hdrs = [
        "atomic_ring_buffer.h",
        "atomic_ring_buffer_state.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    implementation_deps = [
        "@score-baselibs//score/analysis/tracing/common/interface_types:generic_trace_api_types",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
        "@score-baselibs//score/memory/shared:atomic_indirector",
    ],
    tags = ["FFI"],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
)
