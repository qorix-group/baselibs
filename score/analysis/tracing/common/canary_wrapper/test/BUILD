load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_test(
    name = "canary_wrapper_test",
    srcs = [
        "canary_wrapper_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/analysis/tracing/common/canary_wrapper",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":canary_wrapper_test",
    ],
    visibility = ["@score_baselibs//score/analysis/tracing:__subpackages__"],
)
