cc_binary(
    name = "shm_ring_buffer_producer",
    srcs = ["shm_ring_buffer_producer.cpp"],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        "@score-baselibs//score/analysis/tracing/shm_ring_buffer",
        "@score-baselibs//score/memory/shared:atomic_indirector",
        "@score-baselibs//score/mw/log",
    ],
)

cc_binary(
    name = "shm_ring_buffer_consumer",
    srcs = ["shm_ring_buffer_consumer.cpp"],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
    deps = [
        "@score-baselibs//score/analysis/tracing/shm_ring_buffer",
        "@score-baselibs//score/memory/shared:atomic_indirector",
        "@score-baselibs//score/mw/log",
    ],
)

filegroup(
    name = "config_files",
    srcs = ["logging.json"],
    visibility = ["@score-baselibs//score/analysis:__subpackages__"],
)
