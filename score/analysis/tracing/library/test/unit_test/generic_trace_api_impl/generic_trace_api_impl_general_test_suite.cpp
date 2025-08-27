#include "score/analysis/tracing/library/test/unit_test/generic_trace_api_impl/generic_trace_api_impl_test.h"

#include <future>

namespace score
{
namespace analysis
{
namespace tracing
{

constexpr std::int32_t kValidFileDescriptor = 0x04;

TEST_F(GenericTraceAPIImplFixture, TestTraceShallFailWhenRegisteringTMDFails)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
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
        .WillOnce(Invoke([&daemon_notifier]() {
            daemon_notifier.Notify();
            return ResultBlank{};
        }))
        .WillRepeatedly([]() {
            return ResultBlank{};
        });
    EXPECT_CALL(*mock_daemon_communicator_ptr_, RegisterSharedMemoryObject(tmd_filename_))
        .WillOnce(Invoke([&library_notifier]() {
            library_notifier.Notify();
            return score::MakeUnexpected(ErrorCode::kDaemonNotConnectedFatal);
        }));

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    daemon_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);
    // wait for setting global error
    std::this_thread::sleep_for(std::chrono::milliseconds{50});
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);

    RequestLibraryWorkerThreadShutdown(library_notifier);
    ASSERT_FALSE(register_client_result.has_value());
    EXPECT_EQ(register_client_result.error(), ErrorCode::kDaemonNotConnectedFatal);
}

TEST_F(GenericTraceAPIImplFixture, RegisterTmdAreaFailed)
{
    RecordProperty("Verifies", "SCR-39691862");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "Test verifies if RegisterClient() API returns error when TDM allocation fails");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    {
        InSequence sequence;
        EXPECT_CALL(*mock_object_factory_, CreateTraceJobProcessor)
            .WillOnce(Return(ByMove(std::move(mock_trace_job_processor_))));
        EXPECT_CALL(*mock_object_factory_, CreateDaemonCommunicator).WillOnce([this]() {
            return std::move(mock_daemon_communicator_ptr_);
        });
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, SubscribeToDaemonTerminationNotification);
        EXPECT_CALL(*mock_unistd_, getpid()).WillOnce(Return(pid_));
        EXPECT_CALL(*mock_shared_memory_factory_, SetTypedMemoryProvider(_));
        EXPECT_CALL(*mock_shared_memory_factory_, Create).WillOnce(Return(nullptr));
        EXPECT_CALL(*mock_shared_memory_factory_, Remove(tmd_filename_)).WillOnce(Invoke([&library_notifier]() {
            library_notifier.Notify();
        }));
        EXPECT_CALL(*mock_object_factory_, CreateTraceJobAllocator)
            .WillOnce(Return(ByMove(std::move(mock_trace_job_allocator_))));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, Connect).WillOnce([&daemon_notifier]() {
            daemon_notifier.Notify();
            return ResultBlank{};
        });
    }

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());

    daemon_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);
    std::this_thread::sleep_for(std::chrono::milliseconds{50});

    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    RequestLibraryWorkerThreadShutdown(library_notifier);
    EXPECT_FALSE(register_client_result.has_value());
    EXPECT_EQ(register_client_result.error(), ErrorCode::kSharedMemoryObjectRegistrationFailedFatal);
}

TEST_F(GenericTraceAPIImplFixture, RegisterTmdAreaNotInSharedMemory)
{
    {
        InSequence sequence;
        EXPECT_CALL(*mock_object_factory_, CreateTraceJobProcessor)
            .WillOnce(Return(ByMove(std::move(mock_trace_job_processor_))));
        EXPECT_CALL(*mock_object_factory_, CreateDaemonCommunicator).WillOnce([this]() {
            return std::move(mock_daemon_communicator_ptr_);
        });
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, SubscribeToDaemonTerminationNotification);
        EXPECT_CALL(*mock_unistd_, getpid()).WillOnce(Return(pid_));
        EXPECT_CALL(*mock_shared_memory_factory_, SetTypedMemoryProvider(_));
        EXPECT_CALL(*mock_shared_memory_factory_, Create).WillOnce(Return(mock_shared_memory_resource_));
        EXPECT_CALL(*mock_shared_memory_resource_, getBaseAddress)
            .WillOnce(Return(&valid_pointer_));  // just return a valid pointer
        EXPECT_CALL(*mock_shared_memory_resource_, IsShmInTypedMemory).WillOnce(Return(false));
        EXPECT_CALL(*mock_shared_memory_factory_, Remove(tmd_filename_)).Times(1);
        EXPECT_CALL(*mock_object_factory_, CreateTraceJobAllocator)
            .WillOnce(Return(ByMove(std::move(mock_trace_job_allocator_))));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, Connect).WillOnce([]() {
            return ResultBlank{};
        });
    }

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
}

