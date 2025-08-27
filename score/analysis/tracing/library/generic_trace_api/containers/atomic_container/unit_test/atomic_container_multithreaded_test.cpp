///
/// @file atomic_container_multithreaded_test.cpp
/// @copyright Copyright (C) 2025, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief AtomicContainer multithreaded tests source file
///

#include "score/analysis/tracing/library/generic_trace_api/containers/atomic_container/unit_test/common_atomic_container_test.h"

#include <atomic>
#include <chrono>
#include <random>
#include <thread>
#include <vector>

TEST_F(AtomicContainerTest, MassAcquireAndRelease)
{
    std::vector<std::thread> threads;
    std::atomic<int> acquire_count = 0;
    std::atomic<int> release_count = 0;

    auto worker = [&]() noexcept {
        for (std::size_t i = 0; i < kOperationsPerThread; ++i)
        {
            auto element = container.Acquire();
            if (element.has_value())
            {
                acquire_count++;
                container.Release(element.value());
                release_count++;
            }
        }
    };

    for (std::size_t i = 0; i < kThreadCount; ++i)
    {
        threads.emplace_back(worker);
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    EXPECT_EQ(container.GetEmptyElementsCount(), kContainerSize);
    EXPECT_EQ(acquire_count, release_count);
}

TEST_F(AtomicContainerTest, RapidAcquireReleaseLoop)
{
    std::vector<std::thread> threads;
    std::atomic<std::size_t> threads_ready = 0;

    auto worker = [&]() {
        threads_ready.fetch_add(1, std::memory_order_relaxed);
        while (threads_ready.load() < kThreadCount)
        {
        }  // Wait for all threads to be ready

        for (size_t i = 0; i < kOperationsPerThread; ++i)
        {
            auto element = container.Acquire();
            if (element.has_value())
            {
                container.Release(element.value());
            }
        }
    };

    for (std::size_t i = 0; i < kThreadCount; ++i)
    {
        threads.emplace_back(worker);
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    EXPECT_EQ(container.GetEmptyElementsCount(), kContainerSize);
}

TEST_F(AtomicContainerTest, FullContainerHandling)
{
    std::vector<std::optional<std::reference_wrapper<TestData>>> acquired_elements;

    for (std::size_t i = 0; i < kContainerSize; ++i)
    {
        auto element = container.Acquire();
        ASSERT_TRUE(element.has_value());
        acquired_elements.push_back(element);
    }

    EXPECT_EQ(container.GetEmptyElementsCount(), 0);

    auto extra_element = container.Acquire();
    EXPECT_FALSE(extra_element.has_value());

    for (auto& elem : acquired_elements)
    {
        container.Release(elem.value());
    }

    EXPECT_EQ(container.GetEmptyElementsCount(), kContainerSize);
}

TEST_F(AtomicContainerTest, ConcurrentFindIf)
{
    std::vector<std::thread> threads;
    std::atomic<int> found_count = 0;

    // Fill half of the container with TestData{42}
    for (std::size_t i = 0; i < kContainerSize / 2; ++i)
    {
        auto element = container.Acquire();
        ASSERT_TRUE(element.has_value());
        element.value().get().value = 42;
    }

    auto worker = [&]() noexcept {
        for (std::size_t i = 0; i < kOperationsPerThread; ++i)
        {
            auto result = container.FindIf([](const TestData& data) {
                return data.value == 42;
            });
            if (result.has_value())
            {
                found_count++;
            }
        }
    };

    for (std::size_t i = 0; i < kThreadCount; ++i)
    {
        threads.emplace_back(worker);
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    EXPECT_GT(found_count, 0);  // Some elements should have been found
}

TEST_F(AtomicContainerTest, ConcurrentReleaseByValue)
{
    std::vector<std::thread> threads;
    std::vector<TestData> stored_data;

    // Acquire and store references
    for (std::size_t i = 0; i < kContainerSize; ++i)
    {
        auto element = container.Acquire();
        ASSERT_TRUE(element.has_value());
        element.value().get().value = i;
        stored_data.push_back(element.value());
    }

    auto worker = [&](std::size_t start_idx) noexcept {
        for (std::size_t i = start_idx; i < kContainerSize; i += kThreadCount)
        {
            container.Release(stored_data[i]);
        }
    };

    for (std::size_t i = 0; i < kThreadCount; ++i)
    {
        threads.emplace_back(worker, i);
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    EXPECT_EQ(container.GetEmptyElementsCount(), kContainerSize);
}

TEST_F(AtomicContainerTest, ConcurrentReleaseByIndex)
{
    std::vector<std::thread> threads;
    std::vector<std::size_t> indexes;

    // Acquire all elements
    for (std::size_t i = 0; i < kContainerSize; ++i)
    {
        auto element = container.Acquire();
        ASSERT_TRUE(element.has_value());
        indexes.push_back(i);
    }

    auto worker = [&](std::size_t start_idx) noexcept {
        for (std::size_t i = start_idx; i < kContainerSize; i += kThreadCount)
        {
            container.Release(indexes[i]);
        }
    };

    for (std::size_t i = 0; i < kThreadCount; ++i)
    {
        threads.emplace_back(worker, i);
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    EXPECT_EQ(container.GetEmptyElementsCount(), kContainerSize);
}

TEST_F(AtomicContainerTest, RepeatedAcquireReleaseSameSlot)
{
    for (std::size_t i = 0; i < kOperationsPerThread; ++i)
    {
        auto element = container.Acquire();
        ASSERT_TRUE(element.has_value());
        container.Release(element.value());
    }
    EXPECT_EQ(container.GetEmptyElementsCount(), kContainerSize);
}

TEST_F(AtomicContainerTest, RandomizedAcquireRelease)
{
    std::vector<std::thread> threads;
    auto worker = [&]() noexcept {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, 1);

        for (std::size_t i = 0; i < kOperationsPerThread; ++i)
        {
            if (dist(gen))  // Randomly choose to acquire or release
            {
                auto element = container.Acquire();
                if (element.has_value())
                {
                    container.Release(element.value());
                }
            }
        }
    };

    for (std::size_t i = 0; i < kThreadCount; ++i)
    {
        threads.emplace_back(worker);
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    EXPECT_EQ(container.GetEmptyElementsCount(), kContainerSize);
}

TEST_F(AtomicContainerTest, OverflowAcquire)
{
    std::vector<std::optional<std::reference_wrapper<TestData>>> acquired_elements;

    for (std::size_t i = 0; i < kContainerSize; ++i)
    {
        auto element = container.Acquire();
        ASSERT_TRUE(element.has_value());
        acquired_elements.push_back(element);
    }

    auto extra_element = container.Acquire();
    EXPECT_FALSE(extra_element.has_value());

    for (auto& elem : acquired_elements)
    {
        container.Release(elem.value());
    }

    EXPECT_EQ(container.GetEmptyElementsCount(), kContainerSize);
}

TEST_F(AtomicContainerTest, ConcurrentFindIfAcquireRelease)
{
    std::vector<std::thread> threads;
    std::atomic<int> found_count = 0;

    // Fill half the container
    for (std::size_t i = 0; i < kContainerSize / 2; ++i)
    {
        auto element = container.Acquire();
        ASSERT_TRUE(element.has_value());
        element.value().get().value = 42;
    }

    auto find_worker = [&]() noexcept {
        for (std::size_t i = 0; i < kOperationsPerThread; ++i)
        {
            auto result = container.FindIf([](const TestData& data) {
                return data.value == 42;
            });
            if (result.has_value())
            {
                found_count++;
            }
        }
    };

    auto acquire_release_worker = [&]() noexcept {
        for (std::size_t i = 0; i < kOperationsPerThread; ++i)
        {
            auto element = container.Acquire();
            if (element.has_value())
            {
                container.Release(element.value());
            }
        }
    };

    for (std::size_t i = 0; i < kThreadCount / 2; ++i)
    {
        threads.emplace_back(find_worker);
        threads.emplace_back(acquire_release_worker);
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    EXPECT_GT(found_count, 0);
}
