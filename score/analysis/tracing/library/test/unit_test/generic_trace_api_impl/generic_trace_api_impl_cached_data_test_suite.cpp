#include "score/analysis/tracing/library/test/unit_test/generic_trace_api_impl/generic_trace_api_impl_test.h"

#include <future>

namespace score
{
namespace analysis
{
namespace tracing
{

TEST_F(GenericTraceAPIImplFixture, LtpmDaemonConnectionReadyUncachingClientsFails)
{
    std::atomic<bool> delay_ltpm_daemon{true};
    bool ready_to_trace = false;
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
        EXPECT_CALL(*mock_shared_memory_factory_, Create).WillOnce(Return(mock_shared_memory_resource_));
        EXPECT_CALL(*mock_shared_memory_resource_, getBaseAddress)
            .WillOnce(Return(&valid_pointer_));  // just return a valid pointer
        EXPECT_CALL(*mock_shared_memory_resource_, IsShmInTypedMemory).WillOnce(Return(true));
        EXPECT_CALL(*mock_object_factory_, CreateTraceJobAllocator)
            .WillOnce(Return(ByMove(std::move(mock_trace_job_allocator_))));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, Connect)
            .WillRepeatedly(Invoke([&delay_ltpm_daemon, &daemon_notifier]() -> ResultBlank {
                if (!delay_ltpm_daemon.load())
                {
                    daemon_notifier.Notify();
                    return ResultBlank{};
                }
                return MakeUnexpected(ErrorCode::kServerConnectionNameOpenFailedFatal);
            }));

        // uncaching
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient)
            .WillOnce(Return(MakeUnexpected<TraceClientId>(ErrorCode::kMessageSendFailedRecoverable)));
        EXPECT_CALL(*mock_daemon_communicator_ptr_, RegisterSharedMemoryObject(tmd_filename_))
            .WillOnce(Return(tmd_shm_obj_handle_));
        EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, SetTraceMetaDataShmObjectHandle(tmd_shm_obj_handle_))
            .WillOnce(Invoke([this, &ready_to_trace]() {
                ready_to_trace = true;
                condition_variable_.notify_one();
            }));
        EXPECT_CALL(*mock_trace_job_processor_ptr_raw_, ProcessJobs).WillRepeatedly(Return(ResultBlank{}));
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
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto delay_daemon_startup = std::async(std::launch::async, [&delay_ltpm_daemon]() {
        std::this_thread::sleep_for(kDaemonReadyCheckPeriodicity * 10);
        delay_ltpm_daemon.store(false);
    });
    daemon_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);
    delay_daemon_startup.get();
    {
        std::unique_lock<std::mutex> lock{mutex_};
        condition_variable_.wait(lock, [&ready_to_trace]() {
            return ready_to_trace;
        });
    }
    auto trace_result_1 =
        library_->Trace(register_client_result.value(), meta_info_, shm_data_chunk_list_, context_id_);
    auto trace_result_2 = library_->Trace(register_client_result.value(), meta_info_, local_data_chunk_list_);
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    ASSERT_FALSE(trace_result_1.has_value());
    EXPECT_EQ(trace_result_1.error(), ErrorCode::kMessageSendFailedRecoverable);
    ASSERT_FALSE(trace_result_2.has_value());
    EXPECT_EQ(trace_result_2.error(), ErrorCode::kMessageSendFailedRecoverable);
}

TEST_F(GenericTraceAPIImplFixture, LtpmDaemonConnectionReadyUncachingClientsInterrupted)
{
    const TraceClientId client_id{1};
    std::atomic<bool> delay_ltpm_daemon = true;
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
        EXPECT_CALL(*mock_shared_memory_factory_, Create).WillOnce(Return(mock_shared_memory_resource_));
        EXPECT_CALL(*mock_shared_memory_resource_, getBaseAddress)
            .WillOnce(Return(&valid_pointer_));  // just return a valid pointer
        EXPECT_CALL(*mock_shared_memory_resource_, IsShmInTypedMemory).WillOnce(Return(true));
        EXPECT_CALL(*mock_object_factory_, CreateTraceJobAllocator)
            .WillOnce(Return(ByMove(std::move(mock_trace_job_allocator_))));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, Connect)
            .WillRepeatedly(Invoke([&delay_ltpm_daemon, &daemon_notifier]() -> ResultBlank {
                if (!delay_ltpm_daemon.load())
                {
                    daemon_notifier.Notify();
                    return ResultBlank{};
                }
                return MakeUnexpected(ErrorCode::kServerConnectionNameOpenFailedFatal);
            }));

        // uncaching
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient)
            .Times(1)
            .WillOnce(Invoke([this, &library_notifier]() {
                stop_source_.request_stop();
                library_notifier.Notify();
                return client_id;
            }));
    }

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    register_client_result = library_->RegisterClient(BindingType::kVector, app_instance_id_);
    auto delay_daemon_startup = std::async(std::launch::async, [&delay_ltpm_daemon]() {
        std::this_thread::sleep_for(kDaemonReadyCheckPeriodicity * 10);
        delay_ltpm_daemon.store(false);
    });
    std::cout << "daemon_wait\n";
    daemon_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);
    delay_daemon_startup.get();
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
}