TEST_F(GenericTraceAPIImplFixture, TraceJobAllocatorCreationFailed)
{
    {
        InSequence sequence;
        EXPECT_CALL(*mock_object_factory_, CreateTraceJobProcessor)
            .WillOnce(Return(ByMove(std::move(mock_trace_job_processor_))));
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
            .WillOnce(Return(ByMove(MakeUnexpected(ErrorCode::kModuleNotInitializedRecoverable))));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, Connect).WillOnce(Return(ResultBlank{}));
        EXPECT_CALL(*mock_daemon_communicator_ptr_, RegisterSharedMemoryObject(tmd_filename_))
            .WillOnce(Return(tmd_shm_obj_handle_));
    }

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
}

TEST_F(GenericTraceAPIImplFixture, ProcessJobsFailed)
{
    {
        InSequence sequence;
        EXPECT_CALL(*mock_object_factory_, CreateTraceJobProcessor)
            .WillOnce(Return(ByMove(std::move(mock_trace_job_processor_))));
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
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, Connect).WillOnce([]() {
            return ResultBlank{};
        });
        EXPECT_CALL(*mock_daemon_communicator_ptr_, RegisterSharedMemoryObject(tmd_filename_))
            .WillOnce(Return(tmd_shm_obj_handle_));
        EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, SetTraceMetaDataShmObjectHandle(tmd_shm_obj_handle_));
        EXPECT_CALL(*mock_trace_job_processor_ptr_raw_, ProcessJobs)
            .WillRepeatedly(Return(MakeUnexpected(ErrorCode::kNoDeallocatorCallbackRegisteredFatal)));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillOnce(Return(ResultBlank{}));
        EXPECT_CALL(*mock_shared_memory_factory_, Remove(tmd_filename_)).Times(1);
        EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, CloseRingBuffer);
    }

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
}

TEST_F(GenericTraceAPIImplFixture, UnregisterTmdAreaFailed)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};
    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillOnce(Return(MakeUnexpected(ErrorCode::kSharedMemoryObjectUnregisterFailedFatal)));
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
}

TEST_F(GenericTraceAPIImplFixture, TestTooLongPID)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    EXPECT_CALL(*mock_object_factory_, CreateTraceJobProcessor)
        .WillOnce(Return(ByMove(std::move(mock_trace_job_processor_))));
    {
        InSequence sequence;
        EXPECT_CALL(*mock_object_factory_, CreateDaemonCommunicator).WillOnce([this]() {
            return std::move(mock_daemon_communicator_ptr_);
        });
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, SubscribeToDaemonTerminationNotification);
        EXPECT_CALL(*mock_unistd_, getpid()).WillOnce([]() {
            return INT_MAX;
        });
        EXPECT_CALL(*mock_shared_memory_factory_, SetTypedMemoryProvider(_));
        EXPECT_CALL(*mock_shared_memory_factory_, Create).WillOnce(Return(mock_shared_memory_resource_));
        EXPECT_CALL(*mock_shared_memory_resource_, getBaseAddress)
            .WillOnce(Return(&valid_pointer_));  // just return a valid pointer
        EXPECT_CALL(*mock_shared_memory_resource_, IsShmInTypedMemory).WillOnce(Return(true));
        EXPECT_CALL(*mock_object_factory_, CreateTraceJobAllocator)
            .WillOnce(Return(ByMove(score::MakeUnexpected(ErrorCode::kRingBufferInvalidMemoryResourceRecoverable))));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, Connect).WillOnce([&daemon_notifier]() {
            daemon_notifier.Notify();
            return ResultBlank{};
        });
        EXPECT_CALL(*mock_daemon_communicator_ptr_, RegisterSharedMemoryObject("/dev_tmd_2147483647"))
            .WillOnce(Invoke([&library_notifier]() {
                library_notifier.Notify();
                return tmd_shm_obj_handle_;
            }));

        EXPECT_CALL(*mock_trace_job_processor_ptr_raw_, ProcessJobs).WillRepeatedly(Return(ResultBlank{}));
    }

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());

    daemon_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);
    std::this_thread::sleep_for(std::chrono::milliseconds{50});

    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    EXPECT_FALSE(register_client_result.has_value());
    EXPECT_EQ(register_client_result.error(), ErrorCode::kTraceJobAllocatorInitializationFailedFatal);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    stop_source_.request_stop();
}

