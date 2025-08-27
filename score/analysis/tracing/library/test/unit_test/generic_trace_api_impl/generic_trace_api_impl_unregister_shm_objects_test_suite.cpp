#include "score/analysis/tracing/library/test/unit_test/generic_trace_api_impl/generic_trace_api_impl_test.h"

#include <future>

namespace score
{
namespace analysis
{
namespace tracing
{

TEST_F(GenericTraceAPIImplFixture, UnregisterShmObjectShmObjectNotFound)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};
    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
        EXPECT_CALL(*mock_memory_validator_, IsSharedMemoryTyped(Matcher<std::int32_t>(file_descriptor_)))
            .WillOnce(Return(true));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_,
                    RegisterSharedMemoryObject(Matcher<std::int32_t>(file_descriptor_)))
            .Times(1)
            .WillOnce(Return(shm_obj_handle_));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillRepeatedly(Return(ResultBlank{}));
        EXPECT_CALL(*mock_shared_memory_factory_, Remove(tmd_filename_)).WillOnce(Invoke([&library_notifier]() {
            library_notifier.Notify();
        }));
        EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, CloseRingBuffer);
    }

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);
    library_notifier.Reset();
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto register_shm_by_file_descriptor =
        library_->RegisterShmObject(register_client_result.value(), file_descriptor_);
    auto unregister_shm_result =
        library_->UnregisterShmObject(static_cast<TraceClientId>(register_client_result.value()), shm_obj_handle_ + 1);
    EXPECT_TRUE(unregister_shm_result.has_value());
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_TRUE(register_shm_by_file_descriptor.has_value());
}

TEST_F(GenericTraceAPIImplFixture, UnregisterShmObjectDaemonNotConnected)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    const TraceClientId client_id{1};
    EXPECT_CALL(*mock_object_factory_, CreateTraceJobProcessor)
        .WillOnce(Return(ByMove(std::move(mock_trace_job_processor_))));
    EXPECT_CALL(*mock_object_factory_, CreateDaemonCommunicator).WillRepeatedly([this]() {
        return std::move(mock_daemon_communicator_ptr_);
    });
    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, SubscribeToDaemonTerminationNotification);
    EXPECT_CALL(*mock_unistd_, getpid()).WillOnce(Return(pid_));
    EXPECT_CALL(*mock_shared_memory_factory_, SetTypedMemoryProvider(_));
    EXPECT_CALL(*mock_shared_memory_factory_, Create).WillOnce(Return(mock_shared_memory_resource_));
    EXPECT_CALL(*mock_shared_memory_resource_, getBaseAddress)
        .WillOnce(Return(&valid_pointer_));  // just return a valid pointer
    EXPECT_CALL(*mock_shared_memory_resource_, IsShmInTypedMemory).WillOnce(Return(true));
    EXPECT_CALL(*mock_object_factory_, CreateTraceJobAllocator)
        .WillOnce(Return(ByMove(std::move(mock_trace_job_allocator_))));
    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, Connect)
        .WillOnce(Invoke([&library_notifier, &daemon_notifier] {
            daemon_notifier.Notify();
            library_notifier.Notify();
            return score::MakeUnexpected(ErrorCode::kDaemonNotConnectedFatal);
        }))
        .WillRepeatedly(Return(score::MakeUnexpected(ErrorCode::kDaemonNotConnectedFatal)));
    EXPECT_CALL(*mock_memory_validator_, IsSharedMemoryTyped(Matcher<std::int32_t>(file_descriptor_)))
        .WillOnce(Return(true));

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto register_shm_by_file_descriptor = library_->RegisterShmObject(client_id, file_descriptor_);
    auto unregister_shm_result = library_->UnregisterShmObject(client_id, shm_obj_handle_);
    auto request_shutdown = std::async(std::launch::async, [this, &library_notifier]() {
        std::this_thread::sleep_for(kLibraryCheckPeriodicity * 10);
        RequestLibraryWorkerThreadShutdown(library_notifier);
    });

    daemon_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);
    request_shutdown.get();

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_TRUE(register_shm_by_file_descriptor.has_value());
    EXPECT_TRUE(unregister_shm_result.has_value());
}

TEST_F(GenericTraceAPIImplFixture, UnregisterShmObjectOnlyDaemonReadySuccess)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};
    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
        EXPECT_CALL(*mock_memory_validator_, IsSharedMemoryTyped(Matcher<std::int32_t>(file_descriptor_)))
            .WillOnce(Return(true));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_,
                    RegisterSharedMemoryObject(Matcher<std::int32_t>(file_descriptor_)))
            .Times(1)
            .WillOnce(Return(shm_obj_handle_));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(Eq(shm_obj_handle_)))
            .WillOnce(Return(ResultBlank{}));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillOnce(Return(ResultBlank{}));
        EXPECT_CALL(*mock_shared_memory_factory_, Remove(tmd_filename_));
        EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, CloseRingBuffer).WillOnce(Invoke([&library_notifier]() {
            library_notifier.Notify();
        }));
    }

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());

    daemon_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);

    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto register_shm_by_file_descriptor = library_->RegisterShmObject(client_id, file_descriptor_);
    auto unregister_shm_result = library_->UnregisterShmObject(client_id, shm_obj_handle_);
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);
    library_notifier.Reset();
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_TRUE(register_shm_by_file_descriptor.has_value());
    EXPECT_TRUE(unregister_shm_result.has_value());
}

