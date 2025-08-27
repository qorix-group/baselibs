///
/// @file generic_trace_api_impl.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API main implementation header
///

#ifndef GENERIC_TRACE_API_GENERIC_TRACE_API_IMPL_H
#define GENERIC_TRACE_API_GENERIC_TRACE_API_IMPL_H

#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"
#include "score/analysis/tracing/library/generic_trace_api/i_object_factory.h"
#include "score/analysis/tracing/library/generic_trace_api/library_state.h"
#include "score/analysis/tracing/library/generic_trace_api/memory_validator/memory_validator.h"
#include "score/analysis/tracing/library/generic_trace_api/object_factory.h"
#include "score/analysis/tracing/library/interface/i_trace_library.h"
#include "score/analysis/tracing/library/interface/meta_info.h"
#include "score/analysis/tracing/library/interface/meta_info_variants.h"
#include "score/memory/shared/atomic_indirector.h"
#include "score/os/unistd.h"

#include <score/callback.hpp>
#include <score/jthread.hpp>
#include <array>
#include <variant>

#include "score/analysis/tracing/library/generic_trace_api/containers/client_id/client_id_container.h"
#include "score/analysis/tracing/library/generic_trace_api/containers/shm_object_handle/shm_object_handle_container.h"

namespace score
{
namespace analysis
{
namespace tracing
{
namespace detail
{

constexpr std::uint8_t kMaxSizeForTmdSharedMemoryPath = 20U;
// There is no identifier reuse within the same namespace.
// coverity[autosar_cpp14_a2_10_4_violation]
static constexpr auto kDaemonConnectionRetrySleepTime = std::chrono::milliseconds{300};
// No harm from defining static variables here
//   coverity[autosar_cpp14_a3_3_2_violation]
static const std::string kSharedMemoryPath{"/dev_tmd_"};
/// @brief GenericTraceAPIImplInternal class
///
/// Implementation of Generic Trace API functionalities.
template <template <class> class IndirectorType = score::memory::shared::AtomicIndirectorReal>
class GenericTraceAPIImplInternal : public ITraceLibrary
{
    using ITraceLibrary::RegisterShmObject;  // Make all base overloads visible
  public:
    /// @brief Constructor. Create a new GenericTraceAPIImplInternal object.
    ///
    /// @param factory Reference used to create library objects.
    explicit GenericTraceAPIImplInternal(
        std::unique_ptr<IObjectFactory> factory = std::make_unique<ObjectFactory>(),
        std::unique_ptr<score::os::Unistd> unistd_impl = std::make_unique<score::os::internal::UnistdImpl>(),
        std::unique_ptr<IMemoryValidator> memory_validator = std::make_unique<MemoryValidator>(),
        const std::optional<score::cpp::stop_token>& token_test = std::nullopt,
        const std::string shared_memory_path = kSharedMemoryPath,
        std::optional<ErrorCode> global_error = std::nullopt);

    GenericTraceAPIImplInternal(const GenericTraceAPIImplInternal&) = delete;
    GenericTraceAPIImplInternal(const GenericTraceAPIImplInternal&&) = delete;
    GenericTraceAPIImplInternal& operator=(const GenericTraceAPIImplInternal&) = delete;
    GenericTraceAPIImplInternal& operator=(const GenericTraceAPIImplInternal&&) = delete;

    /// @brief Destructor. Destroy GenericTraceAPIImplInternal object.
    ~GenericTraceAPIImplInternal() noexcept override;

    /// @brief Register trace_client_id to the API.
    ///
    /// @param binding_type The binding used by the trace_client_id.
    /// @param app_instance_identifier Description of the trace_client_id.
    ///
    /// @return A TraceClientId or error code if operation was not successful.
    // No harm to declare differnt names in base and derived classes for the same api
    //  coverity[autosar_cpp14_m8_4_2_violation]
    RegisterClientResult RegisterClient(const BindingType binding, const std::string& app_instance_identifier) override;

    /// @brief Register shared-memory object.
    ///
    /// @param trace_client_id Id of the trace client.
    /// @param path Path to the shared-memory object.
    ///
    /// @return A handle to shared-memory object or error code if operation was not successful.
    RegisterSharedMemoryObjectResult RegisterShmObject(const TraceClientId trace_client_id,
                                                       const std::string& shm_object_path) noexcept override;

    /// @brief Register shared-memory object.
    ///
    /// @param trace_client_id Id of the trace client.
    /// @param shm_object_fd Shared-memory object file descriptor.
    ///
    /// @return A handle to shared-memory object or error code if operation was not successful.
    // No harm from using more than overloaded function
    //  coverity[autosar_cpp14_m3_9_1_violation]
    RegisterSharedMemoryObjectResult RegisterShmObject(const TraceClientId trace_client_id,
                                                       const std::int32_t shm_object_fd) noexcept override;

    /// @brief Unregister shared-memory object.
    ///
    /// @param handle Handle to previously registered shared-memory object.
    /// @return Error code if operation was not successful.
    ResultBlank UnregisterShmObject(const TraceClientId trace_client_id, const ShmObjectHandle handle) override;

