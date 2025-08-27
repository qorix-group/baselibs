#include "score/analysis/tracing/library/test/unit_test/generic_trace_api_impl/generic_trace_api_impl_test.h"

#include <future>

namespace score
{
namespace analysis
{
namespace tracing
{

TEST_F(GenericTraceAPIImplFixture, RegisterShmObjectEmptyPath)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};
    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillOnce(Return(ResultBlank{}));
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
    auto register_shm_by_path_result = library_->RegisterShmObject(client_id, std::string{});
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_FALSE(register_shm_by_path_result.has_value());
    EXPECT_EQ(register_shm_by_path_result.error(), ErrorCode::kInvalidArgumentFatal);
}

TEST_F(GenericTraceAPIImplFixture, RegisterShmObjectPathClientNotFound)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};
    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
        EXPECT_CALL(*mock_memory_validator_, GetFileDescriptorFromMemoryPath(Matcher<const std::string&>(path_)))
            .WillOnce(Return(file_descriptor_));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillOnce(Return(ResultBlank{}));
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
    auto register_shm_by_path_result =
        library_->RegisterShmObject(static_cast<TraceClientId>(register_client_result.value() + 1), path_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_FALSE(register_shm_by_path_result.has_value());
    EXPECT_EQ(register_shm_by_path_result.error(), ErrorCode::kClientNotFoundRecoverable);
}

TEST_F(GenericTraceAPIImplFixture, RegisterShmMemoryPathWithBadFileDescriptor)
{
    std::atomic<bool> delay_ltpm_daemon = true;
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    PromiseNotifier terminate_notifier;

    ExpectCorrectInitialization(daemon_notifier, library_notifier, delay_ltpm_daemon);
    const TraceClientId client_id{1};
    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
        EXPECT_CALL(*mock_memory_validator_, GetFileDescriptorFromMemoryPath(Matcher<const std::string&>(path_)))
            .WillOnce(Return(MakeUnexpected(ErrorCode::kBadFileDescriptorFatal)));
    }
    ExpectCorrectCleaningUp(terminate_notifier);

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto register_shm_by_path_result =
        library_->RegisterShmObject(static_cast<TraceClientId>(register_client_result.value()), path_);
    RequestLibraryWorkerThreadShutdown(terminate_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_FALSE(register_shm_by_path_result.has_value());
    EXPECT_EQ(register_shm_by_path_result.error(), ErrorCode::kBadFileDescriptorFatal);
}

TEST_F(GenericTraceAPIImplFixture, RegisterShmObjectPathDaemonNotConnected)
{
    RecordProperty("Verifies", "SCR-39687759");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if RegisterSharedMemoryObject() forwards request to tracing backend and returns "
                   "unique handle even when backend is not available");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PromiseNotifier daemon_not_connected_notifier;
    PromiseNotifier library_shutdown_notifier;
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
        .WillOnce([&daemon_not_connected_notifier, &library_shutdown_notifier]() {
            daemon_not_connected_notifier.Notify();
            library_shutdown_notifier.Notify();
            return MakeUnexpected(ErrorCode::kDaemonNotConnectedFatal);
        })
        .WillRepeatedly(Return(score::MakeUnexpected(ErrorCode::kDaemonNotConnectedFatal)));

    EXPECT_CALL(*mock_memory_validator_, GetFileDescriptorFromMemoryPath(Matcher<const std::string&>(path_)))
        .WillOnce(Return(file_descriptor_));
    EXPECT_CALL(*mock_memory_validator_, IsSharedMemoryTyped(Matcher<std::int32_t>(file_descriptor_)))
        .WillOnce(Return(true));

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());

    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto register_shm_by_path_result = library_->RegisterShmObject(register_client_result.value(), path_);
    auto request_shutdown = std::async(std::launch::async, [this, &library_shutdown_notifier]() {
        std::this_thread::sleep_for(kLibraryCheckPeriodicity * 10);
        RequestLibraryWorkerThreadShutdown(library_shutdown_notifier);
    });
    daemon_not_connected_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);
    request_shutdown.get();

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_EQ(register_client_result.value(), client_id);
    EXPECT_TRUE(register_shm_by_path_result.has_value());
}

