cc_library(
    name = "client_id",
    srcs = [
        "client_id_container.cpp",
    ],
    hdrs = [
        "client_id_container.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    tags = ["FFI"],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        ":element",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/containers/atomic_container",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
    ],
)

cc_library(
    name = "element",
    srcs = [
        "client_id_element.cpp",
    ],
    hdrs = [
        "client_id_element.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    tags = ["FFI"],
    deps = [
        "@score-baselibs//score/analysis/tracing/common/interface_types:generic_trace_api_types",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
    ],
)
