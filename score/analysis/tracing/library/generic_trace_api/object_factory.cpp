///
/// @file object_factory.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Object factory source file
///

#include "score/analysis/tracing/library/generic_trace_api/object_factory.h"
#include "score/analysis/tracing/common/flexible_circular_allocator/lockless_flexible_circular_allocator.h"
#include "score/analysis/tracing/common/flexible_circular_allocator/lockless_flexible_circular_allocator_factory.h"
#include "score/analysis/tracing/common/interface_types/types.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_allocator/trace_job_allocator.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_processor/trace_job_processor.h"
#include "score/memory/shared/shared_memory_factory.h"
#include <iostream>
#include <memory>
#include <thread>

namespace score
{
namespace analysis
{
namespace tracing
{

static constexpr std::size_t kFlexibleAllocatorSize = 3000000UL;

ObjectFactory::ObjectFactory(std::unique_ptr<score::mw::time::hwloggertime::EptmReceiver::Factory> logger_time_factory,
                             std::unique_ptr<IDaemonCommunicatorFactory> communicator_factory,
                             std::unique_ptr<IShmRingBuffer> shm_ring_buffer)
    : IObjectFactory(),
      logger_time_factory_{std::move(logger_time_factory)},
      communicator_factory_{std::move(communicator_factory)},
      shm_ring_buffer_(std::move(shm_ring_buffer))
{
    // clang-format off
        // No harm from using this format in std::cout
        // coverity[autosar_cpp14_m8_4_4_violation]
    std::cout << "debug(\"LIB\"): "<< "ObjectFactory::ObjectFactory()" << std::endl;
    // clang-format on
}
// This was intended, using deafult parameter to ensure testability.
// NOLINTBEGIN(google-default-arguments) see comment above
std::unique_ptr<IDaemonCommunicator> ObjectFactory::CreateDaemonCommunicator(
    std::unique_ptr<IDaemonCommunicator> daemon_communicator)
{
    // NOLINTEND(google-default-arguments) see comment above
    return communicator_factory_->CreateDaemonCommunicator(std::move(daemon_communicator));
}

score::Result<std::unique_ptr<ITraceJobAllocator>> ObjectFactory::CreateTraceJobAllocator(
    std::shared_ptr<TraceJobContainer> container,
    ResourcePointer memory_resource) const
{
    // clang-format off
    LocklessFlexibleCircularAllocatorFactory allocator_factory;
    auto memory_block_ptr =
        // No harm from casting void pointer
        // coverity[autosar_cpp14_m5_2_8_violation]
        static_cast<std::uint8_t*>(memory_resource->allocate(kFlexibleAllocatorSize, alignof(std::max_align_t)));

    auto flexible_allocator = allocator_factory.CreateAllocator(memory_block_ptr, kFlexibleAllocatorSize).value();
    if (!shm_ring_buffer_)
    {
        shm_ring_buffer_ = std::make_unique<ShmRingBuffer>(kRingBufferSharedMemoryPath, kNumberOfElements);
        auto result = shm_ring_buffer_->CreateOrOpen(); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) Tolerated.
        if (!result.has_value())
        {
            std::cerr << __func__ << ": " << "Failed to open or create  shared-memory ring buffer"; // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)tolerated
            return score::MakeUnexpected<std::unique_ptr<ITraceJobAllocator>>(result.error());
        }
    }

    auto logger_time = logger_time_factory_->ObtainEptmReceiverTimebase();
    if (logger_time)
    {
        // The init should be successfully returned as the logger time itself exists
        // coverity[autosar_cpp14_a15_0_2_violation]
        // Tooling issue: as reported from quality team that cases where branch coverage is 100% but decision couldn't be analyzed are accepted as deviations
        if (logger_time->Init()) // LCOV_EXCL_BR_LINE see comment above.
        {
            std::cout<< "debug(\"LIB\"): " << std::string((__func__))<< " logger_time initialized";  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) tolerated
            return std::make_unique<TraceJobAllocator>(
                container, memory_resource, flexible_allocator, std::move(shm_ring_buffer_), logger_time);
        }
        else
        {
            std::cout<< "debug(\"LIB\"): " << std::string((__func__))<< " logger_time initialization failed.";  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) tolerated
        }
    }
    else
    {
        std::cout<< "debug(\"LIB\"): " << std::string((__func__))<< " logger_time is nullptr.";  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) tolerated
    }
    std::cerr<< "debug(\"LIB\"): " << std::string((__func__))<< " Could not create EptmReceiver timebase";  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)tolerated
    // clang-format on
    return std::make_unique<TraceJobAllocator>(
        container, memory_resource, flexible_allocator, std::move(shm_ring_buffer_));
}

std::unique_ptr<ITraceJobProcessor> ObjectFactory::CreateTraceJobProcessor(const ClientIdContainer& client_id_container,
                                                                           std::shared_ptr<TraceJobContainer> container,
                                                                           TraceJobDeallocator deallocator_function,
                                                                           const score::cpp::stop_token& stop_token) const
{
    return std::make_unique<TraceJobProcessor<score::memory::shared::AtomicIndirectorReal>>(
        client_id_container, container, std::move(deallocator_function), stop_token);
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
