///
/// @file mock_shm_ring_buffer.h
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#ifndef SCORE_ANALYSIS_TRACING_SHM_RING_BUFFER_MOCK_SHM_RING_BUFFER
#define SCORE_ANALYSIS_TRACING_SHM_RING_BUFFER_MOCK_SHM_RING_BUFFER

#include "score/analysis/tracing/shm_ring_buffer/i_shm_ring_buffer.h"
#include "score/analysis/tracing/shm_ring_buffer/shm_data_segment.h"
#include "score/analysis/tracing/shm_ring_buffer/shm_ring_buffer_element.h"
#include "score/result/result.h"
#include <gmock/gmock.h>
#include <stdint.h>
#include <memory>

namespace score
{
namespace analysis
{
namespace tracing
{

using ElementReferenceType = score::Result<std::reference_wrapper<ShmRingBufferElement>>;

class MockShmRingBuffer : public IShmRingBuffer
{
  public:
    explicit MockShmRingBuffer() = default;

    MockShmRingBuffer(const MockShmRingBuffer&) = delete;
    MockShmRingBuffer& operator=(const MockShmRingBuffer&) = delete;
    MockShmRingBuffer(MockShmRingBuffer&&) = delete;
    MockShmRingBuffer& operator=(MockShmRingBuffer&&) = delete;

    MOCK_METHOD(score::Result<Blank>, Open, (), (override));
    MOCK_METHOD(score::Result<Blank>, Create, (), (override));
    MOCK_METHOD(score::Result<Blank>, CreateOrOpen, (bool is_owner), (override));
    MOCK_METHOD(ElementReferenceType, GetEmptyElement, (), (override));
    MOCK_METHOD(ElementReferenceType, GetReadyElement, (), (override));
    MOCK_METHOD(score::Result<std::uint32_t>, GetUseCount, (), (override));
    MOCK_METHOD(bool, IsBufferEmpty, (), (const, override));
    MOCK_METHOD(Result<std::uint16_t>, GetSize, (), (const, override));
    MOCK_METHOD(Result<ShmRingBufferStatistics>, GetStatistics, (), (const, override));
    MOCK_METHOD(void, Close, (), (override));
    MOCK_METHOD(void, Reset, (), (override));
    MOCK_METHOD(void, ResetStatistics, (), (override));
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_RING_BUFFER_H
