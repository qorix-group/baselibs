///
/// @file mock_object_factory.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Object factory mock header file
///

#ifndef GENERIC_TRACE_API_MOCK_OBJECT_FACTORY_H
#define GENERIC_TRACE_API_MOCK_OBJECT_FACTORY_H

#include "score/analysis/tracing/library/generic_trace_api/i_object_factory.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_allocator/i_trace_job_allocator.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief MockObjectFactory class
class MockObjectFactory : public IObjectFactory
{
  public:
    ~MockObjectFactory() override = default;
    MOCK_METHOD(std::unique_ptr<IDaemonCommunicator>,
                CreateDaemonCommunicator,
                (std::unique_ptr<IDaemonCommunicator> daemon_communicator),
                (override));
    MOCK_METHOD(score::Result<std::unique_ptr<ITraceJobAllocator>>,
                CreateTraceJobAllocator,
                (std::shared_ptr<TraceJobContainer>, ResourcePointer),
                (const, override));
    MOCK_METHOD(
        std::unique_ptr<ITraceJobProcessor>,
        CreateTraceJobProcessor,
        (const ClientIdContainer&, std::shared_ptr<TraceJobContainer>, TraceJobDeallocator, const score::cpp::stop_token&),
        (const, override));
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_MOCK_OBJECT_FACTORY_H