TEST_F(GenericTraceAPIImplFixture, SharedMemoryCreateReturnsNull)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    EXPECT_CALL(*mock_object_factory_, CreateTraceJobProcessor)
        .WillOnce(Return(ByMove(std::move(mock_trace_job_processor_))));
    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).Times(0);
    EXPECT_CALL(*mock_object_factory_, CreateDaemonCommunicator)
        .WillOnce(Return(ByMove(std::move(mock_daemon_communicator_ptr_))));
    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, SubscribeToDaemonTerminationNotification);
    EXPECT_CALL(*mock_object_factory_, CreateTraceJobAllocator)
        .WillOnce(Return(ByMove(std::move(mock_trace_job_allocator_))));
    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, Connect).WillOnce([&daemon_notifier]() {
        daemon_notifier.Notify();
        return ResultBlank{};
    });
    EXPECT_CALL(*mock_unistd_, getpid()).WillOnce(Return(pid_));
    EXPECT_CALL(*mock_shared_memory_factory_, Create)
        .WillOnce(WithArgs<1>([](score::memory::shared::SharedMemoryFactory::InitializeCallback cb) {
            cb(nullptr);
            return nullptr;
        }));
    EXPECT_CALL(*mock_shared_memory_factory_, SetTypedMemoryProvider(_));
    EXPECT_CALL(*mock_shared_memory_factory_, Remove).WillOnce([&library_notifier]() {
        library_notifier.Notify();
    });
    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());

    daemon_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);

    // wait for setting global error
    std::this_thread::sleep_for(std::chrono::milliseconds{50});

    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    EXPECT_FALSE(register_client_result.has_value());
    EXPECT_EQ(register_client_result.error(), ErrorCode::kSharedMemoryObjectRegistrationFailedFatal);
}

TEST_F(GenericTraceAPIImplFixture, IsShmInTypedMemoryReturnsFalse)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    EXPECT_CALL(*mock_object_factory_, CreateTraceJobProcessor)
        .WillOnce(Return(ByMove(std::move(mock_trace_job_processor_))));
    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).Times(0);
    EXPECT_CALL(*mock_object_factory_, CreateDaemonCommunicator)
        .WillOnce(Return(ByMove(std::move(mock_daemon_communicator_ptr_))));
    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, SubscribeToDaemonTerminationNotification);
    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, Connect).WillOnce([&daemon_notifier]() {
        daemon_notifier.Notify();
        return ResultBlank{};
    });
    EXPECT_CALL(*mock_object_factory_, CreateTraceJobAllocator)
        .WillOnce(Return(ByMove(std::move(mock_trace_job_allocator_))));
    EXPECT_CALL(*mock_unistd_, getpid()).WillOnce(Return(pid_));
    EXPECT_CALL(*mock_shared_memory_resource_, getBaseAddress)
        .WillOnce(Return(&valid_pointer_));  // just return a valid pointer
    EXPECT_CALL(*mock_shared_memory_factory_, Create).WillOnce(Return(mock_shared_memory_resource_));
    EXPECT_CALL(*mock_shared_memory_resource_, IsShmInTypedMemory).WillOnce(Return(false));

    EXPECT_CALL(*mock_shared_memory_factory_, SetTypedMemoryProvider(_));
    EXPECT_CALL(*mock_shared_memory_factory_, Remove).WillOnce([&library_notifier]() {
        library_notifier.Notify();
    });
    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());

    daemon_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);
    // wait for setting global error
    std::this_thread::sleep_for(std::chrono::milliseconds{50});

    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    EXPECT_FALSE(register_client_result.has_value());
    EXPECT_EQ(register_client_result.error(), ErrorCode::kSharedMemoryObjectNotInTypedMemoryFatal);
}

