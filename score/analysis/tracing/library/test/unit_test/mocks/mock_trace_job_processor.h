///
/// @file mock_trace_job_processor.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#ifndef GENERIC_TRACE_API_MOCK_TRACE_JOB_PROCESSOR_H
#define GENERIC_TRACE_API_MOCK_TRACE_JOB_PROCESSOR_H

#include "score/analysis/tracing/library/generic_trace_api/trace_job_processor/i_trace_job_processor.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief MockTraceJobProcessor class
class MockTraceJobProcessor : public ITraceJobProcessor
{
  public:
    ~MockTraceJobProcessor() override = default;
    MOCK_METHOD(score::ResultBlank, ProcessJobs, (), (noexcept, override));
    MOCK_METHOD(score::ResultBlank, CleanPendingJobs, (), (noexcept, override));
    MOCK_METHOD(score::ResultBlank, SaveCallback, (const TraceClientId, TraceDoneCallBackType), (noexcept, override));
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_MOCK_TRACE_JOB_PROCESSOR_H
