///
/// @file object_factory_test.cpp
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API object factory test source file
///

#include "score/analysis/tracing/library/generic_trace_api/object_factory.h"
#include "score/analysis/tracing/common/flexible_circular_allocator/lockless_flexible_circular_allocator.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_allocator/trace_job_allocator.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_container/trace_job_container.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_processor/i_trace_job_processor.h"
#include "score/analysis/tracing/library/test/unit_test/local_memory_resource.h"
#include "score/analysis/tracing/library/test/unit_test/mocks/mock_daemon_communicator.h"
#include "score/analysis/tracing/library/test/unit_test/mocks/mock_daemon_communicator_factory.h"
#include "score/analysis/tracing/library/test/unit_test/mocks/mock_trace_job_allocator_constructor.h"
#include "score/analysis/tracing/shm_ring_buffer/mock_shm_ring_buffer.h"

#include "score/memory/shared/shared_memory_factory.h"
#include "score/memory/shared/shared_memory_factory_mock.h"
#include "score/memory/shared/shared_memory_resource_mock.h"
#include "platform/aas/mw/time/HWLoggerTime/receiver/eptm_receiver_mock.h"
#include "platform/aas/mw/time/HWLoggerTime/receiver/factory_mock.h"

#include <score/optional.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::ByMove;
using ::testing::Mock;
using ::testing::Return;
using ::testing::StrictMock;

namespace score
{
namespace analysis
{
namespace tracing
{

using TraceJobAllocatorResult = score::Result<std::unique_ptr<ITraceJobAllocator>>;
using SharedMemoryResourceMock = ::testing::StrictMock<memory::shared::SharedMemoryResourceMock>;

class ObjectFactoryTestFixture : public testing::Test
{
  protected:
    void SetUp() override
    {
        logger_time_factory = std::make_unique<StrictMock<score::mw::time::hwloggertime::EptmReceiverFactoryMock>>();
        no_time_factory_1_ = std::make_unique<StrictMock<score::mw::time::hwloggertime::EptmReceiverFactoryMock>>();
        no_time_factory_2_ = std::make_unique<StrictMock<score::mw::time::hwloggertime::EptmReceiverFactoryMock>>();
        daemon_communicator_factory_ = std::make_unique<StrictMock<MockDaemonCommunicatorFactory>>();
        time = std::make_shared<score::mw::time::hwloggertime::EptmReceiverMock>();

        communicator_factory_pointer_ = daemon_communicator_factory_.get();
        logger_time_factory_ptr_ = logger_time_factory.get();
        no_time_factory_ptr_ = no_time_factory_1_.get();
        shm_ring_buffer_mock_for_logger_ = std::make_unique<MockShmRingBuffer>();
        shm_ring_buffer_mock_for_no_logger_ = std::make_unique<MockShmRingBuffer>();
        shm_ring_buffer_mock_for_no_logger_raw_ = shm_ring_buffer_mock_for_no_logger_.get();

        factory_with_logger_time_injected =
            std::make_unique<ObjectFactory>(std::move(logger_time_factory),
                                            std::move(daemon_communicator_factory_),
                                            std::move(shm_ring_buffer_mock_for_logger_));
        factory_with_no_logger_time_injected =
            std::make_unique<ObjectFactory>(std::move(no_time_factory_1_),
                                            std::move(daemon_communicator_factory_),
                                            std::move(shm_ring_buffer_mock_for_no_logger_));
        factory_with_no_logger_time_no_shmbuffer_injected =
            std::make_unique<ObjectFactory>(std::move(no_time_factory_2_), std::move(daemon_communicator_factory_));
        score::memory::shared::SharedMemoryFactory::InjectMock(&shared_memory_factory);
    }

    void TearDown() override
    {
        factory_with_logger_time_injected.reset();
        factory_with_no_logger_time_injected.reset();
        factory_with_no_logger_time_no_shmbuffer_injected.reset();
        ASSERT_TRUE(Mock::VerifyAndClearExpectations(communicator_factory_pointer_));
        ASSERT_TRUE(Mock::VerifyAndClearExpectations(logger_time_factory_ptr_));
        ASSERT_TRUE(Mock::VerifyAndClearExpectations(no_time_factory_ptr_));
        score::memory::shared::SharedMemoryFactory::InjectMock(nullptr);
    }

    std::pair<void*, void*> ranges_{reinterpret_cast<void*>(100U),
                                    reinterpret_cast<void*>(std::numeric_limits<std::uint64_t>::max())};

    std::shared_ptr<LocalMemoryResource> memory_ = std::make_shared<LocalMemoryResource>(ranges_);

    ShmObjectHandle memory_handle_{1};
    std::shared_ptr<TraceJobContainer> container_ = std::make_shared<TraceJobContainer>();
    ClientIdContainer client_id_container_;
    MockTraceJobAllocatorConstructor* GlobalTraceJobAllocator{};
    score::memory::shared::SharedMemoryFactoryMock shared_memory_factory;
    std::shared_ptr<SharedMemoryResourceMock> shm_resource_mock_{std::make_shared<SharedMemoryResourceMock>()};
    std::shared_ptr<score::mw::time::hwloggertime::EptmReceiverMock> time{};
    std::unique_ptr<StrictMock<score::mw::time::hwloggertime::EptmReceiverFactoryMock>> logger_time_factory{};
    std::unique_ptr<StrictMock<score::mw::time::hwloggertime::EptmReceiverFactoryMock>> no_time_factory_1_{};
    std::unique_ptr<StrictMock<score::mw::time::hwloggertime::EptmReceiverFactoryMock>> no_time_factory_2_{};
    std::unique_ptr<IObjectFactory> factory_with_logger_time_injected{};
    std::unique_ptr<IObjectFactory> factory_with_no_logger_time_injected{};
    std::unique_ptr<IObjectFactory> factory_with_no_logger_time_no_shmbuffer_injected{};
    std::unique_ptr<StrictMock<MockDaemonCommunicatorFactory>> daemon_communicator_factory_{};
    std::unique_ptr<MockShmRingBuffer> shm_ring_buffer_mock_for_logger_;
    std::unique_ptr<MockShmRingBuffer> shm_ring_buffer_mock_for_no_logger_;
    MockShmRingBuffer* shm_ring_buffer_mock_for_no_logger_raw_;

