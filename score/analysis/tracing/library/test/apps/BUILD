APP_FEATURES = [
    "treat_warnings_as_errors",
    "strict_warnings",
    "additional_warnings",
]

cc_binary(
    name = "tracing",
    srcs = ["tracing_app.cpp"],
    features = APP_FEATURES,
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "@score-baselibs//score/analysis:__subpackages__",
        "//platform/aas/test/analysis/tracing:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/analysis/tracing/common/interface_types:shared_memory_location_helpers",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api",
        "@score-baselibs//score/language/safecpp/scoped_function:move_only_scoped_function",
        "@score-baselibs//score/os:unistd",
        "@score-baselibs//score/mw/log",
    ],
)

cc_binary(
    name = "test_app",
    srcs = ["test_app.cpp"],
    features = APP_FEATURES,
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "@score-baselibs//score/analysis:__subpackages__",
        "//platform/aas/test/analysis/tracing:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api",
    ],
)

cc_binary(
    name = "shm_chunk_list_test_app",
    srcs = ["shm_chunk_list_test_app.cpp"],
    features = APP_FEATURES,
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        "@score-baselibs//score/analysis/tracing/common/interface_types:shared_memory_location_helpers",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/chunk_list",
        "@score-baselibs//score/memory/shared",
        "@score-baselibs//score/mw/log",
    ],
)

cc_binary(
    name = "local_chunk_list_test_app",
    srcs = ["local_chunk_list_test_app.cpp"],
    features = APP_FEATURES,
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        "@score-baselibs//score/analysis/tracing/common/interface_types:shared_memory_location_helpers",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/chunk_list",
        "@score-baselibs//score/memory/shared",
        "@score-baselibs//score/mw/log",
    ],
)

cc_binary(
    name = "register_test_app",
    srcs = ["register_test_app.cpp"],
    features = APP_FEATURES,
    visibility = [
        "@score-baselibs//score/analysis:__subpackages__",
        "//platform/aas/test/analysis/tracing:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api",
        "@score-baselibs//score/mw/log",
    ],
)

filegroup(
    name = "config_files",
    srcs = ["logging.json"],
    visibility = ["//visibility:public"],
)
