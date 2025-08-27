///
/// @file trace_job_container.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Trace joc container mock header file
///

#ifndef GENERIC_TRACE_API_MOCK_TRACE_JOB_CONTAINER_H
#define GENERIC_TRACE_API_MOCK_TRACE_JOB_CONTAINER_H

#include "score/analysis/tracing/library/generic_trace_api/trace_job_container/i_trace_job_container.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief MockTraceJobContainer class
class MockTraceJobContainer : public ITraceJobContainer
{
  public:
    MOCK_METHOD(bool, Add, (TraceJobContainerElement), (override));
    MOCK_METHOD((AtomicRingBuffer<TraceJobContainerElement, kTraceJobContainerSize>::AtomicRingBufferResult),
                GetReadyElement,
                (),
                (noexcept, override));
    MOCK_METHOD(bool, ReleaseReadyElement, (), (noexcept, override));
    MOCK_METHOD(std::size_t, Size, (), (noexcept, override));
    MOCK_METHOD(std::size_t, GetEmptyElementsCount, (), (noexcept, override));
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_MOCK_TRACE_JOB_CONTAINER_H