TEST_F(GenericTraceAPIImplFixture, LtpmDaemonConnectionReadyUncachingShmObjectsTraceSuccess)
{
    RecordProperty("Verifies", "SCR-39687815");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if Trace() API returns success after cached register client request was forwarded "
                   "when absent backed became available");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::atomic<bool> delay_ltpm_daemon = true;
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    PromiseNotifier terminate_notifier;

    ExpectCorrectInitialization(daemon_notifier, library_notifier, delay_ltpm_daemon);
    {
        InSequence sequence;
        EXPECT_CALL(*mock_memory_validator_, IsSharedMemoryTyped(Matcher<std::int32_t>(file_descriptor_)))
            .WillOnce(Return(true));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_,
                    RegisterSharedMemoryObject(Matcher<std::int32_t>(file_descriptor_)))
            .Times(1)
            .WillOnce(Return(shm_obj_handle_));
        EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, AllocateShmJob(client_id_, _, _, _, _, context_id_))
            .WillOnce(Return(ResultBlank{}));
    }
    ExpectCorrectCleaningUp(terminate_notifier);

    // uncaching
    EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillRepeatedly(Return(client_id_));

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(mock_memory_validator_),
                                                          stop_source_.get_token());
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    EXPECT_TRUE(register_client_result.has_value());
    auto register_shm_by_file_descriptor =
        library_->RegisterShmObject(register_client_result.value(), file_descriptor_);
    EXPECT_TRUE(register_shm_by_file_descriptor.has_value());

    auto delay_daemon_startup = std::async(std::launch::async, [&delay_ltpm_daemon]() {
        std::this_thread::sleep_for(kDaemonReadyCheckPeriodicity * 10);
        delay_ltpm_daemon.store(false);
    });
    daemon_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);
    delay_daemon_startup.get();
    auto shm_trace_result =
        library_->Trace(register_client_result.value(), meta_info_, shm_data_chunk_list_, context_id_);
    RequestLibraryWorkerThreadShutdown(terminate_notifier);
    EXPECT_TRUE(shm_trace_result.has_value());
}

TEST_F(GenericTraceAPIImplFixture, LtpmDaemonConnectionReadyUncachingShmObjectsFails)
{
    std::atomic<bool> delay_ltpm_daemon = true;
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    const TraceClientId client_id{1};
    auto nice_mock_memory_validator_ = std::make_unique<NiceMock<MockMemoryValidator>>();
    ON_CALL(*nice_mock_memory_validator_, IsSharedMemoryTyped(Matcher<std::int32_t>(file_descriptor_)))
        .WillByDefault(Return(true));

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
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, Connect)
            .WillRepeatedly(Invoke([&daemon_notifier, &delay_ltpm_daemon]() -> ResultBlank {
                if (!delay_ltpm_daemon.load())
                {
                    daemon_notifier.Notify();
                    return ResultBlank{};
                }
                return MakeUnexpected(ErrorCode::kServerConnectionNameOpenFailedFatal);
            }));

        // uncaching
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_,
                    RegisterSharedMemoryObject(Matcher<std::int32_t>(file_descriptor_)))
            .WillOnce(Invoke([&]() -> score::Result<ShmObjectHandle> {
                library_notifier.Notify();
                return MakeUnexpected<ShmObjectHandle>(ErrorCode::kMessageSendFailedRecoverable);
            }));
    }

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(nice_mock_memory_validator_),
                                                          stop_source_.get_token());
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto register_shm_by_file_descriptor = library_->RegisterShmObject(client_id, file_descriptor_);
    auto delay_daemon_startup = std::async(std::launch::async, [&delay_ltpm_daemon]() {
        std::this_thread::sleep_for(kDaemonReadyCheckPeriodicity * 10);
        delay_ltpm_daemon.store(false);
    });
    daemon_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);
    delay_daemon_startup.get();
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_TRUE(register_shm_by_file_descriptor.has_value());
}

TEST_F(GenericTraceAPIImplFixture, LtpmDaemonConnectionReadyUncachingShmObjectsInterrupted)
{
    std::atomic<bool> delay_ltpm_daemon = true;
    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    const TraceClientId client_id{1};

    auto nice_mock_memory_validator_ = std::make_unique<NiceMock<MockMemoryValidator>>();
    ON_CALL(*nice_mock_memory_validator_, IsSharedMemoryTyped(Matcher<std::int32_t>(file_descriptor_)))
        .WillByDefault(Return(true));
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
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, Connect)
            .WillRepeatedly(Invoke([&daemon_notifier, &delay_ltpm_daemon]() -> ResultBlank {
                if (!delay_ltpm_daemon.load())
                {
                    daemon_notifier.Notify();
                    return ResultBlank{};
                }
                return MakeUnexpected(ErrorCode::kServerConnectionNameOpenFailedFatal);
            }));

        // uncaching
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_,
                    RegisterSharedMemoryObject(Matcher<std::int32_t>(file_descriptor_)))
            .WillOnce(Invoke([this, &library_notifier]() {
                stop_source_.request_stop();
                library_notifier.Notify();
                return ShmObjectHandle{0};
            }));
    }

    auto library_ = std::make_unique<GenericTraceAPIImpl>(std::move(mock_object_factory_),
                                                          std::move(mock_unistd_),
                                                          std::move(nice_mock_memory_validator_),
                                                          stop_source_.get_token());
    auto register_client_result = library_->RegisterClient(BindingType::kVectorZeroCopy, app_instance_id_);
    auto register_shm_by_file_descriptor = library_->RegisterShmObject(client_id, file_descriptor_);
    auto delay_daemon_startup = std::async(std::launch::async, [&delay_ltpm_daemon]() {
        std::this_thread::sleep_for(kDaemonReadyCheckPeriodicity * 20);
        delay_ltpm_daemon.store(false);
    });
    daemon_notifier.WaitForNotificationWithTimeout(kDaemonReadyCheckPeriodicity * 100);
    delay_daemon_startup.get();
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_TRUE(register_shm_by_file_descriptor.has_value());
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