    /// @brief Register trace call done callback.
    ///
    /// @param trace_client_id Id of the trace client.
    /// @param trace_done_callback TraceDoneCallBackType function used as callback.
    ///
    /// @return An error code in case where register operation was not successful.
    RegisterTraceDoneCallBackResult RegisterTraceDoneCB(const TraceClientId trace_client_id,
                                                        TraceDoneCallBackType trace_done_callback) override;

    /// @brief Trace data placed in shared-memory region.
    ///
    /// @param meta_info Meta info data.
    /// @param data List of data chunks placed in shared-memory region which should be traced.
    /// @param context_id Context id of data used to distinguish it.
    ///
    /// @return A TraceContextId used to distinguish traced data.
    TraceResult Trace(const TraceClientId trace_client_id,
                      const MetaInfoVariants::type& meta_info,
                      ShmDataChunkList& data,
                      TraceContextId context_id) override;

    /// @brief Trace data placed in local memory region.
    ///
    /// @param meta_info Meta info data.
    /// @param data List of data chunks placed in local memory region which should be traced.
    ///             This data will be copied to shared-memory region be GenericTraceAPI.
    ///
    /// @return A TraceContextId used to distinguish traced data.
    TraceResult Trace(const TraceClientId trace_client_id,
                      const MetaInfoVariants::type& meta_info,
                      LocalDataChunkList& data) noexcept override;

  private:
    /// @brief Create Trace Meta Data shared memory per library instance and register it to the daemon map
    ///
    /// @return The memory handle as per daemon map or error in case of any failure
    score::Result<Blank> CreateTraceMetaDataSharedMemory(const std::string shared_memory_path = kSharedMemoryPath);

    /// @brief Unregister Trace Meta Data shared memory and remove it from file system
    ///
    /// @return Error in case of any failure.
    ResultBlank UnregisterAndRemoveTraceMetaDataSharedMemory();

    /// @brief Worker thread method. Used to process trace data.
    ///
    /// @param stop_token Stop token used to stop worker thread
    void WorkerThread(const score::cpp::stop_token& stop_token);

    bool TryDaemonConnection();

    bool IsLibraryReady() const;
    bool IsDaemonReady() const;
    bool WaitForDaemonReady() const;
    bool WaitForInitReady() const;

    ResultBlank RegisterLtpmDaemonClients(const score::cpp::stop_token& stop_token);
    ResultBlank RegisterLtpmDaemonShmObjectHandles(const score::cpp::stop_token& stop_token);

    ResultBlank RegisterLocalClientToLtpmDaemon(ClientIdElement& client_id_element);
    ResultBlank RegisterLocalShmObjectHandleToLtpmDaemon(ShmObjectHandleElement& local_shm_object_handle);

    ResultBlank UnregisterLtpmDaemonShmObject(const ShmObjectHandle handle);

    bool ConnectToDaemon(const score::cpp::stop_token& stop_token);
    bool RegisterLtpmDaemonClientsAndShmObjectHandles(const score::cpp::stop_token& stop_token);
    bool InitializeTraceMetaDataAndAllocator();
    score::Result<Blank> ProcessJobsLoop(const score::cpp::stop_token& stop_token);
    void CleanUpResources();
    void HandleDaemonTermination();

    // Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
    // Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
    // coverity[autosar_cpp14_a15_5_3_violation]
    ResultBlank ValidateClientAndShmObject(const TraceClientId trace_client_id, std::int32_t file_descriptor) noexcept;
    // Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
    // Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
    // coverity[autosar_cpp14_a15_5_3_violation]
    RegisterSharedMemoryObjectResult HandleShmObjectRegistration(std::int32_t shm_object_identifier) noexcept;

    std::unique_ptr<IDaemonCommunicator> daemon_communicator_;  ///< Daemon communicator.
    std::unique_ptr<ITraceJobAllocator> trace_job_allocator_;   ///< Trace job allocator.
    std::unique_ptr<ITraceJobProcessor> trace_job_processor_;   ///< Trace job processor.
    std::atomic<LibraryState> state_;                           ///< State of the instance.
    std::shared_ptr<TraceJobContainer> trace_job_container_;

    std::unique_ptr<score::cpp::jthread> worker_thread_;  ///< State of the instance.
    SharedResourcePointer trace_metadata_memory_resource_;

    ClientIdContainer client_id_container_;
    ShmObjectHandleContainer shm_object_handle_container_;

    std::array<char, kMaxSizeForTmdSharedMemoryPath> trace_meta_data_shared_memory_path_;
    std::unique_ptr<score::os::Unistd> unistd_impl_;
    ShmObjectHandle trace_meta_data_memory_handle_;

    /// @brief Object Factory instance
    std::unique_ptr<IObjectFactory> factory_;
    std::unique_ptr<IMemoryValidator> memory_validator_;

    score::Result<std::unique_ptr<ITraceJobAllocator>> trace_job_allocator_result_;
    score::Result<Blank> create_tmd_result_;

    // To store a fatal error that occurs in the WorkerThread and to ensure that no further
    // operations are performed if a fatal error has already occurred in the WorkerThread.
    std::atomic<std::optional<ErrorCode>> global_error_;
    score::cpp::stop_token stop_token_;
};

}  // namespace detail

using GenericTraceAPIImpl = detail::GenericTraceAPIImplInternal<>;

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_GENERIC_TRACE_API_IMPL_H
