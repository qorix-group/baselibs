///
/// @file i_object_factory.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Object factory interface header file
///

#ifndef GENERIC_TRACE_API_I_OBJECT_FACTORY_H
#define GENERIC_TRACE_API_I_OBJECT_FACTORY_H

#include "score/analysis/tracing/common/flexible_circular_allocator/flexible_circular_allocator_interface.h"
#include "score/analysis/tracing/library/generic_trace_api/containers/client_id/client_id_container.h"
#include "score/analysis/tracing/library/generic_trace_api/daemon_communicator/daemon_communicator_factory_interface.h"
#include "score/analysis/tracing/library/generic_trace_api/daemon_communicator/i_daemon_communicator.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_allocator/i_trace_job_allocator.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_container/trace_job_container.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_processor/i_trace_job_processor.h"
#include "score/analysis/tracing/shm_ring_buffer/i_shm_ring_buffer.h"
#include <memory>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief IObjectFactory class
// No need to define the rule of five for that class
// coverity[autosar_cpp14_a12_8_6_violation]
// coverity[autosar_cpp14_a12_0_1_violation]
class IObjectFactory
{
  public:
    /// @brief Method to create DaemonCommunicator instance.
    ///
    /// @return Unique pointer to IDaemonCommunicator.
    // This is intended, we need to use the default argument to ensure testability
    // NOLINTBEGIN(google-default-arguments) see comment above
    virtual std::unique_ptr<IDaemonCommunicator> CreateDaemonCommunicator(
        std::unique_ptr<IDaemonCommunicator> daemon_communicator = nullptr) = 0;
    // NOLINTEND(google-default-arguments) see comment above

    /// @brief Method to create TraceJobAllocator instance.
    ///
    /// @return Unique pointer to ITraceJobAllocator.
    virtual score::Result<std::unique_ptr<ITraceJobAllocator>> CreateTraceJobAllocator(
        std::shared_ptr<TraceJobContainer> container,
        ResourcePointer memory_resource) const = 0;

    /// @brief Method to create TraceJobProcessor instance.
    ///
    /// @return Unique pointer to ITraceJobProcessor.
    virtual std::unique_ptr<ITraceJobProcessor> CreateTraceJobProcessor(const ClientIdContainer& client_id_container,
                                                                        std::shared_ptr<TraceJobContainer> container,
                                                                        TraceJobDeallocator deallocator_function,
                                                                        const score::cpp::stop_token& stop_token) const = 0;

    /// @brief Destructor.
    virtual ~IObjectFactory() = default;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_I_OBJECT_FACTORY_H
