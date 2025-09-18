load("@score-baselibs//:bazel/unit_tests.bzl", "cc_gtest_unit_test")
load("@score-baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "datetime_converter",
    srcs = ["DateTimeConverter.cpp"],
    hdrs = ["DateTimeConverter.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "time_conversion",
    srcs = ["time_conversion.cpp"],
    hdrs = ["time_conversion.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["//visibility:public"],
)

cc_gtest_unit_test(
    name = "time_conversion_test",
    srcs = ["time_conversion_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [":time_conversion"],
)

test_suite(
    name = "unit_tests",
    tests = [
        ":time_conversion_test",
        "@score-baselibs//score/datetime_converter/test:DateTimeConverter_test",
    ],
    visibility = ["//visibility:public"],
)
