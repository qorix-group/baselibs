alias(
    name = "require_typed_memory",  # only for the client apps using the GTL, not for internal use
    actual = "@score_baselibs//score/analysis/tracing/daemon/flags:force_typed_memory",
    visibility = ["@score_baselibs//score/analysis/tracing/generic_trace_library:__subpackages__"],
)

config_setting(
    name = "require_typed_memory_enabled",
    constraint_values = [
        "@platforms//os:qnx",
    ],
    flag_values = {
        ":require_typed_memory": "True",
    },
    visibility = ["@score_baselibs//score/analysis/tracing/generic_trace_library:__subpackages__"],
)