TEST_F(GenericTraceAPIImplFixture, UnregisterShmObjectSuccess)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};
    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
        EXPECT_CALL(*mock_memory_validator_, IsSharedMemoryTyped(Matcher<std::int32_t>(file_descriptor_)))
            .WillOnce(Return(true));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_,
                    RegisterSharedMemoryObject(Matcher<std::int32_t>(file_descriptor_)))
            .Times(1)
            .WillOnce(Return(shm_obj_handle_));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(Eq(shm_obj_handle_)))
            .WillOnce(Return(ResultBlank{}));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillOnce(Return(ResultBlank{}));
        EXPECT_CALL(*mock_shared_memory_factory_, Remove(tmd_filename_));
        EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, CloseRingBuffer).WillOnce(Invoke([&library_notifier]() {
            library_notifier.Notify();
        }));
    }

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);
    library_notifier.Reset();
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto register_shm_by_file_descriptor = library_->RegisterShmObject(client_id, file_descriptor_);
    auto unregister_shm_result = library_->UnregisterShmObject(client_id, shm_obj_handle_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_TRUE(register_shm_by_file_descriptor.has_value());
    EXPECT_TRUE(unregister_shm_result.has_value());
}

TEST_F(GenericTraceAPIImplFixture, UnregisterShmObjectFailUnregisterSharedMemoryObject)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};
    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
        EXPECT_CALL(*mock_memory_validator_, IsSharedMemoryTyped(Matcher<std::int32_t>(file_descriptor_)))
            .WillOnce(Return(true));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_,
                    RegisterSharedMemoryObject(Matcher<std::int32_t>(file_descriptor_)))
            .Times(1)
            .WillOnce(Return(shm_obj_handle_));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(Eq(shm_obj_handle_)))
            .WillOnce(Return(MakeUnexpected(ErrorCode::kDaemonNotConnectedFatal)))
            .WillOnce(Return(ResultBlank{}));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillOnce(Return(ResultBlank{}));
        EXPECT_CALL(*mock_shared_memory_factory_, Remove(tmd_filename_));
        EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, CloseRingBuffer).WillOnce(Invoke([&library_notifier]() {
            library_notifier.Notify();
        }));
    }

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);
    library_notifier.Reset();
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto register_shm_by_file_descriptor = library_->RegisterShmObject(client_id, file_descriptor_);
    auto unregister_shm_result = library_->UnregisterShmObject(client_id, shm_obj_handle_);
    EXPECT_FALSE(unregister_shm_result.has_value());
    unregister_shm_result = library_->UnregisterShmObject(client_id, shm_obj_handle_);
    EXPECT_TRUE(unregister_shm_result.has_value());
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_TRUE(register_shm_by_file_descriptor.has_value());
}

TEST_F(GenericTraceAPIImplFixture, UnregisterShmObjectClientNotFound)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};
    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
        EXPECT_CALL(*mock_memory_validator_, IsSharedMemoryTyped(Matcher<std::int32_t>(file_descriptor_)))
            .WillOnce(Return(true));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_,
                    RegisterSharedMemoryObject(Matcher<std::int32_t>(file_descriptor_)))
            .Times(1)
            .WillOnce(Return(shm_obj_handle_));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillOnce(Return(ResultBlank{}));
        EXPECT_CALL(*mock_shared_memory_factory_, Remove(tmd_filename_));
        EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, CloseRingBuffer).WillOnce(Invoke([&library_notifier]() {
            library_notifier.Notify();
        }));
    }

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);
    library_notifier.Reset();
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto register_shm_by_file_descriptor =
        library_->RegisterShmObject(register_client_result.value(), file_descriptor_);
    auto unregister_shm_result =
        library_->UnregisterShmObject(static_cast<TraceClientId>(register_client_result.value() + 1), shm_obj_handle_);
    EXPECT_EQ(unregister_shm_result.error(), ErrorCode::kClientNotFoundRecoverable);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_TRUE(register_shm_by_file_descriptor.has_value());
}

TEST_F(GenericTraceAPIImplFixture, UnregisterSharedMemoryObjectReturnError)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};
    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject)
            .WillOnce(Return(MakeUnexpected(ErrorCode::kDaemonNotConnectedFatal)));
        EXPECT_CALL(*mock_shared_memory_factory_, Remove(tmd_filename_));
        EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, CloseRingBuffer);
    }

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    library_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);

    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_EQ(register_client_result.value(), client_id);
    stop_source_.request_stop();
}

TEST_F(GenericTraceAPIImplFixture, UnregisterShmObjectShallFailWhenCleaningPendingJobsByHandleFail)
{
    RecordProperty("Verifies", "SCR-39689311");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "UnRegisterShmObject Shall fail when CleanPendingJobs is provided an invalid Handle");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // TODO: shall be removed after invalidating the requirement after sop-2603.
}
}  // namespace tracing
}  // namespace analysis
}  // namespace score
