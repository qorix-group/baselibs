#include "score/analysis/tracing/library/test/unit_test/generic_trace_api_impl/generic_trace_api_impl_test.h"

namespace score
{
namespace analysis
{
namespace tracing
{

TEST_F(GenericTraceAPIImplFixture, TraceShmClientNotFound)
{
    RecordProperty("Verifies", "SCR-39691016, SCR-39688772");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if Trace() API returns recoverable error when invalid trace client ID is given");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};
    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
    ExpectCorrectCleaningUp(library_notifier);

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);
    library_notifier.Reset();
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    EXPECT_TRUE(register_client_result.has_value());
    auto shm_trace_result = library_->Trace(
        static_cast<TraceClientId>(register_client_result.value() + 1), meta_info_, shm_data_chunk_list_, context_id_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_EQ(shm_trace_result.error(), ErrorCode::kClientNotFoundRecoverable);
}

TEST_F(GenericTraceAPIImplFixture, TraceShmDaemonNotConnected)
{
    RecordProperty("Verifies", "SCR-39691333, SCR-39688772");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if Trace() API returns recoverable error when Daemon is not available");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    PromiseNotifier terminate_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};

    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
    ExpectCorrectCleaningUp(terminate_notifier);

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());

    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 10);
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);

    RequestLibraryWorkerThreadShutdown(terminate_notifier);
    auto shm_trace_result =
        library_->Trace(register_client_result.value(), meta_info_, shm_data_chunk_list_, context_id_);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_FALSE(shm_trace_result.has_value());
    EXPECT_EQ(shm_trace_result.error(), ErrorCode::kDaemonNotConnectedRecoverable);
}

TEST_F(GenericTraceAPIImplFixture, TraceUseUnregisteredShm)
{
    const SharedMemoryLocation shm_offset_ptr{-1, 0};
    const SharedMemoryChunk shm_chunk{shm_offset_ptr, 0};
    ShmDataChunkList shm_data_chunk_list{shm_chunk};

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
    }
    ExpectCorrectCleaningUp(library_notifier);

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);
    library_notifier.Reset();
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto register_shm_by_file_descriptor =
        library_->RegisterShmObject(register_client_result.value(), file_descriptor_);
    EXPECT_TRUE(register_shm_by_file_descriptor.has_value());
    auto shm_trace_result =
        library_->Trace(register_client_result.value(), meta_info_, shm_data_chunk_list, context_id_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_FALSE(shm_trace_result.has_value());
}

TEST_F(GenericTraceAPIImplFixture, TraceShmSuccess)
{
    RecordProperty("Verifies", "SCR-39766357, SCR-39766327, SCR-39765455");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if Library:"
                   "1) uses trace_job_allocator to queue data_chunk_list for sending"
                   "2) uses daemon_communicator - service oriented request and response interface."
                   "3) UnregisterSharedMemoryObject and free TMD memory when the process linking against the library "
                   "is being terminated");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

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
        EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, AllocateShmJob(client_id, _, _, _, _, context_id_))
            .WillOnce(Return(ResultBlank{}));
    }
    ExpectCorrectCleaningUp(library_notifier);

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);
    library_notifier.Reset();
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto register_shm_by_file_descriptor =
        library_->RegisterShmObject(register_client_result.value(), file_descriptor_);
    auto shm_trace_result =
        library_->Trace(register_client_result.value(), meta_info_, shm_data_chunk_list_, context_id_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_shm_by_file_descriptor.has_value());
    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_TRUE(shm_trace_result.has_value());
}

