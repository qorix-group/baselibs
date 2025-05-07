load("//score/mw:common_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "slog_backend",
    srcs = [
        "slog_backend.cpp",
    ],
    hdrs = [
        "slog_backend.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "@score-baselibs//score/mw/log:__subpackages__",
        # "@ddad@score-baselibs//score/mw/log:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/os/qnx:slog2",
        "@score-baselibs//score/mw/log/detail:backend_interface",
        "@score-baselibs//score/mw/log/detail:circular_allocator",
        "@score-baselibs//score/mw/log/detail:initialization_reporter",
    ],
)

cc_library(
    name = "slog",
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "@score-baselibs//score/mw/log:__subpackages__",
        # "@ddad@score-baselibs//score/mw/log:__subpackages__",
    ],
    deps = [
        ":slog_backend",
        ":slog_recorder_factory",
    ],
)

cc_library(
    name = "slog_recorder_factory",
    srcs = [
        "slog_recorder_factory.cpp",
    ],
    hdrs = [
        "slog_recorder_factory.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "@score-baselibs//score/mw/log:__subpackages__",
        # "@ddad@score-baselibs//score/mw/log:__subpackages__",
    ],
    deps = [
        ":slog_backend",
        "@score-baselibs//score/mw/log/detail:log_recorder_factory",
        "@score-baselibs//score/mw/log/detail/file_logging:text_recorder",
    ],
)

test_suite(
    name = "unit_tests",
    tests = [],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "slog_backend_test",
    srcs = [
        "slog_backend_test.cpp",
    ],
    features = [
        "aborts_upon_exception",
    ],
    tags = [
        "manual",
        "unit",
    ],
    visibility = [
        # "@ddad@score-baselibs//score/mw/log/detail:__pkg__",
        # "@ddad@score-baselibs//score/mw/log/detail/slog:__pkg__",
    ],
    deps = [
        "@score-baselibs//score/mw/log/configuration",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@score-baselibs//score/language/futurecpp:futurecpp_test_support",
    ] + select({
        "@platforms//os:linux": [],
        "@platforms//os:qnx": [
            ":slog",
            "@score-baselibs//score/os/mocklib/qnx:slog2_mock",
        ],
    }),
)
