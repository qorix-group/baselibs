#include "score/analysis/tracing/library/test/unit_test/generic_trace_api_impl/generic_trace_api_impl_test.h"

namespace score
{
namespace analysis
{
namespace tracing
{

TEST_F(GenericTraceAPIImplFixture, RegisterTraceDoneCallbackClientNotFound)
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
    auto register_callback_result = library_->RegisterTraceDoneCB(
        static_cast<TraceClientId>(register_client_result.value() + 1), std::move(trace_done_callback));
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_EQ(register_callback_result.error(), ErrorCode::kClientNotFoundRecoverable);
}

TEST_F(GenericTraceAPIImplFixture, RegisterTraceDoneCallbackEmptyCallback)
{
    RecordProperty("Verifies", "SCR-39688078");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if callback registration API returns an error if nullptr callback is given as input");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

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
    TraceDoneCallBackType no_callback{};
    auto register_callback_result = library_->RegisterTraceDoneCB(
        static_cast<TraceClientId>(register_client_result.value()), std::move(no_callback));
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_EQ(register_callback_result.error(), ErrorCode::kInvalidArgumentFatal);
}

TEST_F(GenericTraceAPIImplFixture, RegisterTraceDoneCallbackSuccess)
{
    RecordProperty("Verifies", "SCR-39687939, SCR-39688041");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if GTL provides an API to set the release callback per client"
                   "Test verifies also if subsequent trial of callback registration for the same client returns error");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PromiseNotifier daemon_notifier;
    PromiseNotifier library_notifier;
    ExpectCorrectInitialization(daemon_notifier, library_notifier);
    const TraceClientId client_id{1};
    {
        InSequence sequence;
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, RegisterClient).WillOnce(Return(client_id));
        EXPECT_CALL(*mock_trace_job_processor_ptr_raw_, SaveCallback(client_id, _)).WillOnce(Return(ResultBlank{}));
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
    auto register_callback_result =
        library_->RegisterTraceDoneCB(register_client_result.value(), std::move(trace_done_callback));
    auto register_2nd_callback_result =
        library_->RegisterTraceDoneCB(register_client_result.value(), std::move(trace_done_callback));
    RequestLibraryWorkerThreadShutdown(library_notifier);

    EXPECT_TRUE(register_callback_result.has_value());
    EXPECT_FALSE(register_2nd_callback_result.has_value());
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