TEST_F(GenericTraceAPIImplFixture, TraceShmFailAfterDaemonIsDisconnected)
{
    std::atomic<bool> delay_ltpm_daemon = false;
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    PromiseNotifier daemon_crash_callback_notifier;
    PromiseNotifier terminate_notifier;
    const TraceClientId client_id{1};

    EXPECT_CALL(*mock_object_factory_, CreateTraceJobProcessor)
        .WillOnce([this](const ClientIdContainer&,
                         std::shared_ptr<TraceJobContainer>,
                         TraceJobDeallocator deallocator_function,
                         const score::cpp::stop_token&) {
            callback_ = std::move(deallocator_function);
            return std::move(mock_trace_job_processor_);
        });
    EXPECT_CALL(*mock_object_factory_, CreateDaemonCommunicator).WillOnce([this]() {
        return std::move(mock_daemon_communicator_ptr_);
    });
    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, SubscribeToDaemonTerminationNotification)
        .WillRepeatedly(Invoke([this](IDaemonCommunicator::DaemonTerminationCallback callback) -> void {
            daemon_crash_callback_ = std::move(callback);
        }));
    EXPECT_CALL(*mock_unistd_, getpid()).WillOnce(Return(pid_));
    EXPECT_CALL(*mock_shared_memory_factory_, SetTypedMemoryProvider(_));
    EXPECT_CALL(*mock_shared_memory_factory_, Create).WillOnce(Return(mock_shared_memory_resource_));
    EXPECT_CALL(*mock_shared_memory_resource_, getBaseAddress)
        .WillOnce(Return(&valid_pointer_));  // just return a valid pointer
    EXPECT_CALL(*mock_shared_memory_resource_, IsShmInTypedMemory).WillOnce(Return(true));
    EXPECT_CALL(*mock_object_factory_, CreateTraceJobAllocator)
        .WillOnce(Return(ByMove(std::move(mock_trace_job_allocator_))));
    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, Connect).WillRepeatedly(Invoke([&]() -> ResultBlank {
        if (!delay_ltpm_daemon.load())
        {
            daemon_notifier.Notify();
            return ResultBlank{};
        }
        else if (stop_source_.get_token().stop_requested())
        {
            terminate_notifier.Notify();
        }
        return MakeUnexpected(ErrorCode::kServerConnectionNameOpenFailedFatal);
    }));

    EXPECT_CALL(*mock_daemon_communicator_ptr_, RegisterSharedMemoryObject(tmd_filename_))
        .WillOnce(Return(tmd_shm_obj_handle_));
    EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, SetTraceMetaDataShmObjectHandle(tmd_shm_obj_handle_));
    EXPECT_CALL(*mock_trace_job_processor_ptr_raw_, ProcessJobs)
        .WillOnce(Invoke([&library_notifier]() {
            library_notifier.Notify();
            return ResultBlank{};
        }))
        .WillRepeatedly(Return(ResultBlank{}));
    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
        EXPECT_CALL(*mock_memory_validator_, IsSharedMemoryTyped(Matcher<std::int32_t>(file_descriptor_)))
            .WillOnce(Return(true));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_,
                    RegisterSharedMemoryObject(Matcher<std::int32_t>(file_descriptor_)))
            .Times(1)
            .WillOnce(Return(shm_obj_handle_));
        EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, AllocateShmJob(client_id, _, _, _, _, context_id_))
            .WillOnce(Return(ResultBlank{}));
        EXPECT_CALL(*mock_trace_job_processor_ptr_raw_, CleanPendingJobs).WillOnce(Return(ResultBlank{}));
        EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, ResetRingBuffer)
            .WillOnce(Invoke([&daemon_crash_callback_notifier, &delay_ltpm_daemon]() {
                delay_ltpm_daemon.store(true);
                daemon_crash_callback_notifier.Notify();
                return;
            }));
    }

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());

    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 50);

    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto register_shm_by_file_descriptor =
        library_->RegisterShmObject(register_client_result.value(), file_descriptor_);
    auto shm_trace_result =
        library_->Trace(register_client_result.value(), meta_info_, shm_data_chunk_list_, context_id_);
    daemon_crash_callback_();
    daemon_crash_callback_notifier.WaitForNotificationWithTimeout(std::chrono::milliseconds(100));
    auto shm_trace_result_after_daemon_disconnection =
        library_->Trace(register_client_result.value(), meta_info_, shm_data_chunk_list_, context_id_);

    RequestLibraryWorkerThreadShutdown(terminate_notifier);

    EXPECT_TRUE(register_shm_by_file_descriptor.has_value());
    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_TRUE(shm_trace_result.has_value());
    EXPECT_FALSE(shm_trace_result_after_daemon_disconnection.has_value());
    EXPECT_EQ(shm_trace_result_after_daemon_disconnection.error(), ErrorCode::kDaemonIsDisconnectedRecoverable);
}

TEST_F(GenericTraceAPIImplFixture, TraceLocalDataClientNotFound)
{
    RecordProperty("Verifies", "SCR-39691016, SCR-39688772");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if Trace() API returns recoverable error when invalid trace client ID is given");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};
    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
    ExpectCorrectCleaningUp(library_notifier);

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);
    library_notifier.Reset();
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    EXPECT_TRUE(register_client_result.has_value());
    auto local_trace_result = library_->Trace(
        static_cast<TraceClientId>(register_client_result.value() + 1), meta_info_, local_data_chunk_list_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_EQ(local_trace_result.error(), ErrorCode::kClientNotFoundRecoverable);
}

