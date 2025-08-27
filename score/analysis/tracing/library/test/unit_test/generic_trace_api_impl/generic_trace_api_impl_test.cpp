#include "score/analysis/tracing/library/test/unit_test/generic_trace_api_impl/generic_trace_api_impl_test.h"

#include "score/memory/shared/shared_memory_factory_mock.h"

namespace score
{
namespace analysis
{
namespace tracing
{

void GenericTraceAPIImplFixtureBase::SetUp()
{
    mock_daemon_communicator_ptr_ = std::make_unique<StrictMock<MockDaemonCommunicator>>();
    mock_object_factory_ = std::make_unique<StrictMock<MockObjectFactory>>();
}

TraceResult GenericTraceAPIImplFixture::deallocator(SharedMemoryLocation, TraceJobType)
{
    return {};
}

void GenericTraceAPIImplFixture::SetUp()
{
    GenericTraceAPIImplFixtureBase::SetUp();
    mock_unistd_ = std::make_unique<StrictMock<os::UnistdMock>>();
    mock_memory_validator_ = std::make_unique<StrictMock<MockMemoryValidator>>();
    mock_shared_memory_resource_ = std::make_shared<StrictMock<memory::shared::SharedMemoryResourceMock>>();
    mock_shared_memory_factory_ = std::make_shared<StrictMock<memory::shared::SharedMemoryFactoryMock>>();
    memory::shared::SharedMemoryFactory::InjectMock(mock_shared_memory_factory_.get());
    mock_trace_job_allocator_ = std::make_unique<StrictMock<MockTraceJobAllocator>>();
    mock_trace_job_allocator_ptr_raw_ = mock_trace_job_allocator_.get();
    mock_trace_job_processor_ = std::make_unique<StrictMock<MockTraceJobProcessor>>();
    mock_trace_job_processor_ptr_raw_ = mock_trace_job_processor_.get();
    mock_daemon_communicator_ptr_raw_ = mock_daemon_communicator_ptr_.get();
}

void GenericTraceAPIImplFixture::TearDown()
{
    ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(mock_shared_memory_resource_.get()));
    ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(mock_daemon_communicator_ptr_raw_));
    ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(mock_trace_job_allocator_ptr_raw_));
    ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(mock_trace_job_processor_ptr_raw_));
}

void GenericTraceAPIImplFixture::ExpectCorrectInitialization(
    PromiseNotifier& daemon_notifier,
    PromiseNotifier& library_notifier,
    std::optional<std::reference_wrapper<std::atomic<bool>>> delay_ltpm_daemon_param)
{
    std::atomic<bool> delay_ltpm_daemon = false;
    if (delay_ltpm_daemon_param)
    {
        delay_ltpm_daemon = delay_ltpm_daemon_param->get().load();
    }

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
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, Connect)
            .WillRepeatedly(Invoke([&delay_ltpm_daemon, &daemon_notifier]() -> ResultBlank {
                if (!delay_ltpm_daemon.load())
                {
                    daemon_notifier.Notify();
                    return ResultBlank{};
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
    }
}

void GenericTraceAPIImplFixture::ExpectCorrectCleaningUp(PromiseNotifier& terminate_notifier)
{
    InSequence sequence;
    {
        EXPECT_CALL(*mock_daemon_communicator_ptr_raw_, UnregisterSharedMemoryObject(tmd_shm_obj_handle_))
            .WillOnce(Return(ResultBlank{}));
        EXPECT_CALL(*mock_shared_memory_factory_, Remove(tmd_filename_));
        EXPECT_CALL(*mock_trace_job_allocator_ptr_raw_, CloseRingBuffer).WillOnce(Invoke([&terminate_notifier]() {
            terminate_notifier.Notify();
        }));
    }
}

void GenericTraceAPIImplFixture::RequestLibraryWorkerThreadShutdown(PromiseNotifier& notifier)
{
    std::cout << "Requesting stop..." << std::endl;
    std::this_thread::sleep_for(10 * kLibraryCheckPeriodicity);
    stop_source_.request_stop();
    std::cout << "Waiting for the library worker thread to shutdown..." << std::endl;
    notifier.WaitForNotificationWithTimeout(kLibraryCheckPeriodicity * 10);
    std::cout << "Library worker thread finished !" << std::endl;
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
