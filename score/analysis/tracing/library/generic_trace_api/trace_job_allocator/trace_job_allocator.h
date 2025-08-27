///
/// @file trace_job_allocator.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#ifndef GENERIC_TRACE_API_TRACE_JOB_ALLOCATOR_H
#define GENERIC_TRACE_API_TRACE_JOB_ALLOCATOR_H

#include "score/analysis/tracing/common/flexible_circular_allocator/flexible_circular_allocator_interface.h"
#include "score/analysis/tracing/common/interface_types/types.h"
#include "score/analysis/tracing/library//interface/meta_info_variants.h"
#include "score/analysis/tracing/library/generic_trace_api/chunk_list/local_data_chunk_list.h"
#include "score/analysis/tracing/library/generic_trace_api/chunk_list/shm_data_chunk_list.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_allocator/i_trace_job_allocator.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_container/trace_job_container.h"
#include "score/analysis/tracing/shm_ring_buffer/i_shm_ring_buffer.h"
#include "score/analysis/tracing/shm_ring_buffer/shm_ring_buffer.h"
#include "platform/aas/mw/time/HWLoggerTime/receiver/eptm_receiver.h"

#include <score/optional.hpp>

namespace score
{
namespace analysis
{
namespace tracing
{

using LoggerTime = score::cpp::optional<std::shared_ptr<score::mw::time::hwloggertime::EptmReceiver>>;
/// @brief TraceJobAllocator class
///
/// Class used to allocate trace jobs
class TraceJobAllocator : public ITraceJobAllocator
{
  public:
    /// @brief Constructor. Create a new TraceJobAllocator object.
    ///
    /// @param container Container to put allocated jobs
    /// @param memory_resource shared pointer with shared-memory resource where chunk list will be placed.
    /// @param handle shared-memory handle assigned by the daemon during region registration
    explicit TraceJobAllocator(std::shared_ptr<TraceJobContainer> container,
                               ResourcePointer tmd_memory_resource,
                               std::shared_ptr<IFlexibleCircularAllocator> flexible_allocator,
                               std::unique_ptr<IShmRingBuffer> ring_buffer,
                               LoggerTime logger_time = score::cpp::nullopt);

    TraceJobAllocator();
    TraceJobAllocator(const TraceJobAllocator&) = delete;
    TraceJobAllocator(TraceJobAllocator&&) = delete;
    TraceJobAllocator& operator=(const TraceJobAllocator&) = delete;
    TraceJobAllocator& operator=(TraceJobAllocator&&) = delete;

    TraceResult AllocateShmJob(const TraceClientId client,
                               const MetaInfoVariants::type& meta_info,
                               const BindingType binding_type,
                               const AppIdType& app_instance_id,
                               ShmDataChunkList& data,
                               TraceContextId context_id) override;

    TraceResult AllocateLocalJob(const TraceClientId client,
                                 const MetaInfoVariants::type& meta_info,
                                 const BindingType binding_type,
                                 const AppIdType& app_instance_id,
                                 LocalDataChunkList& data) override;
    TraceResult DeallocateJob(const SharedMemoryLocation chunk_list, const TraceJobType job_type) override;
    bool IsLoggerTimeAvailable() override;
    void SetTraceMetaDataShmObjectHandle(ShmObjectHandle handle) override;
    /// @brief Default destructor. Destroys a TraceJobAllocator object.
    // No harm to define default destructor
    //  coverity[autosar_cpp14_a10_3_1_violation]
    ~TraceJobAllocator() = default;
    void CloseRingBuffer() noexcept override;
    void ResetRingBuffer() noexcept override;

  private:
    std::uint64_t GetTimeStamp();
    std::shared_ptr<TraceJobContainer> container_;
    ResourcePointer trace_metadata_memory_resource_;
    ShmObjectHandle trace_metadata_memory_handle_;
    std::unique_ptr<IShmRingBuffer> ring_buffer_;
    std::uint32_t job_id_;
    std::shared_ptr<IFlexibleCircularAllocator> flexible_allocator_;
    LoggerTime logger_time_;
    bool logger_time_available_;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_TRACE_JOB_ALLOCATOR_H