TEST_F(GenericTraceAPIImplFixture, RegisterShmObjectPathOnlyDaemonReadySuccess)
{
    RecordProperty("Verifies", "SCR-39687759");
    RecordProperty("ASIL", "QM");
    RecordProperty(
        "Description",
        "Test verifies if RegisterSharedMemoryObject() forwards request to tracing backend and returns unique handle");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    PromiseNotifier terminate_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};

    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));

        EXPECT_CALL(*mock_memory_validator_, GetFileDescriptorFromMemoryPath(Matcher<const std::string&>(path_)))
            .WillOnce(Return(file_descriptor_));
        EXPECT_CALL(*mock_memory_validator_, IsSharedMemoryTyped(Matcher<std::int32_t>(file_descriptor_)))
            .WillOnce(Return(true));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_,
                    RegisterSharedMemoryObject(Matcher<std::int32_t>(file_descriptor_)))
            .WillOnce(Return(shm_obj_handle_));
    }
    ExpectCorrectCleaningUp(terminate_notifier);

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    daemon_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto register_shm_by_path_result = library_->RegisterShmObject(register_client_result.value(), path_);

    RequestLibraryWorkerThreadShutdown(terminate_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_TRUE(register_shm_by_path_result.has_value());
}

TEST_F(GenericTraceAPIImplFixture, RegisterShmObjectPathLtpmDaemonCommunicationFailed)
{
    RecordProperty("Verifies", "SCR-39687873");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if RegisterSharedMemoryObject() API returns error when Library fails to register it "
                   "in backend.");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};

    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));

        EXPECT_CALL(*mock_memory_validator_, GetFileDescriptorFromMemoryPath(Matcher<const std::string&>(path_)))
            .WillOnce(Return(file_descriptor_));
        EXPECT_CALL(*mock_memory_validator_, IsSharedMemoryTyped(Matcher<std::int32_t>(file_descriptor_)))
            .WillOnce(Return(true));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_,
                    RegisterSharedMemoryObject(Matcher<std::int32_t>(file_descriptor_)))
            .WillOnce(Return(MakeUnexpected<ShmObjectHandle>(ErrorCode::kMessageSendFailedRecoverable)));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillOnce(Return(ResultBlank{}));
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
    auto register_shm_by_path_result = library_->RegisterShmObject(register_client_result.value(), path_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_FALSE(register_shm_by_path_result.has_value());
    EXPECT_EQ(register_shm_by_path_result.error(), ErrorCode::kMessageSendFailedRecoverable);
}

TEST_F(GenericTraceAPIImplFixture, RegisterShmObjectPathNotTypedMemoryCheckFailed)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};

    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));

        EXPECT_CALL(*mock_memory_validator_, GetFileDescriptorFromMemoryPath(Matcher<const std::string&>(path_)))
            .WillOnce(Return(file_descriptor_));
        EXPECT_CALL(*mock_memory_validator_, IsSharedMemoryTyped(Matcher<std::int32_t>(file_descriptor_)))
            .WillOnce(Return(MakeUnexpected(ErrorCode::kSharedMemoryObjectFlagsRetrievalFailedFatal)));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillOnce(Return(ResultBlank{}));
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
    auto register_shm_by_path_result = library_->RegisterShmObject(register_client_result.value(), path_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_FALSE(register_shm_by_path_result.has_value());
    EXPECT_EQ(register_shm_by_path_result.error(), ErrorCode::kSharedMemoryObjectFlagsRetrievalFailedFatal);
}

