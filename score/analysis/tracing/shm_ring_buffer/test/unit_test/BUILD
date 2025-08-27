load("@score-baselibs//third_party/itf:py_unittest_qnx_test.bzl", "py_unittest_qnx_test")

cc_test(
    name = "main_unit_test",
    srcs = [
        "shm_ring_buffer_test.cpp",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    linkopts = ["-latomic"],
    tags = ["unit"],
    visibility = ["@score-baselibs//score/analysis/tracing/shm_ring_buffer:__pkg__"],
    deps = [
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
        "@score-baselibs//score/analysis/tracing/library/test/unit_test:local_memory_resource",
        "@score-baselibs//score/analysis/tracing/shm_ring_buffer",
        "@score-baselibs//score/memory/shared:atomic_indirector",
        "@score-baselibs//score/memory/shared:atomic_indirector_mock_binding",
        "@score-baselibs//score/memory/shared:shared_memory_factory_mock",
        "@score-baselibs//score/memory/shared:shared_memory_resource_heap_allocator_mock",
        "@score-baselibs//score/memory/shared:shared_memory_resource_mock",
        "@score-baselibs//score/memory/shared:types",
        "@score-baselibs//score/os/mocklib:unistd_mock",
        "@score-baselibs//score/mw/log",
        "//third_party/libatomic",
        "@googletest//:gtest_main",
    ],
)

py_unittest_qnx_test(
    name = "unit_tests_qnx",
    test_cases = [
        ":main_unit_test",
    ],
    visibility = ["@score-baselibs//score/analysis/tracing:__subpackages__"],
)
