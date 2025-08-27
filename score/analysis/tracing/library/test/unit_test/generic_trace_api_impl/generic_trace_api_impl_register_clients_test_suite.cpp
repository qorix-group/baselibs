#include "score/analysis/tracing/library/test/unit_test/generic_trace_api_impl/generic_trace_api_impl_test.h"

#include <future>

namespace score
{
namespace analysis
{
namespace tracing
{

TEST_F(GenericTraceAPIImplFixture, RegisterClientEmptyAppInstanceId)
{
    RecordProperty("Verifies", "SCR-39683296, SCR-39683351");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if RegisterClient() API returns error when empty instance id is given");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
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
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, std::string{});
    stop_source_.request_stop();
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_FALSE(register_client_result.has_value());
    EXPECT_EQ(register_client_result.error(), ErrorCode::kInvalidArgumentFatal);
}

TEST_F(GenericTraceAPIImplFixture, RegisterClientInvalidBindingType)
{
    RecordProperty("Verifies", "SCR-39683215, SCR-39683351");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if RegisterClient() API returns error when invalid binding type is given");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
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
    auto register_client_result = library_->RegisterClient(static_cast<BindingType>(UINT8_MAX), app_instance_id_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_FALSE(register_client_result.has_value());
    EXPECT_EQ(register_client_result.error(), ErrorCode::kInvalidArgumentFatal);
}

TEST_F(GenericTraceAPIImplFixture, RegisterClientDaemonNotConnected)
{
    PromiseNotifier daemon_not_connected_notifier;
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
        .WillOnce([&daemon_not_connected_notifier]() {
            daemon_not_connected_notifier.Notify();
            return MakeUnexpected(ErrorCode::kServerConnectionNameOpenFailedFatal);
        })
        .WillRepeatedly([]() {
            return MakeUnexpected(ErrorCode::kServerConnectionNameOpenFailedFatal);
        });
    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).Times(0);

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());

    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    EXPECT_TRUE(register_client_result.has_value());

    RequestLibraryWorkerThreadShutdown(daemon_not_connected_notifier);
}

TEST_F(GenericTraceAPIImplFixture, RegisterClientMessageSendFailure)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient)
            .WillOnce(Return(MakeUnexpected(ErrorCode::kMessageSendFailedRecoverable)));
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

    EXPECT_FALSE(register_client_result.has_value());
    EXPECT_EQ(register_client_result.error(), ErrorCode::kMessageSendFailedRecoverable);
    RequestLibraryWorkerThreadShutdown(library_notifier);
}

TEST_F(GenericTraceAPIImplFixture, RegisterLocalClientDaemonNotReady)
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
    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto request_shutdown = std::async(std::launch::async, [this, &library_shutdown_notifier]() {
        std::this_thread::sleep_for(kLibraryCheckPeriodicity * 10);
        RequestLibraryWorkerThreadShutdown(library_shutdown_notifier);
    });
    daemon_not_connected_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);
    request_shutdown.get();

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_EQ(register_client_result.value(), client_id);
}

TEST_F(GenericTraceAPIImplFixture, RegisterLtpmDaemonClientDaemonOnly)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};
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
    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_EQ(register_client_result.value(), client_id);
    RequestLibraryWorkerThreadShutdown(library_notifier);
}

TEST_F(GenericTraceAPIImplFixture, RegisterClientDaemonNeverReady)
{
    PromiseNotifier daemon_not_connected_notifier;
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
        .WillRepeatedly(Return(score::MakeUnexpected(ErrorCode::kDaemonNotConnectedFatal)));

    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).Times(0);
    constexpr auto kStopRequestDelay{std::chrono::milliseconds{10}};
    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());

    auto stop_request_future =
        std::async(std::launch::async, [this, kStopRequestDelay, &daemon_not_connected_notifier]() noexcept {
            std::this_thread::sleep_for(kStopRequestDelay);
            stop_source_.request_stop();
            std::this_thread::sleep_for(kStopRequestDelay);
            daemon_not_connected_notifier.Notify();
        });
    stop_request_future.get();
    daemon_not_connected_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);

    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    EXPECT_FALSE(register_client_result.has_value());
    EXPECT_EQ(register_client_result.error(), ErrorCode::kDaemonNotAvailableFatal);
}

TEST_F(GenericTraceAPIImplFixture, RegisterLtpmDaemonClientSuccess)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};
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
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_EQ(register_client_result.value(), client_id);
}

TEST_F(GenericTraceAPIImplFixture, RegisterClientAlreadyRegistered)
{
    RecordProperty("Verifies", "SCR-39683555, SCR-39683516");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if RegisterClient() API returns same client ID when registration of already "
                   "registered client is done and if only 8 first bytes of client ID are considered");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const std::string client_name_a{"ClientNameA"};
    const std::string client_name_b{"ClientNameB"};
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};
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
    auto register_client_a_result = library_->RegisterClient(BindingType::kVectorZeroCopy, client_name_a);
    auto register_client_b_result = library_->RegisterClient(BindingType::kVectorZeroCopy, client_name_b);
    EXPECT_TRUE(register_client_a_result.has_value());
    EXPECT_TRUE(register_client_b_result.has_value());
    EXPECT_EQ(register_client_a_result.value(), register_client_b_result.value());

    RequestLibraryWorkerThreadShutdown(library_notifier);
}

TEST_F(GenericTraceAPIImplFixture, RegisterSameAppInstanceIdDifferentBindingType)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId first_client_id{1};
    const TraceClientId second_client_id{2};
    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient)
            .Times(2)
            .WillOnce(Return(first_client_id))
            .WillOnce(Return(second_client_id));
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
    register_client_result = library_->RegisterClient(BindingType::kVector, app_instance_id_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_EQ(register_client_result.value(), second_client_id);
}

TEST_F(GenericTraceAPIImplFixture, RegisterShallFailWhenExceedingMaxNumberOfClients)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};
    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillRepeatedly(Return(client_id));
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

    for (unsigned int client = 0; client < kClientIdContainerSize; client++)
    {
        register_client_result =
            library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_ + std::to_string(client));
    }
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_FALSE(register_client_result.has_value());
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