TEST_F(GenericTraceAPIImplFixture, RegisterShmObjectPathNotInTypedMemory)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};

    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));

        EXPECT_CALL(*mock_memory_validator_, GetFileDescriptorFromMemoryPath(Matcher<const std::string&>(path_)))
            .WillOnce(Return(file_descriptor_));
        EXPECT_CALL(*mock_memory_validator_, IsSharedMemoryTyped(Matcher<std::int32_t>(file_descriptor_)))
            .WillOnce(Return(false));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillOnce(Return(ResultBlank{}));
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
    auto register_shm_by_path_result = library_->RegisterShmObject(register_client_result.value(), path_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_FALSE(register_shm_by_path_result.has_value());
    EXPECT_EQ(register_shm_by_path_result.error(), ErrorCode::kSharedMemoryObjectNotInTypedMemoryFatal);
}

TEST_F(GenericTraceAPIImplFixture, RegisterShmObjectPathSuccess)
{
    RecordProperty("Verifies", "SCR-39687759");
    RecordProperty("ASIL", "QM");
    RecordProperty(
        "Description",
        "Test verifies if RegisterSharedMemoryObject() forwards request to tracing backend and returns unique handle");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};

    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));

        EXPECT_CALL(*mock_memory_validator_, GetFileDescriptorFromMemoryPath(Matcher<const std::string&>(path_)))
            .WillOnce(Return(file_descriptor_));
        EXPECT_CALL(*mock_memory_validator_, IsSharedMemoryTyped(Matcher<std::int32_t>(file_descriptor_)))
            .WillOnce(Return(true));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_,
                    RegisterSharedMemoryObject(Matcher<std::int32_t>(file_descriptor_)))
            .WillOnce(Return(shm_obj_handle_));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillOnce(Return(ResultBlank{}));
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
    auto register_shm_by_path_result = library_->RegisterShmObject(register_client_result.value(), path_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_TRUE(register_shm_by_path_result.has_value());
}

TEST_F(GenericTraceAPIImplFixture, RegisterShmObjectDuplicatePath)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};

    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));

    EXPECT_CALL(*mock_memory_validator_, GetFileDescriptorFromMemoryPath(Matcher<const std::string&>(path_)))
        .WillRepeatedly(Return(file_descriptor_));
    EXPECT_CALL(*mock_memory_validator_, IsSharedMemoryTyped(Matcher<std::int32_t>(file_descriptor_)))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterSharedMemoryObject(file_descriptor_))
        .Times(1)
        .WillOnce(Return(shm_obj_handle_));
    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
        .WillOnce(Return(ResultBlank{}));
    EXPECT_CALL(*mock_shared_memory_factory_, Remove(tmd_filename_)).WillOnce(Invoke([&library_notifier]() {
        library_notifier.Notify();
    }));
    EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, CloseRingBuffer);

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);
    library_notifier.Reset();
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto register_shm_by_path_result = library_->RegisterShmObject(register_client_result.value(), path_);
    register_shm_by_path_result = library_->RegisterShmObject(register_client_result.value(), path_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_FALSE(register_shm_by_path_result.has_value());
    EXPECT_EQ(register_shm_by_path_result.error(), ErrorCode::kSharedMemoryObjectAlreadyRegisteredRecoverable);
}

TEST_F(GenericTraceAPIImplFixture, RegisterShmObjectInvalidFileDescriptor)
{
    RecordProperty("Verifies", "SCR-39688657");
    RecordProperty("ASIL", "QM");
    RecordProperty(
        "Description",
        "Test verifies if RegisterShmObject() API returns error when unregistered handle is given as a param");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};
    Result<ShmObjectHandle> invalid_file_descriptor_result(MakeUnexpected(ErrorCode::kInvalidArgumentFatal));

    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillOnce(Return(ResultBlank{}));
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
    auto register_shm_by_file_descriptor = library_->RegisterShmObject(
        static_cast<TraceClientId>(register_client_result.value() + 1), invalid_file_descriptor_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_EQ(register_shm_by_file_descriptor.error(), ErrorCode::kInvalidArgumentFatal);
}

TEST_F(GenericTraceAPIImplFixture, RegisterShmObjectFileDescriptorClientNotFound)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};
    Result<ShmObjectHandle> invalid_file_descriptor_result(MakeUnexpected(ErrorCode::kInvalidArgumentFatal));

    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillOnce(Return(ResultBlank{}));
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
        library_->RegisterShmObject(static_cast<TraceClientId>(register_client_result.value() + 1), file_descriptor_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_EQ(register_shm_by_file_descriptor.error(), ErrorCode::kClientNotFoundRecoverable);
}