TEST_F(GenericTraceAPIImplFixture, TraceLocalDataDaemonNotConnected)
{
    RecordProperty("Verifies", "SCR-39691333, SCR-39688772");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if Trace() API returns recoverable error when Daemon is not available");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};
    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
    ExpectCorrectCleaningUp(library_notifier);

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);
    library_notifier.Reset();
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    RequestLibraryWorkerThreadShutdown(library_notifier);
    auto local_trace_result =
        library_->Trace(static_cast<TraceClientId>(register_client_result.value()), meta_info_, local_data_chunk_list_);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_FALSE(local_trace_result.has_value());
    EXPECT_EQ(local_trace_result.error(), ErrorCode::kDaemonNotConnectedRecoverable);
}

TEST_F(GenericTraceAPIImplFixture, TraceLocalDataSuccess)
{
    RecordProperty("Verifies", "SCR-32734879");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if Trace() API does a copy of LocalDataChunkList and share it with backend");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};
    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
        EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, AllocateLocalJob(client_id, _, _, _, _))
            .WillOnce(Invoke([this](const TraceClientId,
                                    const MetaInfoVariants::type&,
                                    const BindingType,
                                    const AppIdType&,
                                    LocalDataChunkList& data) {
                EXPECT_EQ(local_data_chunk_list_, data);
                return ResultBlank{};
            }));
    }
    ExpectCorrectCleaningUp(library_notifier);

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);
    library_notifier.Reset();
    auto register_client_result = library_->RegisterClient(BindingType::kVector, app_instance_id_);
    EXPECT_TRUE(register_client_result.has_value());
    auto local_trace_result = library_->Trace(register_client_result.value(), meta_info_, local_data_chunk_list_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(local_trace_result.has_value());
}

TEST_F(GenericTraceAPIImplFixture, SuccessfulTraceAfterSecondConnectionWithTheDaemon)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    PromiseNotifier second_round_library_notifier;
    PromiseNotifier terminate_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};

    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillRepeatedly(Return(client_id));
    EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, AllocateLocalJob(client_id, _, _, _, _))
        .WillRepeatedly(Invoke([this](const TraceClientId,
                                      const MetaInfoVariants::type&,
                                      const BindingType,
                                      const AppIdType&,
                                      LocalDataChunkList& data) {
            EXPECT_EQ(local_data_chunk_list_, data);
            return ResultBlank{};
        }));

    ExpectCorrectCleaningUp(terminate_notifier);

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    library_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);
    auto register_client_result = library_->RegisterClient(BindingType::kVector, app_instance_id_);
    EXPECT_TRUE(register_client_result.has_value());
    auto local_trace_result = library_->Trace(register_client_result.value(), meta_info_, local_data_chunk_list_);
    // Expect second initialization sequence after the daemon is connected again
    {
        InSequence sequence;
        EXPECT_CALL(*mock_trace_job_processor_ptr_raw_, CleanPendingJobs).WillOnce(Return(ResultBlank{}));
        EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, ResetRingBuffer);
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, Connect).WillRepeatedly(Invoke([&]() -> ResultBlank {
            return ResultBlank{};
        }));

        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterSharedMemoryObject(tmd_filename_))
            .WillOnce(Return(tmd_shm_obj_handle_));
        EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, SetTraceMetaDataShmObjectHandle(tmd_shm_obj_handle_));
        EXPECT_CALL(*mock_trace_job_processor_ptr_raw_, ProcessJobs).WillRepeatedly(Invoke([&]() {
            static std::uint8_t times = 0U;
            times++;
            if (times == 1)
            {
                second_round_library_notifier.Notify();
            }
            return ResultBlank{};
        }));
    }
    // The daemon should be disconnected
    daemon_crash_callback_();

    second_round_library_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);
    auto second_trace_result = library_->Trace(register_client_result.value(), meta_info_, local_data_chunk_list_);
    RequestLibraryWorkerThreadShutdown(terminate_notifier);

    EXPECT_TRUE(local_trace_result.has_value());
    EXPECT_TRUE(second_trace_result.has_value());
}

