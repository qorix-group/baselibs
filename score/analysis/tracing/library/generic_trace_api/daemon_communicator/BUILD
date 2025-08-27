alias(
    name = "daemon_communicator",
    actual = select({
        "@platforms//os:qnx": ":impl",
        "//conditions:default": ":linux_stub",
    }),
    visibility = ["@score-baselibs//score/analysis/tracing/library:__subpackages__"],
)

filegroup(
    name = "common_hdrs",
    srcs = [
        "daemon_communicator_factory.h",
    ],
)

cc_library(
    name = "daemon_communicator_interface",
    hdrs = [
        "daemon_communicator_factory_interface.h",
        "i_daemon_communicator.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = ["@score-baselibs//score/analysis/tracing/library:__subpackages__"],
    deps = [
        "@score-baselibs//score/analysis/tracing/common/interface_types:generic_trace_api_types",
    ],
)

cc_library(
    name = "impl",
    srcs = [
        "impl/daemon_communicator.cpp",
        "impl/daemon_communicator_factory.cpp",
    ],
    hdrs = [
        "impl/daemon_communicator.h",
        ":common_hdrs",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FUSA"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "@score-baselibs//score/analysis/tracing/library:__subpackages__",
        "@score-baselibs//score/analysis/tracing/plugin/ipc_trace_plugin/ipc_trace/test/itf:__pkg__",
    ],
    deps = [
        ":daemon_communicator_interface",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
        "@score-baselibs//score/analysis/tracing/plugin/ipc_trace_plugin/interface:daemon_communicator_request",
        "@score-baselibs//score/analysis/tracing/plugin/ipc_trace_plugin/interface:daemon_communicator_response",
        "@score-baselibs//score/os/qnx:channel",
        "@score-baselibs//score/os/qnx:dispatch",
        "@score-baselibs//score/os/qnx:mman",
        "@score-baselibs//score/os/qnx:neutrino",
        "@score-baselibs//score/os/qnx:sigevent_qnx",
        "@score-baselibs//score/os/utils:thread",
    ],
)

cc_library(
    name = "linux_stub",
    srcs = [
        "stub/daemon_communicator.cpp",
        "stub/daemon_communicator_factory.cpp",
    ],
    hdrs = [
        "stub/daemon_communicator.h",
        ":common_hdrs",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = ["@score-baselibs//score/analysis/tracing/library:__subpackages__"],
    deps = [
        ":daemon_communicator_interface",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
    ],
)