TEST_F(GenericTraceAPIImplFixture, RegisterShmObjectFileDescriptorDaemonNotConnected)
{
    PromiseNotifier daemon_not_connected_notifier;
    PromiseNotifier library_shutdown_notifier;
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
        .WillOnce([&daemon_not_connected_notifier, &library_shutdown_notifier]() {
            daemon_not_connected_notifier.Notify();
            library_shutdown_notifier.Notify();
            return MakeUnexpected(ErrorCode::kDaemonNotConnectedFatal);
        })
        .WillRepeatedly(Return(score::MakeUnexpected(ErrorCode::kDaemonNotConnectedFatal)));
    EXPECT_CALL(*mock_memory_validator_, IsSharedMemoryTyped(Matcher<std::int32_t>(file_descriptor_)))
        .WillOnce(Return(true));

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto register_shm_by_file_descriptor = library_->RegisterShmObject(client_id, file_descriptor_);
    auto request_shutdown = std::async(std::launch::async, [this, &library_shutdown_notifier]() {
        std::this_thread::sleep_for(kLibraryCheckPeriodicity * 10);
        RequestLibraryWorkerThreadShutdown(library_shutdown_notifier);
    });
    daemon_not_connected_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);
    request_shutdown.get();

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_EQ(register_client_result.value(), client_id);
    EXPECT_TRUE(register_shm_by_file_descriptor.has_value());
}

TEST_F(GenericTraceAPIImplFixture, RegisterShmObjectFileDescriptorOnlyDaemonReadySuccess)
{
    RecordProperty("Verifies", "SCR-39687759");
    RecordProperty("ASIL", "QM");
    RecordProperty(
        "Description",
        "Test verifies if RegisterSharedMemoryObject() forwards request to tracing backend and returns unique handle");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    PromiseNotifier terminate_notifier;
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
    ExpectCorrectCleaningUp(terminate_notifier);
    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    daemon_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto register_shm_by_file_descriptor = library_->RegisterShmObject(client_id, file_descriptor_);

    RequestLibraryWorkerThreadShutdown(terminate_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_TRUE(register_shm_by_file_descriptor.has_value());
}

TEST_F(GenericTraceAPIImplFixture, RegisterShmObjectFileDescriptorLtpmDaemonCommunicationFailed)
{
    RecordProperty("Verifies", "SCR-39687873");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if RegisterSharedMemoryObject() API returns error when Library fails to register it "
                   "in backend.");
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
            .WillOnce(Return(MakeUnexpected<ShmObjectHandle>(ErrorCode::kMessageSendFailedRecoverable)));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillOnce(Return(ResultBlank{}));
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
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_FALSE(register_shm_by_file_descriptor.has_value());
    EXPECT_EQ(register_shm_by_file_descriptor.error(), ErrorCode::kMessageSendFailedRecoverable);
}

TEST_F(GenericTraceAPIImplFixture, RegisterShmObjectFileDescriptorTypedMemoryCheckFailed)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};

    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
        EXPECT_CALL(*mock_memory_validator_, IsSharedMemoryTyped(Matcher<std::int32_t>(file_descriptor_)))
            .WillOnce(Return(MakeUnexpected(ErrorCode::kSharedMemoryObjectFlagsRetrievalFailedFatal)));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillOnce(Return(ResultBlank{}));
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
    auto register_shm_by_file_descriptor = library_->RegisterShmObject(client_id, file_descriptor_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_FALSE(register_shm_by_file_descriptor.has_value());
    EXPECT_EQ(register_shm_by_file_descriptor.error(), ErrorCode::kSharedMemoryObjectFlagsRetrievalFailedFatal);
}

