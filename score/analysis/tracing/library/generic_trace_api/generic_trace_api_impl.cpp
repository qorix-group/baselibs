///
/// @file generic_trace_api_impl.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API main implementation file
///

#include "score/analysis/tracing/library/generic_trace_api/generic_trace_api_impl.h"
#include "score/analysis/tracing/library/generic_trace_api/object_factory.h"
#include "score/concurrency/interruptible_wait.h"
#include "score/memory/shared/shared_memory_factory.h"
#include "score/os/utils/thread.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <thread>

namespace score
{
namespace analysis
{
namespace tracing
{
namespace detail
{
namespace
{
static constexpr auto kWorkerThreadPeriodicity = std::chrono::milliseconds{40};
static constexpr auto kTmdSharedMemorySize = 3145728U;
}  // namespace
template <template <class> class IndirectorType>
GenericTraceAPIImplInternal<IndirectorType>::GenericTraceAPIImplInternal(
    std::unique_ptr<IObjectFactory> factory,
    std::unique_ptr<os::Unistd> unistd_impl,
    std::unique_ptr<IMemoryValidator> memory_validator,
    const std::optional<score::cpp::stop_token>& token_test,
    const std::string shared_memory_path,
    std::optional<ErrorCode> global_error)
    : ITraceLibrary(),
      daemon_communicator_{nullptr},
      state_{LibraryState::kNotInitialized},
      worker_thread_{nullptr},
      trace_meta_data_shared_memory_path_{},
      unistd_impl_{std::move(unistd_impl)},
      trace_meta_data_memory_handle_{0},
      factory_{std::move(factory)},
      memory_validator_{std::move(memory_validator)},
      global_error_{global_error},
      stop_token_{}
{
    static_assert(std::atomic<std::optional<ErrorCode>>::is_always_lock_free, "is not lock-free!");
    // No harm from calling the function in that format
    //  coverity[autosar_cpp14_m8_4_4_violation]
    std::cout << "debug(\"LIB\"): GenericTraceAPIImplInternal::GenericTraceAPIImplInternal" << std::endl;
    score::cpp::ignore = memory::shared::MemoryResourceRegistry::getInstance();
    trace_job_container_ = std::make_shared<TraceJobContainer>();
    trace_job_processor_ = factory_->CreateTraceJobProcessor(
        client_id_container_,
        trace_job_container_,
        [this](SharedMemoryLocation chunk_list, TraceJobType job_type) noexcept -> TraceResult {
            if (trace_job_allocator_ != nullptr)
            {
                // Suppress "AUTOSAR C++14 A15-4-2" rule findings. This rule states: "Throwing an exception in a
                // "noexcept" function." In this case it is ok, because the system anyways forces the process to
                // terminate if an exception is thrown.
                // coverity[autosar_cpp14_a15_4_2_violation]
                return trace_job_allocator_->DeallocateJob(chunk_list, job_type);
            }
            return MakeUnexpected(ErrorCode::kInvalidArgumentFatal);
        },
        stop_token_);

    daemon_communicator_ = factory_->CreateDaemonCommunicator();
    daemon_communicator_->SubscribeToDaemonTerminationNotification([this]() {
        state_ = LibraryState::kDaemonDisconnected;
    });

    create_tmd_result_ = CreateTraceMetaDataSharedMemory(shared_memory_path);

    trace_job_allocator_result_ =
        factory_->CreateTraceJobAllocator(trace_job_container_, trace_metadata_memory_resource_);
    // No harm from ignorning the returned value here
    //   coverity[autosar_cpp14_a0_1_2_violation]
    worker_thread_ = std::make_unique<score::cpp::jthread>([this, token_test](const score::cpp::stop_token& token) -> void {
        stop_token_ = token_test.has_value() ? token_test.value() : token;
        WorkerThread(stop_token_);
    });

    os::set_thread_name(*worker_thread_, "TracingWorkerThread");
}

// Suppress "AUTOSAR C++14 A15-5-3" rule finding. This rule states: "The std::terminate()
// function shall not be called implicitly"
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// By design, thread is joined() in destructor, and there is no termination API.
// This violation concerns throw exception by join(), and the need to have destructor as noexcept.
template <template <class> class IndirectorType>
// coverity[autosar_cpp14_a15_5_1_violation]
// coverity[autosar_cpp14_a15_5_3_violation]
GenericTraceAPIImplInternal<IndirectorType>::~GenericTraceAPIImplInternal() noexcept
{
    // No harm from ignorning the returned value here
    //   coverity[autosar_cpp14_a0_1_2_violation]
    worker_thread_->request_stop();
    // Not testable by unit tests since worker thread is already created in constructor and it's an active thread so to
    // cover negative case that thread is not joinable we shall deactivate the thread and this is not feasible
    if (worker_thread_->joinable())  // LCOV_EXCL_BR_LINE not testable see comment above.
    {
        // No harm from ignorning the exception here
        //   coverity[autosar_cpp14_a15_4_2_violation]
        worker_thread_->join();
    }
    worker_thread_.reset();
}
// clang-format off

template <template <class> class IndirectorType>
RegisterClientResult GenericTraceAPIImplInternal<IndirectorType>::RegisterClient(const BindingType binding,
                                                         const std::string& app_instance_identifier)
{
    auto loaded_error = IndirectorType<std::optional<ErrorCode>>::load(global_error_, std::memory_order_acquire);
    if (loaded_error.has_value())
    {
        return MakeUnexpected(loaded_error.value());
    }

    if ((app_instance_identifier.empty()) || (binding >= BindingType::kUndefined))
    {
        //No harm from calling the function in that format
        //  coverity[autosar_cpp14_m8_4_4_violation]
        std::cerr << "error(\"LIB\"): GenericTraceAPIImplInternal::RegisterClient: Invalid argument: app_instance_identifier" << std::endl;
        return MakeUnexpected(ErrorCode::kInvalidArgumentFatal);
    }
                //No harm from calling the function in that format
                //  coverity[autosar_cpp14_m8_4_4_violation]
    std::cout << "debug(\"LIB\"): GenericTraceAPIImplInternal::RegisterClient: " << app_instance_identifier << std::endl;
    // truncate the first 8 characters of the given app id
    AppIdType truncated_app_identifier{};
    // fill white spaces as a default value for the eight characters
    truncated_app_identifier.fill('\0');

    if (app_instance_identifier.size() >= kApplicationIdentifierLength)
    {
        std::ignore = std::copy(app_instance_identifier.cbegin(),
                  app_instance_identifier.cbegin() + static_cast<std::ptrdiff_t>(kApplicationIdentifierLength),
                  truncated_app_identifier.begin());
    }
    else
    {
        std::ignore = std::copy(app_instance_identifier.cbegin(), app_instance_identifier.cend(), truncated_app_identifier.begin());
    }

    auto client_id = client_id_container_.GetTraceClientId(binding, truncated_app_identifier);
    if (client_id.has_value())
    {
        std::cout << "GenericTraceAPIImplInternal::RegisterClient client is already registered" << std::endl;
        return client_id.value();
    }
    else {
        std::cout << "GenericTraceAPIImplInternal::RegisterClient: new client = " << app_instance_identifier << std::endl;
    }
    auto register_client_result = client_id_container_.RegisterLocalTraceClient(binding, truncated_app_identifier);
    if (!register_client_result.has_value())
    {
        return MakeUnexpected<TraceClientId>(register_client_result.error());
    }
    if (IsDaemonReady())
    {
        const auto register_ltpm_daemon_client_result = RegisterLocalClientToLtpmDaemon(register_client_result.value().get());
        if (!register_ltpm_daemon_client_result.has_value())
        {
            return MakeUnexpected<TraceClientId>(register_ltpm_daemon_client_result.error());
        }
    }
    return register_client_result.value().get().local_client_id_;
}
// clang-format on

template <template <class> class IndirectorType>
bool GenericTraceAPIImplInternal<IndirectorType>::IsDaemonReady() const
{
    return ((state_ == LibraryState::kDaemonInitialized) || (IsLibraryReady()));
}

template <template <class> class IndirectorType>
bool GenericTraceAPIImplInternal<IndirectorType>::IsLibraryReady() const
{
    return state_ == LibraryState::kInitialized;
}

template <template <class> class IndirectorType>
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
ResultBlank GenericTraceAPIImplInternal<IndirectorType>::ValidateClientAndShmObject(
    const TraceClientId trace_client_id,
    std::int32_t file_descriptor) noexcept
{
    if (!client_id_container_.GetTraceClientById(trace_client_id).has_value())
    {
        return MakeUnexpected(ErrorCode::kClientNotFoundRecoverable);
    }

    if (shm_object_handle_container_.IsShmObjectAlreadyRegistered(file_descriptor))
    {
        return MakeUnexpected(ErrorCode::kSharedMemoryObjectAlreadyRegisteredRecoverable);
    }

    const auto is_typed_memory = memory_validator_->IsSharedMemoryTyped(file_descriptor);
    if (!is_typed_memory.has_value())
    {
        return MakeUnexpected<Blank>(is_typed_memory.error());
    }
    else if (!is_typed_memory.value())
    {
        return MakeUnexpected(ErrorCode::kSharedMemoryObjectNotInTypedMemoryFatal);
    }
    return {};
}

template <template <class> class IndirectorType>
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
RegisterSharedMemoryObjectResult GenericTraceAPIImplInternal<IndirectorType>::HandleShmObjectRegistration(
    std::int32_t shm_object_identifier) noexcept
{
    auto register_shm_object_result = shm_object_handle_container_.RegisterLocalShmObjectHandle(shm_object_identifier);
    if (!register_shm_object_result.has_value())
    {
        return MakeUnexpected<ShmObjectHandle>(register_shm_object_result.error());
    }

    if (IsDaemonReady())
    {
        // Suppress "AUTOSAR C++14 A15-4-2" rule findings. This rule states: "Throwing an exception in a "noexcept"
        // function." In this case it is ok, because the system anyways forces the process to terminate if an exception
        // is thrown.
        // coverity[autosar_cpp14_a15_4_2_violation] see above
        const auto register_ltpm_daemon_shm_object_result =
            RegisterLocalShmObjectHandleToLtpmDaemon(register_shm_object_result.value().get());
        if (!register_ltpm_daemon_shm_object_result.has_value())
        {
            return MakeUnexpected<ShmObjectHandle>(register_ltpm_daemon_shm_object_result.error());
        }
    }
    return register_shm_object_result.value().get().local_handle_;
}

// clang-format off

// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
template <template <class> class IndirectorType>
// coverity[autosar_cpp14_a15_5_3_violation]
RegisterSharedMemoryObjectResult GenericTraceAPIImplInternal<IndirectorType>::RegisterShmObject(const TraceClientId trace_client_id,
                                                                        const std::string& shm_object_path) noexcept
{
    auto loaded_error = IndirectorType<std::optional<ErrorCode>>::load(global_error_, std::memory_order_acquire);
    if (loaded_error.has_value())
    {
        return MakeUnexpected(loaded_error.value());
    }

    if (shm_object_path.empty())
    {
        //No harm from calling the function in that format
                //  coverity[autosar_cpp14_m8_4_4_violation]
        std::cerr << "error(\"LIB\"): GenericTraceAPIImplInternal::RegisterShmObject: Invalid argument: " << shm_object_path << std::endl;
        return MakeUnexpected(ErrorCode::kInvalidArgumentFatal);
    }
                //No harm from calling the function in that format
                //  coverity[autosar_cpp14_m8_4_4_violation]
    std::cout << "debug(\"LIB\"): "<< "GenericTraceAPIImplInternal::RegisterShmObject: " << static_cast<unsigned>(trace_client_id)<< ", path: " << shm_object_path << std::endl;

    auto file_descriptor = memory_validator_->GetFileDescriptorFromMemoryPath(shm_object_path);

    if(!file_descriptor.has_value())
    {
        return MakeUnexpected<ShmObjectHandle>(file_descriptor.error());
    }

    auto validation_result = ValidateClientAndShmObject(trace_client_id, file_descriptor.value());
    if(!validation_result.has_value())
    {
        return MakeUnexpected<ShmObjectHandle>(validation_result.error());
    }

    return HandleShmObjectRegistration(file_descriptor.value());
}
// clang-format on

// clang-format off
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
template <template <class> class IndirectorType>
// coverity[autosar_cpp14_a15_5_3_violation]
RegisterSharedMemoryObjectResult GenericTraceAPIImplInternal<IndirectorType>::RegisterShmObject(const TraceClientId trace_client_id,
                                                                        const std::int32_t shm_object_fd) noexcept
{
    auto loaded_error = IndirectorType<std::optional<ErrorCode>>::load(global_error_, std::memory_order_acquire);
    if (loaded_error.has_value())
    {
        return MakeUnexpected(loaded_error.value());
    }

    // No harm from calling the function in that format
    // coverity[autosar_cpp14_m8_4_4_violation]
    std::cout << "debug(\"LIB\"): "<< "GenericTraceAPIImplInternal::RegisterShmObject: " << static_cast<unsigned>(trace_client_id)<< " fd: " << shm_object_fd << std::endl;

    if (shm_object_fd < 0)
    {
        // No harm from calling the function in that format
        // coverity[autosar_cpp14_m8_4_4_violation]
        std::cout << "error(\"LIB\"):  GenericTraceAPIImplInternal::RegisterShmObject: Invalid SHM object file descriptor" << std::endl;
        return MakeUnexpected(ErrorCode::kInvalidArgumentFatal);
    }

    auto validation_result = ValidateClientAndShmObject(trace_client_id, shm_object_fd);
    if(!validation_result.has_value())
    {
        return MakeUnexpected<ShmObjectHandle>(validation_result.error());
    }

    return HandleShmObjectRegistration(shm_object_fd);
}
// clang-format on

// clang-format off
template <template <class> class IndirectorType>
ResultBlank GenericTraceAPIImplInternal<IndirectorType>::UnregisterShmObject(const TraceClientId trace_client_id, const ShmObjectHandle handle)
{
    auto loaded_error = IndirectorType<std::optional<ErrorCode>>::load(global_error_, std::memory_order_acquire);
    if (loaded_error.has_value())
    {
        shm_object_handle_container_.DeregisterLocalShmObject(handle);
        return MakeUnexpected(loaded_error.value());
    }

    // No harm from calling the function in that format
    //   coverity[autosar_cpp14_m8_4_4_violation]
    if (!client_id_container_.GetTraceClientById(trace_client_id).has_value())
    {
        return MakeUnexpected(ErrorCode::kClientNotFoundRecoverable);
    }

    if (IsDaemonReady()) // LCOV_EXCL_BR_LINE tooling issue branches and lines are covered.
    {
        return UnregisterLtpmDaemonShmObject(handle);
    }
    else
    {
        shm_object_handle_container_.DeregisterLocalShmObject(handle);
    }
    return {};
}
// clang-format on

template <template <class> class IndirectorType>
RegisterTraceDoneCallBackResult GenericTraceAPIImplInternal<IndirectorType>::RegisterTraceDoneCB(
    const TraceClientId trace_client_id,
    TraceDoneCallBackType trace_done_callback)
{
    auto loaded_error = IndirectorType<std::optional<ErrorCode>>::load(global_error_, std::memory_order_acquire);
    if (loaded_error.has_value())
    {
        return MakeUnexpected(loaded_error.value());
    }

    if (!trace_done_callback)
    {
        return MakeUnexpected(ErrorCode::kInvalidArgumentFatal);
    }

    // clang-format off
    // No harm from calling the function in that format
    //   coverity[autosar_cpp14_m8_4_4_violation]
    std::cout << "debug(\"LIB\"):GenericTraceAPIImplInternal::RegisterTraceCallDoneCB: "<< static_cast<unsigned>(trace_client_id) << std::endl;
    // clang-format on
    if (!client_id_container_.GetTraceClientById(trace_client_id).has_value())
    {
        return MakeUnexpected(ErrorCode::kClientNotFoundRecoverable);
    }

    // if the pointer is nullptr it means the worker thread is finished
    if (trace_job_processor_ != nullptr)  // LCOV_EXCL_BR_LINE this condition will always be true as a library instance
                                          // should have been instantiated
    {
        // save a callback with the local trace client id
        return trace_job_processor_->SaveCallback(trace_client_id, std::move(trace_done_callback));
    }
    return MakeUnexpected(ErrorCode::kTerminalFatal);  // LCOV_EXCL_LINE not reachable (see suppression above)
}

template <template <class> class IndirectorType>
TraceResult GenericTraceAPIImplInternal<IndirectorType>::Trace(const TraceClientId trace_client_id,
                                                               const MetaInfoVariants::type& meta_info,
                                                               ShmDataChunkList& data,
                                                               TraceContextId context_id)
{
    auto loaded_error = IndirectorType<std::optional<ErrorCode>>::load(global_error_, std::memory_order_acquire);
    if (loaded_error.has_value())
    {
        return MakeUnexpected(loaded_error.value());
    }

    const auto& element = client_id_container_.GetTraceClientById(trace_client_id);
    if (!element.has_value())
    {
        return MakeUnexpected(ErrorCode::kClientNotFoundRecoverable);
    }

    const auto& element_value = element.value().get();
    // return pending error
    if (element_value.pending_error_.has_value())
    {
        auto error = element_value.pending_error_.value();
        return MakeUnexpected(error);
    }

    // make sure that the client was registered to the LTPM Daemon
    if (element_value.client_id_ == kInvalidTraceClientId)
    {
        return MakeUnexpected(ErrorCode::kDaemonNotConnectedRecoverable);
    }

    if (IsLibraryReady())
    {
        // replace the local shm object handles with the LTPM Daemon-registered ones
        auto& chunk_list = data.GetList();
        // Tooling issue: as reported from quality team that cases where branch coverage is 100% but decision couldn't
        // be analyzed are accepted as deviations
        for (std::uint8_t i = 0U; i < data.Size(); i++)  // LCOV_EXCL_BR_LINE not testable see comment above.
        {
            // Suppress "AUTOSAR C++14 M5-0-3" rule findings. This rule states: "A cvalue expression shall
            // not be implicitly converted to a different underlying type"
            // False positive, right hand value is the same type.
            // coverity[autosar_cpp14_m5_0_3_violation]
            auto& chunk_list_element = chunk_list.at(i);

            const auto local_handle = chunk_list_element.start_.shm_object_handle_;
            const auto handle = shm_object_handle_container_.GetLtpmDaemonRegisteredShmObjectHandle(local_handle);
            if (!handle.has_value())
            {
                return MakeUnexpected<Blank>(handle.error());
            }
            chunk_list_element.start_.shm_object_handle_ = handle.value();
        }

        return trace_job_allocator_->AllocateShmJob(
            element_value.client_id_, meta_info, element_value.binding_, element_value.app_id_, data, context_id);
    }
    return MakeUnexpected(ErrorCode::kDaemonNotConnectedRecoverable);
}

// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
template <template <class> class IndirectorType>
// coverity[autosar_cpp14_a15_5_3_violation]
TraceResult GenericTraceAPIImplInternal<IndirectorType>::Trace(const TraceClientId trace_client_id,
                                                               const MetaInfoVariants::type& meta_info,
                                                               LocalDataChunkList& data) noexcept
{
    auto loaded_error = IndirectorType<std::optional<ErrorCode>>::load(global_error_, std::memory_order_acquire);
    if (loaded_error.has_value())
    {
        return MakeUnexpected(loaded_error.value());
    }

    const auto& element = client_id_container_.GetTraceClientById(trace_client_id);
    if (!element.has_value())
    {
        return MakeUnexpected(ErrorCode::kClientNotFoundRecoverable);
    }

    const auto& element_value = element.value().get();
    // return pending error
    if (element_value.pending_error_.has_value())
    {
        auto error = element_value.pending_error_.value();
        return MakeUnexpected(error);
    }

    // make sure that the client was registered to the LTPM Daemon
    if (element_value.client_id_ == kInvalidTraceClientId)
    {
        return MakeUnexpected(ErrorCode::kDaemonNotConnectedRecoverable);
    }

    if (IsLibraryReady())
    {
        // Suppress "AUTOSAR C++14 A15-4-2" rule findings. This rule states: "Throwing an exception in a
        // "noexcept" function." In this case it is ok, because the system anyways forces the process to
        // terminate if an exception is thrown.
        // coverity[autosar_cpp14_a15_4_2_violation]
        return trace_job_allocator_->AllocateLocalJob(
            element_value.client_id_, meta_info, element_value.binding_, element_value.app_id_, data);
    }
    return MakeUnexpected(ErrorCode::kDaemonNotConnectedRecoverable);
}

template <template <class> class IndirectorType>
bool GenericTraceAPIImplInternal<IndirectorType>::TryDaemonConnection()
{
    auto result = daemon_communicator_->Connect();

    if (result.has_value())
    {
        state_ = LibraryState::kDaemonInitialized;
        return true;
    }
    return false;
}
// clang-format on

template <template <class> class IndirectorType>
score::Result<Blank> GenericTraceAPIImplInternal<IndirectorType>::CreateTraceMetaDataSharedMemory(
    const std::string shared_memory_path)
{
    memory::shared::SharedMemoryFactory::WorldWritable permissions{};
    // the shared-memory path should be dev_tmd_{pid}
    // Concatenate the PID to the path_name
    std::stringstream path_stream;
    path_stream << shared_memory_path << unistd_impl_->getpid();

    std::ignore = std::strncpy(
        trace_meta_data_shared_memory_path_.data(), path_stream.str().c_str(), kMaxSizeForTmdSharedMemoryPath);

    memory::shared::SharedMemoryFactory::SetTypedMemoryProvider(memory::shared::TypedMemory::Default());
    // create the shared memory for trace meta data
    trace_metadata_memory_resource_ = memory::shared::SharedMemoryFactory::Create(
        std::string(static_cast<const char*>(trace_meta_data_shared_memory_path_.data())),
        [](auto&&) {},
        kTmdSharedMemorySize,
        {permissions},
        true);

    if ((trace_metadata_memory_resource_ == nullptr) || (trace_metadata_memory_resource_->getBaseAddress() == nullptr))
    {
        std::cerr << "debug(\"LIB\"): " << std::string(static_cast<const char*>(__func__))
                  << " Failed to create shared memory region";
        state_ = LibraryState::kGenericError;
        score::memory::shared::SharedMemoryFactory::Remove(
            std::string(static_cast<const char*>(trace_meta_data_shared_memory_path_.data())));
        return MakeUnexpected(ErrorCode::kSharedMemoryObjectRegistrationFailedFatal);
    }

    if (!trace_metadata_memory_resource_->IsShmInTypedMemory())
    {  // clang-format off
        // No harm from calling the function in that format
        //   coverity[autosar_cpp14_m8_4_4_violation]
        std::cerr << "debug(\"LIB\"): " << std::string(static_cast<const char*>(__func__))<< " The allocated shared memory region is not typed memory" << std::endl;
        // clang-format on

        score::memory::shared::SharedMemoryFactory::Remove(trace_meta_data_shared_memory_path_.data());
        return MakeUnexpected(ErrorCode::kSharedMemoryObjectNotInTypedMemoryFatal);
    }

    return {};
}

// clang-format off
template <template <class> class IndirectorType>
ResultBlank GenericTraceAPIImplInternal<IndirectorType>::UnregisterAndRemoveTraceMetaDataSharedMemory()
{
    const auto result = daemon_communicator_->UnregisterSharedMemoryObject(trace_meta_data_memory_handle_);
    if (!result.has_value())
    {
        // No harm from calling the function in that format
        //   coverity[autosar_cpp14_m8_4_4_violation]
        std::cerr << "debug(\"LIB\"): GenericTraceAPIImplInternal::UnregisterAndRemoveTraceMetaDataSharedMemory: failed to unregister TMD memory"<< std::endl;
    }
    // Unlink the shared memory object
    memory::shared::SharedMemoryFactory::Remove(trace_meta_data_shared_memory_path_.data());

    return {};
}
// clang-format on

template <template <class> class IndirectorType>
void GenericTraceAPIImplInternal<IndirectorType>::WorkerThread(const score::cpp::stop_token& stop_token)
{
    // No harm from calling the function in that format
    // coverity[autosar_cpp14_m8_4_4_violation]
    std::cout << "debug(\"LIB\"): GenericTraceAPIImplInternal::WorkerThread started" << std::endl;
    while (!stop_token.stop_requested())
    {
        if (!ConnectToDaemon(stop_token))
        {
            return;
        }

        if (!RegisterLtpmDaemonClientsAndShmObjectHandles(stop_token))
        {
            return;
        }

        if (!InitializeTraceMetaDataAndAllocator())
        {
            return;
        }

        state_ = LibraryState::kInitialized;
        IndirectorType<std::optional<ErrorCode>>::store(global_error_, std::nullopt, std::memory_order_release);
        auto result = ProcessJobsLoop(stop_token);
        if (!result.has_value())
        {
            break;
        }
    }

    CleanUpResources();
}

template <template <class> class IndirectorType>
bool GenericTraceAPIImplInternal<IndirectorType>::ConnectToDaemon(const score::cpp::stop_token& stop_token)
{
    while (!stop_token.stop_requested())
    {
        if (TryDaemonConnection())
        {
            return true;
        }
        std::ignore = score::concurrency::wait_for(stop_token, kDaemonConnectionRetrySleepTime);
    }
    // if we are here the LTPM Daemon never became available
    // No harm from calling the function in that format
    // coverity[autosar_cpp14_m8_4_4_violation]
    std::cerr << "debug(\"LIB\"): GenericTraceAPIImplInternal::WorkerThread: LTPM Daemon not available" << std::endl;
    IndirectorType<std::optional<ErrorCode>>::store(
        global_error_, ErrorCode::kDaemonNotAvailableFatal, std::memory_order_release);
    return false;
}

template <template <class> class IndirectorType>
bool GenericTraceAPIImplInternal<IndirectorType>::RegisterLtpmDaemonClientsAndShmObjectHandles(
    const score::cpp::stop_token& stop_token)
{
    const auto register_clients_result = RegisterLtpmDaemonClients(stop_token);
    if (!register_clients_result.has_value())
    {
        // clang-format off
        std::cerr << "debug(\"LIB\"): GenericTraceAPIImplInternal::WorkerThread: Failed to register the cached client registration requests, error: "
                    // No harm from calling the function in that format
                    //  coverity[autosar_cpp14_m8_4_4_violation]
                  << register_clients_result.error().Message() << std::endl;
        // clang-format on
        IndirectorType<std::optional<ErrorCode>>::store(
            global_error_, ErrorCode::kFailedRegisterCachedClientsFatal, std::memory_order_release);
        return false;
    }

    const auto register_shm_object_result = RegisterLtpmDaemonShmObjectHandles(stop_token);
    if (!register_shm_object_result.has_value())
    {
        // clang-format off
        std::cerr << "debug(\"LIB\"): GenericTraceAPIImplInternal::WorkerThread: Failed to register the cached SHM object registration requests, error: "
                    // No harm from calling the function in that format
                    // coverity[autosar_cpp14_m8_4_4_violation]
                  << register_shm_object_result.error().Message() << std::endl;
        // clang-format on
        IndirectorType<std::optional<ErrorCode>>::store(
            global_error_, ErrorCode::kFailedRegisterCachedShmObjectsFatal, std::memory_order_release);
        return false;
    }
    return true;
}

template <template <class> class IndirectorType>
bool GenericTraceAPIImplInternal<IndirectorType>::InitializeTraceMetaDataAndAllocator()
{
    if (!create_tmd_result_.has_value())
    {
        state_ = LibraryState::kGenericError;
        IndirectorType<std::optional<ErrorCode>>::store(
            global_error_,
            // No harm from going outside the range as that will jump to the default state in switch_case(a7_2_1)
            // coverity[autosar_cpp14_a7_2_1_violation]
            static_cast<ErrorCode>(*create_tmd_result_.error()),
            std::memory_order_release);
        return false;
    }
    auto register_tmd_result =
        daemon_communicator_->RegisterSharedMemoryObject(trace_meta_data_shared_memory_path_.data());
    if (!register_tmd_result.has_value())
    {
        state_ = LibraryState::kGenericError;
        IndirectorType<std::optional<ErrorCode>>::store(
            global_error_,
            // No harm from going outside the range as that will jump to the default state in switch_case(a7_2_1)
            // coverity[autosar_cpp14_a7_2_1_violation]
            static_cast<ErrorCode>(*register_tmd_result.error()),
            std::memory_order_release);
        return false;
    }
    trace_meta_data_memory_handle_ = register_tmd_result.value();
    if (!trace_job_allocator_result_.has_value())
    {
        // clang-format off
        // No harm from calling the function in that format
        // coverity[autosar_cpp14_m8_4_4_violation]
        std::cerr << "debug(\"LIB\"): GenericTraceAPIImplInternal::WorkerThread: TraceJobAllocator initialization failed" << std::endl;
        // clang-format on
        IndirectorType<std::optional<ErrorCode>>::store(
            global_error_, ErrorCode::kTraceJobAllocatorInitializationFailedFatal, std::memory_order_release);
        return false;
    }
    // we need to ensure that we initialize the trace job allocator only one time after construction
    // but if we have the daemon disconnected and connected back again, we expect that it's already initalized
    if (trace_job_allocator_ == nullptr)
    {
        // we move the allocator only one time after the first connection with daemon
        trace_job_allocator_ = std::move(trace_job_allocator_result_.value());
    }
    trace_job_allocator_->SetTraceMetaDataShmObjectHandle(trace_meta_data_memory_handle_);
    return true;
}

template <template <class> class IndirectorType>
score::Result<Blank> GenericTraceAPIImplInternal<IndirectorType>::ProcessJobsLoop(const score::cpp::stop_token& stop_token)
{
    do  // LCOV_EXCL_LINE - False positive
    {
        if (state_ == LibraryState::kDaemonDisconnected)
        {
            IndirectorType<std::optional<ErrorCode>>::store(
                global_error_, ErrorCode::kDaemonIsDisconnectedRecoverable, std::memory_order_release);

            HandleDaemonTermination();
            break;
        }
        const auto process_jobs_result = trace_job_processor_->ProcessJobs();
        if (!process_jobs_result.has_value())
        {
            IndirectorType<std::optional<ErrorCode>>::store(
                global_error_, ErrorCode::kFailedToProcessJobsFatal, std::memory_order_release);
            // clang-format off
            // No harm from calling the function in that format
            // coverity[autosar_cpp14_m8_4_4_violation]
            std::cerr << "debug(\"LIB\"): GenericTraceAPIImplInternal::WorkerThread: " << process_jobs_result.error()<< std::endl;
            // clang-format on
            return MakeUnexpected<Blank>(process_jobs_result.error());
        }
        std::ignore = concurrency::wait_for(stop_token, kWorkerThreadPeriodicity);
        // Tooling issue: as reported from quality team that cases where branch coverage is 100% but decision couldn't
        // be analyzed are accepted as deviations
    } while (!stop_token.stop_requested());  // LCOV_EXCL_BR_LINE not testable see comment above.
    return {};
}

template <template <class> class IndirectorType>
void GenericTraceAPIImplInternal<IndirectorType>::CleanUpResources()
{
    state_ = LibraryState::kNotInitialized;
    // unregister and remove the tmd shared memory
    std::ignore = UnregisterAndRemoveTraceMetaDataSharedMemory();
    trace_job_allocator_->CloseRingBuffer();
    // No harm from calling the function in that format
    // coverity[autosar_cpp14_m8_4_4_violation]
    std::cout << "debug(\"LIB\"): GenericTraceAPIImplInternal::WorkerThread finished" << std::endl;
    // cleanup resources before leaving
    daemon_communicator_.reset();
    trace_job_processor_.reset();
    trace_job_allocator_.reset();
    trace_metadata_memory_resource_.reset();
}

template <template <class> class IndirectorType>
void GenericTraceAPIImplInternal<IndirectorType>::HandleDaemonTermination()
{
    // Cleaning the pending jobs and call the corresponding callbacks
    score::cpp::ignore = trace_job_processor_->CleanPendingJobs();

    trace_job_allocator_->ResetRingBuffer();
    // No harm from calling the function in that format
    // coverity[autosar_cpp14_m8_4_4_violation]
    std::cout << "debug(\"LIB\"): GenericTraceAPIImplInternal::WorkerThread, The Daemon is disconnected, The "
                 "WorkerThread will be alive till it's up again"
              // coverity[autosar_cpp14_m8_4_4_violation]
              << std::endl;
    client_id_container_.InvalidateRemoteRegistrationOfAllClientIds();
    shm_object_handle_container_.InvalidateRemoteRegistrationOfAllShmObjects();
}

template <template <class> class IndirectorType>
ResultBlank GenericTraceAPIImplInternal<IndirectorType>::RegisterLocalShmObjectHandleToLtpmDaemon(
    ShmObjectHandleElement& local_shm_object_handle)
{
    auto register_shm_object_result =
        daemon_communicator_->RegisterSharedMemoryObject(local_shm_object_handle.file_descriptor_);

    if (!register_shm_object_result.has_value())
    {
        return MakeUnexpected<Blank>(register_shm_object_result.error());
    }
    local_shm_object_handle.handle_ = register_shm_object_result.value();
    return {};
}

template <template <class> class IndirectorType>
ResultBlank GenericTraceAPIImplInternal<IndirectorType>::RegisterLocalClientToLtpmDaemon(
    ClientIdElement& client_id_element)
{
    const auto register_ltpm_daemon_client_result =
        daemon_communicator_->RegisterClient(client_id_element.binding_, client_id_element.app_id_);
    if (!register_ltpm_daemon_client_result.has_value())
    {
        std::ostringstream app_identifier_name{};
        for (const auto character : client_id_element.app_id_)
        {
            app_identifier_name << character;
        }
        std::cerr << "Failed to register local client to LTPM Daemon, App ID:" << app_identifier_name.str()
                  << ", binding type:" << static_cast<std::uint32_t>(client_id_element.binding_)
                  << ", Error:" << register_ltpm_daemon_client_result.error().Message();
        return MakeUnexpected<Blank>(register_ltpm_daemon_client_result.error());
    }
    client_id_element.client_id_ = register_ltpm_daemon_client_result.value();
    return {};
}

template <template <class> class IndirectorType>
ResultBlank GenericTraceAPIImplInternal<IndirectorType>::RegisterLtpmDaemonShmObjectHandles(
    const score::cpp::stop_token& stop_token)
{
    std::size_t shm_object_handle_index = 0U;
    const auto& shm_object_container_size = shm_object_handle_container_.Size();
    // clang-format off
    while ((shm_object_handle_index < shm_object_container_size) && (!stop_token.stop_requested()))  // LCOV_EXCL_BR_LINE not testable see comment above.
    {
        // clang-format on
        auto& shm_object_handle = shm_object_handle_container_[shm_object_handle_index].get();
        if ((shm_object_handle.local_handle_ != kInvalidSharedObjectIndex) &&
            (shm_object_handle.handle_ == kInvalidSharedObjectIndex))
        {
            const auto register_shm_object_result = RegisterLocalShmObjectHandleToLtpmDaemon(shm_object_handle);
            if (!register_shm_object_result.has_value())
            {
                return register_shm_object_result;
            }
        }
        ++shm_object_handle_index;
    }
    if (stop_token.stop_requested())
    {
        // if we are here the LTPM Daemon never became available
        // No harm from calling the function in that format
        // coverity[autosar_cpp14_m8_4_4_violation]
        std::cerr << "Stop was requested, not all cached SHM object registration requests were registered" << std::endl;
        return MakeUnexpected(ErrorCode::kFailedRegisterCachedShmObjectsFatal);
    }
    return {};
}

template <template <class> class IndirectorType>
ResultBlank GenericTraceAPIImplInternal<IndirectorType>::RegisterLtpmDaemonClients(const score::cpp::stop_token& stop_token)
{
    std::size_t client_id_index = 0U;
    const auto& client_id_container_size = client_id_container_.Size();
    // clang-format off
    while ((client_id_index < client_id_container_size) && (!stop_token.stop_requested()))  // LCOV_EXCL_BR_LINE not testable see comment above.
    {
        // clang-format on
        auto& client_id_element = client_id_container_[client_id_index].get();
        if ((client_id_element.local_client_id_ != kInvalidTraceClientId) &&
            (client_id_element.client_id_ == kInvalidTraceClientId))
        {
            const auto register_client_result = RegisterLocalClientToLtpmDaemon(client_id_element);
            if (!register_client_result.has_value())
            {
                // Any other values do not correspond to
                // one of the enumerators of the enumeration will be handled with the default case.
                // coverity[autosar_cpp14_a7_2_1_violation]
                client_id_container_.SetClientError(static_cast<ErrorCode>(*register_client_result.error()),
                                                    client_id_element.local_client_id_);
            }
        }
        ++client_id_index;
    }
    if (stop_token.stop_requested())
    {
        // if we are here the LTPM Daemon never became available
        // No harm from calling the function in that format
        // coverity[autosar_cpp14_m8_4_4_violation]
        std::cerr << "Stop was requested, not all cached client requests were registered" << std::endl;
        return MakeUnexpected(ErrorCode::kFailedRegisterCachedClientsFatal);
    }
    return {};
}
template <template <class> class IndirectorType>
ResultBlank GenericTraceAPIImplInternal<IndirectorType>::UnregisterLtpmDaemonShmObject(const ShmObjectHandle handle)
{
    auto found = shm_object_handle_container_.FindIf([handle](const auto& element) {
        return element.local_handle_ == handle;
    });

    if (found.has_value())
    {
        const auto unregister_shm_object_result =
            daemon_communicator_->UnregisterSharedMemoryObject(found.value().get().handle_);
        if (!unregister_shm_object_result.has_value())
        {
            return MakeUnexpected<Blank>(unregister_shm_object_result.error());
        }

        found.value().get().handle_ = kInvalidSharedObjectIndex;
        found.value().get().local_handle_ = kInvalidSharedObjectIndex;
        shm_object_handle_container_.Release(found.value());
    }
    return {};
}

template class GenericTraceAPIImplInternal<score::memory::shared::AtomicIndirectorReal>;
template class GenericTraceAPIImplInternal<score::memory::shared::AtomicIndirectorMock>;

}  // namespace detail
}  // namespace tracing
}  // namespace analysis
}  // namespace score
