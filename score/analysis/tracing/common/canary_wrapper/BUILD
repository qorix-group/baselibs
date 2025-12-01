load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "canary_wrapper",
    hdrs = ["canary_wrapper.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/analysis/tracing:__subpackages__",
    ],
)
