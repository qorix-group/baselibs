///
/// @file trace_job_container_test.cpp
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API trace job container test source file
///

#include "score/analysis/tracing/library/generic_trace_api/trace_job_container/trace_job_container.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <thread>

using ::testing::_;
using ::testing::StrictMock;

using namespace score::analysis::tracing;

class TraceJobContainerFixture : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        container_ = std::make_unique<TraceJobContainer>();
        ring_buffer_element_.global_context_id_ = {client_id_, finished_context_id_};
        ring_buffer_element_.chunk_list_ = chunk_list;
        ring_buffer_element_.status_ = TraceJobStatus::kEmpty;
    }

    void TearDown() override
    {
        container_.reset();
    }

    bool AddTraceJob(const TraceContextId job_type);
    void VerifyContainer(const TraceJobContainerElement& container, const TraceContextId context_id);

    std::unique_ptr<TraceJobContainer> container_;
    ShmRingBufferElement ring_buffer_element_{};
    static constexpr TraceClientId client_id_{1};
    static constexpr TraceContextId context_id_{1};
    static constexpr TraceContextId finished_context_id_{0};
    static constexpr ShmObjectHandle object_handle_{1};
    static constexpr std::size_t offset_{0};
    static constexpr SharedMemoryLocation chunk_list{object_handle_, offset_};
    static constexpr std::size_t thread_count = 4;
    ShmObjectHandle memory_handle_{1};
};

bool TraceJobContainerFixture::AddTraceJob(const TraceContextId context_id)
{
    GlobalTraceContextId global_trace_context_id{client_id_, context_id};

    return container_->Add({std::reference_wrapper<ShmRingBufferElement>(ring_buffer_element_),
                            global_trace_context_id,
                            TraceJobType::kLocalJob,
                            chunk_list});
}

void TraceJobContainerFixture::VerifyContainer(const TraceJobContainerElement& container,
                                               const TraceContextId context_id)
{
    ASSERT_EQ(container.original_trace_context_id_.client_id_, client_id_);
    ASSERT_EQ(container.original_trace_context_id_.context_id_, context_id);
    ASSERT_EQ(container.ring_buffer_element_.get().chunk_list_, container.chunk_list_);
    ASSERT_EQ(container.ring_buffer_element_.get().chunk_list_.shm_object_handle_, memory_handle_);
    ASSERT_EQ(container.ring_buffer_element_.get().status_, TraceJobStatus::kEmpty);
}

TEST_F(TraceJobContainerFixture, VerifyParam)
{
    EXPECT_EQ(container_->Size(), container_->GetEmptyElementsCount());
}

TEST_F(TraceJobContainerFixture, VerifyAddAndRemove)
{
    EXPECT_EQ(container_->Size(), container_->GetEmptyElementsCount());
    EXPECT_TRUE(AddTraceJob(0));
    EXPECT_EQ(container_->Size() - 1, container_->GetEmptyElementsCount());
    EXPECT_TRUE(AddTraceJob(0));
    EXPECT_EQ(container_->Size() - 2, container_->GetEmptyElementsCount());
    container_->ReleaseReadyElement();
    EXPECT_EQ(container_->Size() - 1, container_->GetEmptyElementsCount());
    container_->ReleaseReadyElement();
    EXPECT_EQ(container_->Size(), container_->GetEmptyElementsCount());
}

TEST_F(TraceJobContainerFixture, VerifyContents)
{
    EXPECT_EQ(container_->Size(), container_->GetEmptyElementsCount());
    EXPECT_TRUE(AddTraceJob(0));
    EXPECT_EQ(container_->Size() - 1, container_->GetEmptyElementsCount());
    auto container_element = container_->GetReadyElement();
    VerifyContainer(container_element.value().get().data_, 0);
}

TEST_F(TraceJobContainerFixture, VerifyAllElements)
{
    EXPECT_EQ(container_->Size(), container_->GetEmptyElementsCount());
    for (std::size_t i = 0; i < container_->Size(); i++)
    {
        EXPECT_TRUE(AddTraceJob(static_cast<std::uint32_t>(i)));
    }
    EXPECT_EQ(0, container_->GetEmptyElementsCount());
    EXPECT_FALSE(AddTraceJob(0));
    EXPECT_EQ(0, container_->GetEmptyElementsCount());
    for (std::size_t i = 0; i < container_->Size(); i++)
    {
        auto container_element = container_->GetReadyElement();
        VerifyContainer(container_element.value().get().data_, static_cast<std::uint32_t>(i));
        container_->ReleaseReadyElement();
    }
    EXPECT_EQ(container_->Size(), container_->GetEmptyElementsCount());
}

TEST_F(TraceJobContainerFixture, MultithreadAdd)
{
    std::vector<TraceContextId> vector_of_context_ids;
    std::thread producers[thread_count];
    auto producer = [this]() noexcept {
        for (std::size_t i = 0; i < container_->Size() / thread_count; i++)
        {
            EXPECT_TRUE(AddTraceJob(static_cast<std::uint32_t>(i)));
        }
    };

    for (std::size_t i = 0; i < thread_count; i++)
    {
        producers[i] = std::thread{producer};
    }

    for (std::size_t i = 0; i < thread_count; i++)
    {
        producers[i].join();
    }

    EXPECT_EQ(0, container_->GetEmptyElementsCount());
    for (std::size_t i = 0; i < container_->Size(); i++)
    {
        auto container_element = container_->GetReadyElement();
        vector_of_context_ids.emplace_back(
            container_element.value().get().data_.original_trace_context_id_.context_id_);
        container_->ReleaseReadyElement();
    }
    sort(vector_of_context_ids.begin(), vector_of_context_ids.end());
    for (std::size_t i = 0; i < container_->Size() / thread_count; i++)
    {
        for (std::size_t j = 0; j < thread_count; j++)
        {
            EXPECT_EQ(vector_of_context_ids[i * thread_count + j], i);
        }
    }
}
