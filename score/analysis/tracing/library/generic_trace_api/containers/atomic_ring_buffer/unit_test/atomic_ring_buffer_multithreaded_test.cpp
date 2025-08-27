#include "score/analysis/tracing/library/generic_trace_api/containers/atomic_ring_buffer/atomic_ring_buffer.h"

#include <condition_variable>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace score::analysis::tracing;

static constexpr std::uint16_t kBufferSize = 128;
static constexpr std::int32_t kThreadCount = 8;
static constexpr std::int32_t kIterationCount = 100000;

class AtomicRingBufferTest : public ::testing::Test
{
  protected:
    void Producer(std::atomic<bool>& running)
    {
        for (std::int32_t i = 0; i < kIterationCount; ++i)
        {
            auto result = ring_buffer.GetEmptyElement();
            if (result.has_value())
            {
                result.value().get().data_ = 42;
                result.value().get().is_ready_.store(true, std::memory_order_release);
            }
        }
        running.store(false);
    }

    void Consumer(std::atomic<bool>& running)
    {
        for (std::int32_t i = 0; i < kIterationCount; ++i)
        {
            auto result = ring_buffer.GetReadyElement();
            if (result.has_value())
            {
                result.value().get().is_ready_.store(false, std::memory_order_release);
                ring_buffer.ReleaseReadyElement();
            }
        }
        running.store(false);
    }

    using TestRingBuffer = AtomicRingBuffer<std::int32_t, kBufferSize>;
    TestRingBuffer ring_buffer;
};

TEST_F(AtomicRingBufferTest, TestMultipleProducersSingleConsumer)
{
    std::vector<std::thread> threads;
    std::atomic<bool> running(true);
    std::atomic<bool> start_flag(false);
    std::mutex start_mutex;
    std::condition_variable start_cv;
    std::atomic<int> ready_count(0);

    constexpr int kProducerThreads = kThreadCount - 1;
    for (std::int32_t i = 0; i < kProducerThreads; ++i)
    {
        threads.emplace_back([this, &running, &start_flag, &start_mutex, &start_cv, &ready_count]() noexcept {
            {
                std::unique_lock<std::mutex> lock(start_mutex);
                ready_count.fetch_add(1, std::memory_order_relaxed);
                start_cv.wait(lock, [&start_flag] {
                    return start_flag.load();
                });
            }
            Producer(running);
        });
    }

    threads.emplace_back([this, &running, &start_flag, &start_mutex, &start_cv, &ready_count]() noexcept {
        {
            std::unique_lock<std::mutex> lock(start_mutex);
            ready_count.fetch_add(1, std::memory_order_relaxed);
            start_cv.wait(lock, [&start_flag] {
                return start_flag.load();
            });
        }
        Consumer(running);
    });

    // Ensure all threads are ready before starting
    while (ready_count.load(std::memory_order_acquire) < kThreadCount)
    {
        std::this_thread::yield();
    }

    // Start all the threads at once!
    {
        std::lock_guard<std::mutex> lock(start_mutex);
        start_flag.store(true, std::memory_order_release);
    }
    start_cv.notify_all();

    for (auto& thread : threads)
    {
        thread.join();
    }

    EXPECT_GE(ring_buffer.GetEmptyElementsCount(), 0);
    EXPECT_LE(ring_buffer.GetEmptyElementsCount(), kBufferSize);
}