    StrictMock<score::analysis::tracing::MockDaemonCommunicatorFactory>* communicator_factory_pointer_;
    StrictMock<score::mw::time::hwloggertime::EptmReceiverFactoryMock>* logger_time_factory_ptr_;
    StrictMock<score::mw::time::hwloggertime::EptmReceiverFactoryMock>* no_time_factory_ptr_;
    score::cpp::stop_token stop_token;
};

TEST_F(ObjectFactoryTestFixture, TestDaemonCommunicatorCreated)
{
    auto mock_communicator = std::make_unique<MockDaemonCommunicator>();
    EXPECT_CALL(*communicator_factory_pointer_, CreateDaemonCommunicator)
        .WillOnce(Return(ByMove(std::move(mock_communicator))));
    auto daemon_comunicator = factory_with_logger_time_injected->CreateDaemonCommunicator();
    EXPECT_TRUE(daemon_comunicator);
}

TEST_F(ObjectFactoryTestFixture, TestJobAllocatorOpenSharedMemSuccessWithoutHWTime)
{
    EXPECT_CALL(*no_time_factory_ptr_, ObtainEptmReceiverTimebase()).WillOnce(Return(nullptr));

    std::unique_ptr<uint8_t> non_empty_ptr = std::make_unique<uint8_t>();
    EXPECT_CALL(*(shm_resource_mock_.get()), do_allocate(_, _)).WillRepeatedly(Return(non_empty_ptr.get()));

    TraceJobAllocatorResult result =
        factory_with_no_logger_time_injected->CreateTraceJobAllocator(container_, shm_resource_mock_);
    EXPECT_TRUE(result.has_value());
    EXPECT_FALSE(result.value()->IsLoggerTimeAvailable());
}

TEST_F(ObjectFactoryTestFixture, TestJobAllocatorWithHWTimeFailedToInit)
{
    std::shared_ptr<score::memory::shared::ISharedMemoryResource> x =
        std::make_shared<score::memory::shared::SharedMemoryResourceMock>();
    EXPECT_CALL(shared_memory_factory, Open(_, _, _)).WillRepeatedly(::testing::Return(x));

    EXPECT_CALL(*logger_time_factory_ptr_, ObtainEptmReceiverTimebase()).WillOnce(Return(time));

    EXPECT_CALL(*time.get(), Init()).WillOnce(Return(false));

    std::unique_ptr<uint8_t> non_empty_ptr = std::make_unique<uint8_t>();
    EXPECT_CALL(*(shm_resource_mock_.get()), do_allocate(_, _)).WillRepeatedly(Return(non_empty_ptr.get()));

    TraceJobAllocatorResult result =
        factory_with_logger_time_injected->CreateTraceJobAllocator(container_, shm_resource_mock_);
    EXPECT_TRUE(result.has_value());
    EXPECT_FALSE(result.value()->IsLoggerTimeAvailable());
}

TEST_F(ObjectFactoryTestFixture, TestJobAllocatorWithHWTimeInitSuccess)
{
    EXPECT_CALL(*logger_time_factory_ptr_, ObtainEptmReceiverTimebase()).WillOnce(Return(time));

    EXPECT_CALL(*time.get(), Init()).WillOnce(Return(true));

    std::unique_ptr<uint8_t> non_empty_ptr = std::make_unique<uint8_t>();
    EXPECT_CALL(*(shm_resource_mock_.get()), do_allocate(_, _)).WillRepeatedly(Return(non_empty_ptr.get()));

    TraceJobAllocatorResult result =
        factory_with_logger_time_injected->CreateTraceJobAllocator(container_, shm_resource_mock_);
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result.value()->IsLoggerTimeAvailable());
}

TEST_F(ObjectFactoryTestFixture, JobAllocatorFailedCreateShmRing)
{
    std::unique_ptr<uint8_t> non_empty_ptr = std::make_unique<uint8_t>();
    std::shared_ptr<score::memory::shared::ISharedMemoryResource> x =
        std::make_shared<score::memory::shared::SharedMemoryResourceMock>();
    EXPECT_CALL(*(shm_resource_mock_.get()), do_allocate(_, _)).WillRepeatedly(Return(non_empty_ptr.get()));
    EXPECT_CALL(shared_memory_factory, Create(_, _, _, _, _)).WillRepeatedly(::testing::Return(x));
    EXPECT_CALL(*shm_ring_buffer_mock_for_no_logger_raw_, CreateOrOpen(_))
        .WillRepeatedly(Return(score::MakeUnexpected(ErrorCode::kRingBufferInitializedRecoverable)));
    TraceJobAllocatorResult result =
        factory_with_no_logger_time_no_shmbuffer_injected->CreateTraceJobAllocator(container_, shm_resource_mock_);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ObjectFactoryTestFixture, TestTraceJobProcessorCreated)
{
    auto proc = factory_with_logger_time_injected->CreateTraceJobProcessor(
        client_id_container_, container_, nullptr, stop_token);
    EXPECT_TRUE(proc != nullptr);
}
}  // namespace tracing
}  // namespace analysis
}  // namespace score