TEST_F(GenericTraceAPIImplFixture, WhenProcessJobsReturnErrorGlobalErrorShallBeSet)
{
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    EXPECT_CALL(*mock_object_factory_, CreateTraceJobProcessor)
        .WillOnce(Return(ByMove(std::move(mock_trace_job_processor_))));
    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).Times(0);
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
        EXPECT_CALL(*mock_shared_memory_resource_, IsShmInTypedMemory).WillOnce([&library_notifier]() {
            library_notifier.Notify();
            return true;
        });
        EXPECT_CALL(*mock_object_factory_, CreateTraceJobAllocator)
            .WillOnce(Return(ByMove(std::move(mock_trace_job_allocator_))));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, Connect).WillOnce([]() {
            return ResultBlank{};
        });
        EXPECT_CALL(*mock_daemon_communicator_ptr_, RegisterSharedMemoryObject(tmd_filename_))
            .WillOnce(Return(tmd_shm_obj_handle_));
        EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, SetTraceMetaDataShmObjectHandle(tmd_shm_obj_handle_));
        EXPECT_CALL(*mock_trace_job_processor_ptr_raw_, ProcessJobs).WillRepeatedly([]() {
            return MakeUnexpected(ErrorCode::kNoDeallocatorCallbackRegisteredFatal);
        });
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject)
            .WillOnce(Invoke([&daemon_notifier]() {
                daemon_notifier.Notify();
                return MakeUnexpected(ErrorCode::kDaemonNotConnectedFatal);
            }));

        EXPECT_CALL(*mock_shared_memory_factory_, Remove);
        EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, CloseRingBuffer);
    }

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());

    daemon_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);

    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    EXPECT_FALSE(register_client_result.has_value());
}

TEST_F(GenericTraceAPIImplFixture, TestNotStoppedInTheFirstLoop)
{
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
    RequestLibraryWorkerThreadShutdown(library_notifier);
}

TEST_F(GenericTraceAPIImplFixture, ClientIdContainerTestNotExistingClient)
{
    auto client_id_container = std::make_unique<ClientIdContainer>();
    auto result = client_id_container->GetLocalTraceClientId(0);
    EXPECT_FALSE(result.has_value());
}

TEST_F(GenericTraceAPIImplFixture, ShmObjectHandleContainerTestNotExistingObject)
{
    auto shm_object_handle_container = std::make_unique<ShmObjectHandleContainer>();
    shm_object_handle_container->RegisterLocalShmObjectHandle(kValidFileDescriptor);
    shm_object_handle_container->DeregisterLocalShmObject(-1);
}

TEST_F(GenericTraceAPIImplFixture, ShmObjectHandleContainerDrainContainer)
{
    auto shm_object_handle_container = std::make_unique<ShmObjectHandleContainer>();
    shm_object_handle_container->RegisterLocalShmObjectHandle(kValidFileDescriptor);
    shm_object_handle_container->DeregisterLocalShmObject(-2);
}

TEST_F(GenericTraceAPIImplFixture, SupportedBindingTypesTest)
{
    RecordProperty("Verifies", "SCR-39683215");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if GTL API support only 3 binding types: kLoLa, kVector and kVectorZeroCopy");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_EQ(BindingType::kFirst, BindingType::kLoLa);
    EXPECT_EQ(BindingType::kVector, static_cast<BindingType>(1));
    EXPECT_EQ(BindingType::kVectorZeroCopy, static_cast<BindingType>(2));
    EXPECT_EQ(BindingType::kUndefined, static_cast<BindingType>(3));
}

