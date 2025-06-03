load("@score-baselibs//bazel:unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_test_suite",
    cc_unit_tests = [
        "@score-baselibs//score/static_reflection_with_serialization/serialization:serializer_ut",
        "@score-baselibs//score/static_reflection_with_serialization/serialization:size_visitor_ut",
        "@score-baselibs//score/static_reflection_with_serialization/serialization:visitor_type_traits_ut",
        "@score-baselibs//score/static_reflection_with_serialization/visitor:visitor_ut",
    ],
    visibility = ["@score-baselibs//score/mw/log:__pkg__"],
)
