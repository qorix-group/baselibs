load("@score-baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    test_suites_from_sub_packages = [
        "@score-baselibs//score/language/safecpp/aborts_upon_exception:unit_tests",
        "@score-baselibs//score/language/safecpp/safe_math:unit_tests",
        "@score-baselibs//score/language/safecpp/scoped_function:unit_tests",
        "@score-baselibs//score/language/safecpp/test:unit_tests",
    ],
    visibility = [
        "//visibility:public",
    ],
)
