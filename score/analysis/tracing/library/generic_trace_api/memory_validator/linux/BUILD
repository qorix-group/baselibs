FEAT_COMPILER_WARNINGS_AS_ERRORS = [
    "additional_warnings",
    "strict_warnings",
    "treat_warnings_as_errors",
]

cc_library(
    name = "linux",
    srcs = [
        "linux_memory_validator.cpp",
    ],
    hdrs = [
        "linux_memory_validator.h",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    visibility = [
        "@score-baselibs//score/analysis/tracing/library:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/memory_validator:interface",
    ],
)
