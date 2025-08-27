///
/// @file object_factory.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Object factory header file
///

#ifndef GENERIC_TRACE_API_OBJECT_FACTORY_H
#define GENERIC_TRACE_API_OBJECT_FACTORY_H

#include "score/analysis/tracing/common/flexible_circular_allocator/flexible_circular_allocator_interface.h"
#include "score/analysis/tracing/library/generic_trace_api/daemon_communicator/daemon_communicator_factory.h"
#include "score/analysis/tracing/library/generic_trace_api/i_object_factory.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_allocator/i_trace_job_allocator.h"
#include "platform/aas/mw/time/HWLoggerTime/receiver/details/factory_impl.h"
#include <cstdint>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief ObjectFactory class
class ObjectFactory : public IObjectFactory
{
  public:
    /// @brief Constructor. Create a new ObjectFactory object.
    explicit ObjectFactory(std::unique_ptr<score::mw::time::hwloggertime::EptmReceiver::Factory> logger_time_factory =
                               std::make_unique<score::mw::time::hwloggertime::EptmReceiver::FactoryImpl>(),
                           std::unique_ptr<IDaemonCommunicatorFactory> communicator_factory =
                               std::make_unique<DaemonCommunicatorFactory>(),
                           std::unique_ptr<IShmRingBuffer> shm_ring_buffer = nullptr);

    /// @brief Destructor. Destroy ObjectFactory object.
    // No harm to define default destructor
    //  coverity[autosar_cpp14_a10_3_1_violation]
    ~ObjectFactory() = default;
    ObjectFactory(const ObjectFactory&) = delete;
    ObjectFactory& operator=(const ObjectFactory&) = delete;
    ObjectFactory(ObjectFactory&&) = delete;
    ObjectFactory& operator=(ObjectFactory&&) = delete;

    /// @brief Method to create DaemonCommunicator instance.
    ///
    /// @return Unique pointer to IDaemonCommunicator.
    // This was intended, using deafult parameter to ensure testability.
    // NOLINTBEGIN(google-default-arguments) see comment above
    std::unique_ptr<IDaemonCommunicator> CreateDaemonCommunicator(
        // This is false positive, as the default arguments are the same.
        // coverity[autosar_cpp14_m8_3_1_violation]
        std::unique_ptr<IDaemonCommunicator> daemon_communicator = nullptr) override;
    // NOLINTEND(google-default-arguments) see comment above

    /// @brief Method to create TraceJobAllocator instance.
    ///
    /// @return Unique pointer to ITraceJobAllocator.
    score::Result<std::unique_ptr<ITraceJobAllocator>> CreateTraceJobAllocator(
        std::shared_ptr<TraceJobContainer> container,
        ResourcePointer memory_resource) const override;

    /// @brief Method to create TraceJobProcessor instance.
    ///
    /// @return Unique pointer to ITraceJobProcessor.
    std::unique_ptr<ITraceJobProcessor> CreateTraceJobProcessor(const ClientIdContainer& client_id_container,
                                                                std::shared_ptr<TraceJobContainer> container,
                                                                TraceJobDeallocator deallocator_function,
                                                                const score::cpp::stop_token& stop_token) const override;

  private:
    std::unique_ptr<score::mw::time::hwloggertime::EptmReceiver::Factory> logger_time_factory_;
    std::unique_ptr<IDaemonCommunicatorFactory> communicator_factory_;
    mutable std::unique_ptr<IShmRingBuffer> shm_ring_buffer_;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_OBJECT_FACTORY_H