TEST_F(GenericTraceAPIImplFixture, ClientIdElementTestEquality)
{
    AppIdType appIdType{};
    ClientIdElement element1(1, 1, BindingType::kFirst, appIdType);
    ClientIdElement element2(1, 1, BindingType::kFirst, appIdType);
    ClientIdElement element3(1, 1, BindingType::kVector, appIdType);
    EXPECT_TRUE(element1 == element2);
    EXPECT_FALSE(element1 == element3);
}

TEST_F(GenericTraceAPIImplFixture, TestDeallocatorCallbackSucceed)
{
    SharedMemoryLocation chunk_list{};
    TraceJobType job_type{TraceJobType::kLocalJob};
    TraceResult trace_result;
    chunk_list.shm_object_handle_ = 0xAB;
    chunk_list.offset_ = 0xCD;

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
    }
    EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, DeallocateJob(chunk_list, job_type))
        .WillRepeatedly(Return(trace_result));

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    library_notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 100);
    library_notifier.Reset();

    EXPECT_EQ(callback_(chunk_list, job_type), trace_result);
    EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, CloseRingBuffer);
    stop_source_.request_stop();
    RequestLibraryWorkerThreadShutdown(library_notifier);
}

TEST_F(GenericTraceAPIImplFixture, TestDeallocatorCallbackAndFail)
{
    SharedMemoryLocation chunk_list{};
    TraceJobType job_type{TraceJobType::kLocalJob};
    chunk_list.shm_object_handle_ = 0xAB;
    chunk_list.offset_ = 0xCD;

    {
        InSequence sequence;
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
            .WillOnce(Return(ByMove(score::MakeUnexpected(ErrorCode::kRingBufferInvalidMemoryResourceRecoverable))));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, Connect).WillOnce(Return(ResultBlank{}));
        EXPECT_CALL(*mock_daemon_communicator_ptr_, RegisterSharedMemoryObject(tmd_filename_))
            .WillOnce(Return(tmd_shm_obj_handle_));
    }

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());

    EXPECT_FALSE(callback_(chunk_list, job_type).has_value());
}

TEST_F(GenericTraceAPIImplFixture, GlobalErrorReturnOnAllCalls)
{
    RecordProperty("Verifies", "SCR-39765895");
    RecordProperty("ASIL", "QM");
    RecordProperty(
        "Description",
        "Test verifies if GTL APIs return fatal error for all subsequent API calls after first fatal error occurs");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PromiseNotifier daemon_not_connected_notifier;
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
    ASSERT_FALSE(register_client_result.has_value());
    EXPECT_EQ(register_client_result.error(), ErrorCode::kDaemonNotAvailableFatal);

    auto register_shm_by_path_result = library_->RegisterShmObject(client_id, std::string{});
    ASSERT_FALSE(register_shm_by_path_result.has_value());
    EXPECT_EQ(register_shm_by_path_result.error(), ErrorCode::kDaemonNotAvailableFatal);

    auto register_shm_by_file_descriptor = library_->RegisterShmObject(client_id, file_descriptor_);
    ASSERT_FALSE(register_shm_by_file_descriptor.has_value());
    EXPECT_EQ(register_shm_by_file_descriptor.error(), ErrorCode::kDaemonNotAvailableFatal);

    auto unregister_shm_result = library_->UnregisterShmObject(client_id, shm_obj_handle_);
    ASSERT_FALSE(unregister_shm_result.has_value());
    EXPECT_EQ(unregister_shm_result.error(), ErrorCode::kDaemonNotAvailableFatal);

    auto register_callback_result = library_->RegisterTraceDoneCB(client_id, std::move(trace_done_callback));
    ASSERT_FALSE(register_callback_result.has_value());
    EXPECT_EQ(register_callback_result.error(), ErrorCode::kDaemonNotAvailableFatal);

    auto local_trace_result = library_->Trace(client_id, meta_info_, local_data_chunk_list_);
    ASSERT_FALSE(local_trace_result.has_value());
    EXPECT_EQ(local_trace_result.error(), ErrorCode::kDaemonNotAvailableFatal);

    auto shm_trace_result = library_->Trace(client_id, meta_info_, shm_data_chunk_list_, context_id_);
    ASSERT_FALSE(shm_trace_result.has_value());
    EXPECT_EQ(shm_trace_result.error(), ErrorCode::kDaemonNotAvailableFatal);
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
