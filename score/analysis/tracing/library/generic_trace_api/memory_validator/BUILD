FEAT_COMPILER_WARNINGS_AS_ERRORS = [
    "additional_warnings",
    "strict_warnings",
    "treat_warnings_as_errors",
]

cc_library(
    name = "interface",
    hdrs = [
        "i_memory_validator.h",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    tags = ["FFI"],
    visibility = [
        "@score-baselibs//score/analysis/tracing/library:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/result",
    ],
)

cc_library(
    name = "memory_validator",
    hdrs = [
        "memory_validator.h",
    ],
    features = FEAT_COMPILER_WARNINGS_AS_ERRORS,
    tags = ["FFI"],
    visibility = [
        "@score-baselibs//score/analysis/tracing/library:__subpackages__",
    ],
    deps = select({
        "@platforms//os:qnx": ["@score-baselibs//score/analysis/tracing/library/generic_trace_api/memory_validator/qnx"],
        "//conditions:default": ["@score-baselibs//score/analysis/tracing/library/generic_trace_api/memory_validator/linux"],
    }) + [
        ":interface",
    ],
)
