/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/

#ifndef SCORE_ANALYSIS_TRACING_LIBRARY_ITF_MOCK
#define SCORE_ANALYSIS_TRACING_LIBRARY_ITF_MOCK

#include "score/analysis/tracing/library/interface/generic_trace_api.h"
#include "score/analysis/tracing/library/interface/i_trace_library.h"
#include "score/os/unistd.h"
#include <atomic>
#include <chrono>
namespace score
{
namespace analysis
{
namespace tracing
{

constexpr std::uint8_t kMaxxSizeForTmdSharedMemoryPath = 20U;

class TraceLibraryItfMock : public ITraceLibrary
{
  public:
    TraceLibraryItfMock(
        std::unique_ptr<score::os::Unistd> unistd_impl = std::make_unique<score::os::internal::UnistdImpl>());
    ~TraceLibraryItfMock();
    RegisterClientResult RegisterClient(const BindingType binding, const std::string& app_instance_identifier) override;
    RegisterSharedMemoryObjectResult RegisterShmObject(const TraceClientId trace_client_id,
                                                       const std::string& shm_object_path) override;
    RegisterSharedMemoryObjectResult RegisterShmObject(const TraceClientId trace_client_id,
                                                       std::int32_t shm_object_fd) override;
    ResultBlank UnregisterShmObject(const TraceClientId trace_client_id, ShmObjectHandle handle) override;
    RegisterTraceDoneCallBackResult RegisterTraceDoneCB(const TraceClientId trace_client_id,
                                                        TraceDoneCallBackType trace_done_callback) override;

    TraceResult Trace(const TraceClientId trace_client_id,
                      const MetaInfoVariants::type& meta_info,
                      ShmDataChunkList& data,
                      TraceContextId context_id) override;

    TraceResult Trace(const TraceClientId trace_client_id,
                      const MetaInfoVariants::type& meta_info,
                      LocalDataChunkList& data) noexcept override;

  private:
    /// @brief Create Trace Meta Data shared memory per library instance and register it to the daemon map
    ///
    /// @return The memory handle as per daemon map or error in case of any failure
    RegisterSharedMemoryObjectResult CreateAndRegisterTraceMetaDataSharedMemory();

    /// @brief Unregister Trace Meta Data shared memory and remove it from file system
    ///
    /// @return Error in case of any failure.
    ResultBlank UnregisterAndRemoveTraceMetaDataSharedMemory();

    std::unordered_map<TraceClientId, TraceDoneCallBackType> callback_map_;
    std::unordered_map<TraceClientId, std::optional<std::chrono::time_point<std::chrono::steady_clock>>>
        trace_timing_map_;
    std::atomic<TraceClientId> client_ids;
    std::array<char, kMaxxSizeForTmdSharedMemoryPath> trace_meta_data_shared_memory_path_{};
    SharedResourcePointer trace_metadata_memory_resource_;
    std::unique_ptr<score::os::Unistd> unistd_impl_;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score
#endif  // SCORE_ANALYSIS_TRACING_LIBRARY_ITF_MOCK
