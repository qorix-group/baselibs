FEAT_COMPILER_WARNINGS_AS_ERRORS = [
    "additional_warnings",
    "strict_warnings",
    "treat_warnings_as_errors",
]

cc_library(
    name = "qnx",
    srcs = [
        "qnx_memory_validator.cpp",
    ],
    hdrs = [
        "qnx_memory_validator.h",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "@score-baselibs//score/analysis/tracing/library:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/analysis/tracing/common/interface_types:generic_trace_api_types",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/memory_validator:interface",
        "@score-baselibs//score/os/qnx:mman",
    ],
)
