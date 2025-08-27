cc_library(
    name = "atomic_container",
    srcs = [
        "atomic_container.cpp",
    ],
    hdrs = [
        "atomic_container.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    implementation_deps = [
        "@score-baselibs//score/analysis/tracing/common/interface_types:generic_trace_api_types",
        "@score-baselibs//score/memory/shared:atomic_indirector",
    ],
    tags = ["FFI"],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
)
