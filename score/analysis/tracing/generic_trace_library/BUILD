load("@rules_cc//cc:defs.bzl", "cc_library")
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")
load("@score_baselibs//score/quality/clang_tidy:extra_checks.bzl", "clang_tidy_extra_checks")

alias(
    name = "api",
    actual = "@score_baselibs//score/analysis/tracing/generic_trace_library/interface_types:generic_trace_api",
    visibility = ["//visibility:public"],
)

label_flag(
    name = "implementation",
    build_setting_default = "@score_baselibs//score/analysis/tracing/generic_trace_library/stub_implementation:stub_implementation",
    visibility = ["//visibility:public"],
)

alias(
    name = "mock",
    actual = "@score_baselibs//score/analysis/tracing/generic_trace_library/mock:trace_library_mock",
    visibility = ["//visibility:public"],
)
