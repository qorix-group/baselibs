///
/// @file mock_trace_job_allocator.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#ifndef GENERIC_TRACE_API_MOCK_TRACE_JOB_ALLOCATOR_H
#define GENERIC_TRACE_API_MOCK_TRACE_JOB_ALLOCATOR_H

#include "score/analysis/tracing/library/generic_trace_api/trace_job_allocator/i_trace_job_allocator.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief MockTraceJobAllocator class
class MockTraceJobAllocator : public ITraceJobAllocator
{
  public:
    MOCK_METHOD(TraceResult,
                AllocateShmJob,
                (const TraceClientId,
                 const MetaInfoVariants::type&,
                 const BindingType,
                 const AppIdType&,
                 ShmDataChunkList&,
                 TraceContextId),
                (override));
    MOCK_METHOD(
        TraceResult,
        AllocateLocalJob,
        (const TraceClientId, const MetaInfoVariants::type&, const BindingType, const AppIdType&, LocalDataChunkList&),
        (override));
    MOCK_METHOD(TraceResult, DeallocateJob, (SharedMemoryLocation, TraceJobType), (override));
    MOCK_METHOD(bool, IsLoggerTimeAvailable, (), (override));
    MOCK_METHOD(void, SetTraceMetaDataShmObjectHandle, (ShmObjectHandle), (override));
    MOCK_METHOD(void, CloseRingBuffer, (), (noexcept, override));
    MOCK_METHOD(void, ResetRingBuffer, (), (noexcept, override));
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_MOCK_TRACE_JOB_ALLOCATOR_H
