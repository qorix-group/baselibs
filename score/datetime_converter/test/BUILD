cc_test(
    name = "DateTimeConverter_test",
    srcs = ["DateTimeConverterTest.cpp"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
    ],
    tags = ["unit"],
    visibility = ["@score-baselibs//score/datetime_converter:__pkg__"],
    deps = [
        "@googletest//:gtest_main",
        "@score-baselibs//score/datetime_converter",
    ],
)
