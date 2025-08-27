///
/// @file atomic_ring_buffer_indirector_test.cpp
/// @copyright Copyright (C) 2025, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief AtomicRingBuffer tests with mocked atomic operations source file
///

#include "score/analysis/tracing/library/generic_trace_api/containers/atomic_ring_buffer/atomic_ring_buffer.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_container/trace_job_container_element.h"
#include "score/memory/shared/atomic_mock.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace score::analysis::tracing;
using namespace score::memory::shared;

using ::testing::_;
using ::testing::AtLeast;
using ::testing::InSequence;
using ::testing::Return;

static constexpr std::size_t kTraceJobContainerSize = 500U;

class AtomicRingBufferTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        atomic_mock_ = std::make_unique<AtomicMock<std::size_t>>();
        atomic_state_mock_ = std::make_unique<AtomicMock<AtomicRingBufferState>>();

        EXPECT_EQ(AtomicIndirectorMock<std::size_t>::GetMockObject(), nullptr);
        AtomicIndirectorMock<std::size_t>::SetMockObject(atomic_mock_.get());
        EXPECT_EQ(AtomicIndirectorMock<std::size_t>::GetMockObject(), atomic_mock_.get());

        EXPECT_EQ(AtomicIndirectorMock<AtomicRingBufferState>::GetMockObject(), nullptr);
        AtomicIndirectorMock<AtomicRingBufferState>::SetMockObject(atomic_state_mock_.get());
        EXPECT_EQ(AtomicIndirectorMock<AtomicRingBufferState>::GetMockObject(), atomic_state_mock_.get());
    }

    void TearDown() override
    {
        AtomicIndirectorMock<std::size_t>::SetMockObject(nullptr);
        EXPECT_EQ(AtomicIndirectorMock<std::size_t>::GetMockObject(), nullptr);

        AtomicIndirectorMock<AtomicRingBufferState>::SetMockObject(nullptr);
        EXPECT_EQ(AtomicIndirectorMock<AtomicRingBufferState>::GetMockObject(), nullptr);
    }

    using MockAtomicContainer =
        AtomicRingBuffer<TraceJobContainerElement, kTraceJobContainerSize, AtomicIndirectorMock>;
    std::unique_ptr<AtomicMock<std::size_t>> atomic_mock_;
    std::unique_ptr<AtomicMock<AtomicRingBufferState>> atomic_state_mock_;
    MockAtomicContainer container;
};

TEST_F(AtomicRingBufferTest, GetEmptyElementMaximumRetries)
{
    AtomicRingBufferState current_state;
    current_state.start = 0U;
    current_state.end = 1U;
    current_state.empty = 0U;

    {
        InSequence sequence;
        for (auto retry = 0U; retry < 10U; ++retry)
        {
            EXPECT_CALL(*atomic_state_mock_, load(_)).Times(1).WillOnce(Return(current_state));
            EXPECT_CALL(*atomic_state_mock_, compare_exchange_strong(_, _, _, _)).Times(1).WillOnce(Return(false));
        }
    }

    const auto element = container.GetEmptyElement();
    EXPECT_FALSE(element.has_value());
    EXPECT_EQ(element.error(), ErrorCode::kAtomicRingBufferMaxRetriesRecoverable);
}

TEST_F(AtomicRingBufferTest, GetReadyElementMaximumRetries)
{
    AtomicRingBufferState current_state;
    current_state.start = 0U;
    current_state.end = 1U;
    current_state.empty = 0U;

    {
        InSequence sequence;
        for (auto retry = 0U; retry < 10U; ++retry)
        {
            EXPECT_CALL(*atomic_state_mock_, load(_)).Times(1).WillOnce(Return(current_state));
        }
    }

    const auto element = container.GetReadyElement();
    EXPECT_FALSE(element.has_value());
    EXPECT_EQ(element.error(), ErrorCode::kAtomicRingBufferMaxRetriesRecoverable);
}

TEST_F(AtomicRingBufferTest, ReleaseReadyElementMaximumRetries)
{
    AtomicRingBufferState current_state;
    current_state.start = 0U;
    current_state.end = 1U;
    current_state.empty = 0U;

    {
        InSequence sequence;
        for (auto retry = 0U; retry < 10U; ++retry)
        {
            EXPECT_CALL(*atomic_state_mock_, load(_)).Times(1).WillOnce(Return(current_state));
            EXPECT_CALL(*atomic_state_mock_, compare_exchange_strong(_, _, _, _)).Times(1).WillOnce(Return(false));
        }
    }
    EXPECT_FALSE(container.ReleaseReadyElement());
}
