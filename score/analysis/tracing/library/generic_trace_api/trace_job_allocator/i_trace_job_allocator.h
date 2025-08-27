///
/// @file i_trace_job_allocator.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#ifndef GENERIC_TRACE_API_I_TRACE_JOB_ALLOCATOR_H
#define GENERIC_TRACE_API_I_TRACE_JOB_ALLOCATOR_H

#include "score/analysis/tracing/common/interface_types/types.h"
#include "score/analysis/tracing/library/generic_trace_api/chunk_list/local_data_chunk_list.h"
#include "score/analysis/tracing/library/generic_trace_api/chunk_list/shm_data_chunk_list.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_container/trace_job_container.h"
#include "score/analysis/tracing/library/interface/meta_info_variants.h"

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief ITraceJobAllocator abstract class.
///
/// Class used to allocate trace jobs.
class ITraceJobAllocator
{
  public:
    /// @brief Destructor.
    virtual ~ITraceJobAllocator() = default;
    ITraceJobAllocator() noexcept = default;
    ITraceJobAllocator(const ITraceJobAllocator&) noexcept = delete;
    ITraceJobAllocator(ITraceJobAllocator&&) noexcept = delete;
    ITraceJobAllocator& operator=(const ITraceJobAllocator&) = delete;
    ITraceJobAllocator& operator=(ITraceJobAllocator&&) = delete;
    /// @brief Trace data placed in local region.
    /// Chunk list may be modified and reused after this method returns.
    ///
    /// @param client id of the client.
    /// @param meta_info Meta info data.
    /// @param data List of data chunks placed in shared-memory region which should be traced.
    /// @param context_id Context id of data used to distinguish it.
    ///
    /// @return An error code in case where trace operation was not successful.
    virtual TraceResult AllocateShmJob(const TraceClientId client,
                                       const MetaInfoVariants::type& meta_info,
                                       const BindingType binding_type,
                                       const AppIdType& app_instance_id,
                                       ShmDataChunkList& data,
                                       TraceContextId context_id) = 0;

    /// @brief Trace data placed in shared-memory region.
    /// Chunk list may be modified and reused after this method returns.
    ///
    /// @param client id of the client.
    /// @param meta_info Meta info data.
    /// @param data List of data chunks placed in local memory region which should be traced.
    ///             This data will be copied to shared-memory region by GenericTraceAPI.
    ///
    /// @return An error code in case where trace operation was not successful.
    virtual TraceResult AllocateLocalJob(const TraceClientId client,
                                         const MetaInfoVariants::type& meta_info,
                                         const BindingType binding_type,
                                         const AppIdType& app_instance_id,
                                         LocalDataChunkList& data) = 0;

    /// @brief Deallocate memory from job.
    /// Method frees previously allocated memory. In case of local job all memory in the referenced chunk list vector is
    /// freed. In case of shm job only memory allocated for timestamp and Trace Meta Data is freed.
    ///
    /// @param trace_job trace job which is completed and its resources should be freed.
    ///
    /// @return An error code in case where trace operation was not successful.
    virtual TraceResult DeallocateJob(const SharedMemoryLocation chunk_list, const TraceJobType job_type) = 0;

    virtual bool IsLoggerTimeAvailable() = 0;
    virtual void SetTraceMetaDataShmObjectHandle(ShmObjectHandle handle) = 0;
    virtual void CloseRingBuffer() noexcept = 0;
    virtual void ResetRingBuffer() noexcept = 0;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_I_TRACE_JOB_ALLOCATOR_H
