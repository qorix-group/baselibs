load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")

bool_flag(
    name = "KRemote_Logging",
    build_setting_default = True,
)

config_setting(
    name = "Remote_Logging",
    flag_values = {
        ":KRemote_Logging": "True",
    },
    visibility = [
        "@score-baselibs//score/mw/log:__subpackages__",
    ],
)