TEST_F(GenericTraceAPIImplFixture, RegisterShmObjectFileDescriptorNotInTypedMemory)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};

    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
        EXPECT_CALL(*mock_memory_validator_, IsSharedMemoryTyped(Matcher<std::int32_t>(file_descriptor_)))
            .WillOnce(Return(false));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillOnce(Return(ResultBlank{}));
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
    auto register_shm_by_file_descriptor = library_->RegisterShmObject(client_id, file_descriptor_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_FALSE(register_shm_by_file_descriptor.has_value());
    EXPECT_EQ(register_shm_by_file_descriptor.error(), ErrorCode::kSharedMemoryObjectNotInTypedMemoryFatal);
}

TEST_F(GenericTraceAPIImplFixture, RegisterShmObjectFileDescriptorSuccess)
{
    RecordProperty("Verifies", "SCR-39687759");
    RecordProperty("ASIL", "QM");
    RecordProperty(
        "Description",
        "Test verifies if RegisterSharedMemoryObject() forwards request to tracing backend and returns unique handle");
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
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillOnce(Return(ResultBlank{}));
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
    auto register_shm_by_file_descriptor = library_->RegisterShmObject(client_id, file_descriptor_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_TRUE(register_shm_by_file_descriptor.has_value());
}

TEST_F(GenericTraceAPIImplFixture, RegisterShmObjectDuplicateFileDescriptor)
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
    auto register_shm_by_file_descriptor = library_->RegisterShmObject(client_id, file_descriptor_);
    register_shm_by_file_descriptor = library_->RegisterShmObject(client_id, file_descriptor_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_FALSE(register_shm_by_file_descriptor.has_value());
    EXPECT_EQ(register_shm_by_file_descriptor.error(), ErrorCode::kSharedMemoryObjectAlreadyRegisteredRecoverable);
}

TEST_F(GenericTraceAPIImplFixture, RegisterShmObjectsShallFailWhenExceedingMaxNumberOfObjects)
{
    const TraceClientId client_id{1};
    auto nice_mock_memory_validator_ = std::make_unique<NiceMock<MockMemoryValidator>>();
    ON_CALL(*nice_mock_memory_validator_, IsSharedMemoryTyped(An<std::int32_t>())).WillByDefault(Return(true));
    ON_CALL(*nice_mock_memory_validator_, IsSharedMemoryTyped(An<const std::string&>())).WillByDefault(Return(true));
    ON_CALL(*nice_mock_memory_validator_, GetFileDescriptorFromMemoryPath(An<const std::string&>()))
        .WillByDefault(Return(kShmObjectHandleContainerSize + 1));

    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillRepeatedly(Return(client_id));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterSharedMemoryObject(An<std::int32_t>()))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(shm_obj_handle_));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillRepeatedly(Return(ResultBlank{}));
        EXPECT_CALL(*mock_shared_memory_factory_, Remove(tmd_filename_)).WillOnce(Invoke([&library_notifier]() {
            library_notifier.Notify();
        }));
        EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, CloseRingBuffer);
    }

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(nice_mock_memory_validator_),
                                                          stop_source_.get_token());
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);
    library_notifier.Reset();
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    std::int32_t current_file_descriptor = file_descriptor_;
    auto register_shm_by_file_descriptor = library_->RegisterShmObject(client_id, file_descriptor_);
    for (std::size_t shm_object = 0; shm_object < kShmObjectHandleContainerSize - 1; shm_object++)
    {
        register_shm_by_file_descriptor = library_->RegisterShmObject(client_id, ++current_file_descriptor);
    }
    register_shm_by_file_descriptor = library_->RegisterShmObject(client_id, current_file_descriptor + 1);
    const auto register_shm_by_path = library_->RegisterShmObject(client_id, path_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_EQ(register_client_result.value(), client_id);
    EXPECT_FALSE(register_shm_by_file_descriptor.has_value());
    EXPECT_EQ(register_shm_by_file_descriptor.error(), ErrorCode::kNoMoreSpaceForNewShmObjectFatal);
    EXPECT_FALSE(register_shm_by_path.has_value());
    EXPECT_EQ(register_shm_by_path.error(), ErrorCode::kNoMoreSpaceForNewShmObjectFatal);
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
