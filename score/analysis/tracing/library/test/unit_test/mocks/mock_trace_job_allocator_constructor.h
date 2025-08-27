///
/// @file mock_trace_job_allocator_constructor.h
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief MockTraceJobAllocatorConstructor source file
///

#ifndef GENERIC_TRACE_API_MOCK_TRACE_JOB_ALLOCATOR_CONSTRUCTOR_H
#define GENERIC_TRACE_API_MOCK_TRACE_JOB_ALLOCATOR_CONSTRUCTOR_H

#include "score/analysis/tracing/library/generic_trace_api/trace_job_allocator/trace_job_allocator.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief MockTraceJobAllocatorConstructor class
class MockTraceJobAllocatorConstructor : public TraceJobAllocator
{
  public:
    MockTraceJobAllocatorConstructor() = default;
    virtual ~MockTraceJobAllocatorConstructor() = default;

    MOCK_METHOD(void,
                TraceJobAllocator,
                (TraceJobContainer&,
                 ResourcePointer,
                 ResourcePointer,
                 ShmObjectHandle,
                 std::shared_ptr<IFlexibleCircularAllocator>));
    MOCK_METHOD(void,
                TraceJobAllocator,
                (TraceJobContainer&,
                 ResourcePointer,
                 ResourcePointer,
                 ShmObjectHandle,
                 std::shared_ptr<IFlexibleCircularAllocator>,
                 LoggerTime));
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_MOCK_TRACE_JOB_ALLOCATOR_CONSTRUCTOR_H