TEST_F(GenericTraceAPIImplFixture, TestTraceWithNoReadyLibrary)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_not_ready_notifier;
    EXPECT_CALL(*mock_object_factory_, CreateTraceJobProcessor)
        .WillOnce(Return(ByMove(std::move(mock_trace_job_processor_))));
    {
        InSequence sequence;
        EXPECT_CALL(*mock_object_factory_, CreateDaemonCommunicator).WillOnce([this]() {
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
            .WillOnce(Return(ByMove(score::MakeUnexpected(ErrorCode::kRingBufferInvalidMemoryResourceRecoverable))));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, Connect).WillOnce([]() {
            return ResultBlank{};
        });
        EXPECT_CALL(*mock_daemon_communicator_ptr_, RegisterSharedMemoryObject(tmd_filename_))
            .WillOnce(Invoke([&daemon_notifier]() -> score::Result<ShmObjectHandle> {
                daemon_notifier.Notify();
                return tmd_shm_obj_handle_;
            }));

        EXPECT_CALL(*mock_trace_job_processor_ptr_raw_, ProcessJobs).WillRepeatedly(Return(ResultBlank{}));
    }

    const TraceClientId client_id{1};

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());

    daemon_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);
    library_not_ready_notifier.WaitForNotificationWithTimeout(std::chrono::milliseconds(50));
    library_not_ready_notifier.Reset();

    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    EXPECT_FALSE(register_client_result.has_value());
    EXPECT_EQ(register_client_result.error(), ErrorCode::kTraceJobAllocatorInitializationFailedFatal);
    auto local_trace_result = library_->Trace(client_id, meta_info_, local_data_chunk_list_);
    EXPECT_FALSE(local_trace_result.has_value());
    EXPECT_EQ(local_trace_result.error(), ErrorCode::kTraceJobAllocatorInitializationFailedFatal);
    library_not_ready_notifier.WaitForNotificationWithTimeout(std::chrono::milliseconds(50));
    stop_source_.request_stop();
}

TEST_F(GenericTraceAPIImplFixture, TraceShmFailInvalidClientID)
{
    RecordProperty("Verifies", "SCR-39691016, SCR-39688772");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if Trace() API returns recoverable error when invalid trace client ID is given");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PromiseNotifier daemon_connection_failed_notifier;
    EXPECT_CALL(*mock_object_factory_, CreateTraceJobProcessor)
        .WillOnce([this](const ClientIdContainer&,
                         std::shared_ptr<TraceJobContainer>,
                         TraceJobDeallocator deallocator_function,
                         const score::cpp::stop_token&) {
            callback_ = std::move(deallocator_function);
            return std::move(mock_trace_job_processor_);
        });
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
        .WillOnce(Invoke([&daemon_connection_failed_notifier]() {
            daemon_connection_failed_notifier.Notify();
            return MakeUnexpected(ErrorCode::kDaemonNotConnectedFatal);
        }))
        .WillRepeatedly(Return(score::MakeUnexpected(ErrorCode::kDaemonNotConnectedFatal)));

    ON_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient)
        .WillByDefault(Return(MakeUnexpected(ErrorCode::kMessageSendFailedRecoverable)));

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto shm_trace_result =
        library_->Trace(register_client_result.value(), meta_info_, shm_data_chunk_list_, context_id_);
    RequestLibraryWorkerThreadShutdown(daemon_connection_failed_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_FALSE(shm_trace_result.has_value());
    EXPECT_EQ(shm_trace_result.error(), ErrorCode::kDaemonNotConnectedRecoverable);
}

TEST_F(GenericTraceAPIImplFixture, TraceLocalFailInvalidClientID)
{
    RecordProperty("Verifies", "SCR-39691016, SCR-39688772");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if Trace() API returns recoverable error when invalid trace client ID is given");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PromiseNotifier daemon_connection_failed_notifier;
    EXPECT_CALL(*mock_object_factory_, CreateTraceJobProcessor)
        .WillOnce([this](const ClientIdContainer&,
                         std::shared_ptr<TraceJobContainer>,
                         TraceJobDeallocator deallocator_function,
                         const score::cpp::stop_token&) {
            callback_ = std::move(deallocator_function);
            return std::move(mock_trace_job_processor_);
        });
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
        .WillOnce(Invoke([&daemon_connection_failed_notifier]() {
            daemon_connection_failed_notifier.Notify();
            return MakeUnexpected(ErrorCode::kDaemonNotConnectedFatal);
        }))
        .WillRepeatedly(Return(score::MakeUnexpected(ErrorCode::kDaemonNotConnectedFatal)));

    ON_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient)
        .WillByDefault(Return(MakeUnexpected(ErrorCode::kMessageSendFailedRecoverable)));

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto local_trace_result = library_->Trace(register_client_result.value(), meta_info_, local_data_chunk_list_);
    RequestLibraryWorkerThreadShutdown(daemon_connection_failed_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_FALSE(local_trace_result.has_value());
    EXPECT_EQ(local_trace_result.error(), ErrorCode::kDaemonNotConnectedRecoverable);
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
