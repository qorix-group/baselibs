///
/// @file shm_ring_buffer_test.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API generic_trace_api test source
///

#include "score/analysis/tracing/shm_ring_buffer/shm_ring_buffer.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"
#include "score/analysis/tracing/library/test/unit_test/local_memory_resource.h"
#include "score/memory/shared/atomic_indirector.h"
#include "score/memory/shared/atomic_mock.h"
#include "score/memory/shared/shared_memory_factory.h"
#include "score/memory/shared/shared_memory_factory_mock.h"
#include "score/memory/shared/shared_memory_resource_heap_allocator_mock.h"
#include "score/memory/shared/shared_memory_resource_mock.h"
#include "score/os/mocklib/unistdmock.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>
#include <random>
#include <thread>
#include <vector>
using namespace score::analysis::tracing;
using namespace score::analysis::tracing::detail;
using namespace score::memory::shared;
using ::testing::_;
using ::testing::AtLeast;
using ::testing::ByMove;
using ::testing::InSequence;
using ::testing::Invoke;
using ::testing::Return;
using testing::StrictMock;

template <template <class> class AtomicIndirectorType = AtomicIndirectorReal>
class ShmRingBufferFixtureBase : public testing::TestWithParam<bool>
{
  protected:
    static constexpr std::uint32_t kThreadCount = 4;
    static constexpr std::size_t kTotalElements = 100;
    static constexpr std::size_t kBufferElements = 20;
    static constexpr std::size_t kElementsPerProducer =
        static_cast<std::size_t>((kTotalElements + kThreadCount - 1) / kThreadCount);
    static constexpr std::uint32_t get_write_element_max_retries = 10U;
    static constexpr std::size_t number_of_elements = 10;
    static constexpr std::size_t too_big_number_of_elements = 0x8000;
    static constexpr bool empty = true;
    static constexpr bool ready = false;
    static constexpr std::uint16_t kMax15Bits = 0x7FFFU;

    ShmRingBufferState ring_buffer_state_{};
    std::shared_ptr<SharedMemoryFactoryMock> mock_shared_memory_factory_;
    std::shared_ptr<SharedMemoryResourceMock> mock_shared_memory_resource_ =
        std::make_shared<SharedMemoryResourceMock>();

    static constexpr char kRingBufferSharedMemoryPathTest[] = "/shmem_test";
    static constexpr std::size_t kRingBufferSharedMemorySize = 4096U;

    std::shared_ptr<ISharedMemoryResource> shared_memory_;
    std::unique_ptr<ShmRingBufferImpl<AtomicIndirectorType>> ring_buffer_;

    std::shared_ptr<AtomicMock<ShmRingBufferState>> atomic_state_mock_;
    std::shared_ptr<AtomicMock<TraceJobStatus>> atomic_status_mock_;
    std::shared_ptr<AtomicMock<std::uint64_t>> atomic_statistics_mock_;

    void GetElementsWithSuccess(ShmRingBufferImpl<AtomicIndirectorType>& buffer,
                                std::uint16_t elements_count,
                                const bool type,
                                std::optional<TraceJobStatus> override_trace_job_status = std::nullopt)
    {
        if constexpr (std::is_same_v<ShmRingBufferImpl<AtomicIndirectorType>, ShmRingBufferImpl<AtomicIndirectorMock>>)
        {
            EXPECT_CALL(*atomic_state_mock_, compare_exchange_weak(_, _, _))
                .Times(elements_count)
                .WillRepeatedly(Return(true));

            if (type == empty && GetParam())
            {
                EXPECT_CALL(*atomic_statistics_mock_, fetch_add(_, _)).Times(AtLeast(1)).WillRepeatedly(Return(0));
                EXPECT_CALL(*atomic_status_mock_, store(TraceJobStatus::kAllocated, _)).Times(elements_count);
            }
            else if (type == ready && GetParam())
            {
                EXPECT_CALL(*atomic_status_mock_, store(TraceJobStatus::kReady, _)).Times(elements_count);
            }
        }

        for (std::uint16_t i = 0; i < elements_count; i++)
        {
            if constexpr (std::is_same_v<ShmRingBufferImpl<AtomicIndirectorType>,
                                         ShmRingBufferImpl<AtomicIndirectorMock>>)
            {
                EXPECT_CALL(*atomic_state_mock_, load(_)).Times(1).WillOnce(Return(ring_buffer_state_));
            }
            score::Result<std::reference_wrapper<ShmRingBufferElement>> element{
                score::MakeUnexpected<>(ErrorCode::kGenericErrorRecoverable)};
            if (type == empty)
            {
                element = buffer.GetEmptyElement();
                // simulate the update of the ShmRingBufferState
                const std::uint16_t previous_end_index = ring_buffer_state_.end;
                const auto new_end_index =
                    static_cast<std::uint16_t>((previous_end_index + 1U) % ring_buffer_->GetSize().value());
                const ShmRingBufferState new_state{0U,
                                                   static_cast<std::uint16_t>(ring_buffer_state_.start & kMax15Bits),
                                                   static_cast<std::uint16_t>(new_end_index & kMax15Bits)};
                ring_buffer_state_ = new_state;
            }
            else
            {
                element = buffer.GetReadyElement();
                // simulate the update of the ShmRingBufferState
                const auto new_start_index =
                    static_cast<std::uint16_t>((ring_buffer_state_.start + 1U) % ring_buffer_->GetSize().value());
                ring_buffer_state_ = {
                    static_cast<std::uint8_t>((new_start_index & kMax15Bits) == (ring_buffer_state_.end & kMax15Bits)),
                    static_cast<std::uint16_t>(new_start_index & kMax15Bits),
                    static_cast<std::uint16_t>(ring_buffer_state_.end & kMax15Bits)};
            }

            ASSERT_TRUE(element.has_value());
            element.value().get().global_context_id_ = {0, i};
            if (type == empty)
            {
                element.value().get().status_ = TraceJobStatus::kReady;
            }
            else
            {
                element.value().get().status_ = TraceJobStatus::kEmpty;
            }

            if (override_trace_job_status.has_value())
            {
                element.value().get().status_ = override_trace_job_status.value();
            }
        }
    }
    void CreateRingBufferWithSize(const std::size_t size)
    {
        ring_buffer_ = std::make_unique<ShmRingBufferImpl<AtomicIndirectorType>>(
            kRingBufferSharedMemoryPathTest, size, GetParam());
    }
    void SetUp() override
    {

        if constexpr (std::is_same_v<ShmRingBufferImpl<AtomicIndirectorType>, ShmRingBufferImpl<AtomicIndirectorMock>>)
        {
            ring_buffer_state_.start = 0U;
            ring_buffer_state_.end = 0U;
            ring_buffer_state_.empty = 1U;

            atomic_state_mock_ = std::make_shared<AtomicMock<ShmRingBufferState>>();
            atomic_status_mock_ = std::make_shared<AtomicMock<TraceJobStatus>>();
            atomic_statistics_mock_ = std::make_shared<AtomicMock<std::uint64_t>>();
            AtomicIndirectorMock<ShmRingBufferState>::SetMockObject(atomic_state_mock_.get());
            AtomicIndirectorMock<TraceJobStatus>::SetMockObject(atomic_status_mock_.get());
            AtomicIndirectorMock<std::uint64_t>::SetMockObject(atomic_statistics_mock_.get());
        }
    }

    void TearDown() override
    {
        SharedMemoryFactory::Remove(kRingBufferSharedMemoryPathTest);
    }
};

using ShmRingBufferFixture = ShmRingBufferFixtureBase<>;
using AtomicIndirectorMockFixture = ShmRingBufferFixtureBase<AtomicIndirectorMock>;

TEST_P(ShmRingBufferFixture, MultiProducerSingleConsumerRaceTest)
{
    CreateRingBufferWithSize(number_of_elements);
    ASSERT_TRUE(ring_buffer_->CreateOrOpen().has_value());
    static constexpr auto kProcessCycleSleepStep{10};
    std::atomic<std::size_t> produced_count{0};
    std::atomic<std::size_t> produced_trial_count{0};
    std::atomic<std::size_t> consumed_count{0};
    std::vector<std::thread> producers;

    for (std::uint32_t thread_index = 0; thread_index < kThreadCount; ++thread_index)
    {
        producers.emplace_back([this, &produced_count, &produced_trial_count, thread_index]() noexcept {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<std::uint32_t> dist(1, 5);
            const std::uint32_t empty_element_fetch_count = dist(gen);
            std::uint32_t step = dist(gen);
            std::uint32_t sleep_duration{0};
            std::uint32_t produced{0};

            while (produced < kElementsPerProducer)
            {
                step = dist(gen);
                sleep_duration = step * kProcessCycleSleepStep;

                for (std::uint32_t element_count = 0; element_count < empty_element_fetch_count; ++element_count)
                {
                    produced_trial_count.fetch_add(1, std::memory_order_release);
                    auto result = ring_buffer_->GetEmptyElement();
                    if (result.has_value())
                    {
                        auto& element = result.value().get();
                        element.global_context_id_ = {0, static_cast<std::uint32_t>(thread_index * 1000 + produced)};
                        element.status_ = TraceJobStatus::kReady;
                        produced_count.fetch_add(1, std::memory_order_release);
                        ++produced;
                    }
                }
                if (produced >= kElementsPerProducer)
                {
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(sleep_duration));
            }
        });
    }

    std::thread consumer([this, &consumed_count]() noexcept {
        const auto kProcessPeriodicity = std::chrono::milliseconds{kProcessCycleSleepStep};
        while (consumed_count.load(std::memory_order_acquire) < kTotalElements)
        {
            while (true)
            {
                auto element = ring_buffer_->GetReadyElement();
                if (element.has_value())
                {
                    element.value().get().status_ = TraceJobStatus::kEmpty;
                    consumed_count.fetch_add(1, std::memory_order_release);
                }
                else
                {
                    break;
                }
            }
            if (consumed_count.load(std::memory_order_acquire) >= kTotalElements)
            {
                break;
            }
            std::this_thread::sleep_for(kProcessPeriodicity);
        }
    });

    for (auto& producer : producers)
    {
        producer.join();
    }
    consumer.join();
    EXPECT_GE(produced_count.load(), kThreadCount * kElementsPerProducer);
    EXPECT_GE(consumed_count.load(), kThreadCount * kElementsPerProducer);
}

TEST_P(ShmRingBufferFixture, IsStateStructLockFree)
{
    RecordProperty("Verifies", "SCR-41486856");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies that component offers a lock-free interface accepting IPC data"
                   "to be passed to the interface of the network stack");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::atomic<ShmRingBufferState> stateStruct{};
    ASSERT_TRUE(stateStruct.is_lock_free());
}

TEST_P(ShmRingBufferFixture, GetStatisticsWhenNotInitialized)
{
    CreateRingBufferWithSize(number_of_elements);
    auto result = ring_buffer_->GetStatistics();
    ASSERT_FALSE(result.has_value());
    if (GetParam())
    {
        ASSERT_EQ(result.error(), ErrorCode::kRingBufferNotInitializedRecoverable);
    }
    else
    {
        ASSERT_EQ(result.error(), ErrorCode::kGenericErrorRecoverable);
    }
}

TEST_P(ShmRingBufferFixture, GetSizeWhenNotInitialized)
{
    CreateRingBufferWithSize(number_of_elements);
    auto result = ring_buffer_->GetSize();
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), ErrorCode::kRingBufferNotInitializedRecoverable);
}

TEST_P(ShmRingBufferFixture, GetUseCountWhenNotInitialized)
{
    CreateRingBufferWithSize(number_of_elements);
    auto result = ring_buffer_->GetUseCount();
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), ErrorCode::kRingBufferNotInitializedRecoverable);
}

TEST_P(ShmRingBufferFixture, GetUseCountInvalidStateSizeWise)
{
    CreateRingBufferWithSize(0);
    ring_buffer_->CreateOrOpen();
    auto result = ring_buffer_->GetUseCount();
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), ErrorCode::kRingBufferInvalidStateRecoverable);
}

TEST_P(ShmRingBufferFixture, GetEmptyElementWhenNotInitialized)
{
    CreateRingBufferWithSize(number_of_elements);
    auto result = ring_buffer_->GetEmptyElement();
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), ErrorCode::kRingBufferNotInitializedRecoverable);
}

TEST_P(ShmRingBufferFixture, GetEmptyElementInvalidStateSizeWise)
{
    CreateRingBufferWithSize(0);
    ring_buffer_->CreateOrOpen();
    auto result = ring_buffer_->GetEmptyElement();
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), ErrorCode::kRingBufferInvalidStateRecoverable);
}

TEST_P(ShmRingBufferFixture, GetReadyElementWhenNotInitialized)
{
    ShmRingBuffer shm_ring_buffer{kRingBufferSharedMemoryPathTest, 0};
    auto result = shm_ring_buffer.GetReadyElement();
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), ErrorCode::kRingBufferNotInitializedRecoverable);
}

TEST_P(ShmRingBufferFixture, GetReadyElementInvalidStateSizeWise)
{
    CreateRingBufferWithSize(0);
    ASSERT_TRUE(ring_buffer_->CreateOrOpen());
    auto result = ring_buffer_->GetReadyElement();
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), ErrorCode::kRingBufferInvalidStateRecoverable);
}

TEST_P(ShmRingBufferFixture, GetSize)
{
    CreateRingBufferWithSize(number_of_elements);
    ring_buffer_->CreateOrOpen();
    auto result = ring_buffer_->GetSize();
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), number_of_elements);
}

TEST_P(ShmRingBufferFixture, GetReadyElementValidState)
{
    CreateRingBufferWithSize(1);
    ring_buffer_->CreateOrOpen();
    ASSERT_TRUE(ring_buffer_->IsBufferEmpty());
    GetElementsWithSuccess(*ring_buffer_, 1, empty);
    auto result = ring_buffer_->GetReadyElement();
    ASSERT_TRUE(result.has_value());
}

TEST(ShmRingBufferWithMockSharedMemory, CreateSharedMemoryGetMemoryResourceProxyNull)
{
    // Prepare shared memory resource mock
    auto shared_memory_resource_mock = std::make_shared<score::memory::shared::SharedMemoryResourceMock>();
    EXPECT_CALL(*shared_memory_resource_mock, IsShmInTypedMemory).WillRepeatedly(Return(true));
    EXPECT_CALL(*shared_memory_resource_mock, getMemoryResourceProxy).WillOnce(Return(nullptr));

    // Prepare shared memory factory mock
    auto shared_memory_factory_mock = std::make_shared<score::memory::shared::SharedMemoryFactoryMock>();
    EXPECT_CALL(*shared_memory_factory_mock, Open).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(*shared_memory_factory_mock, Create).WillRepeatedly(Return(shared_memory_resource_mock));

    // inject shared memory factory mock
    score::memory::shared::SharedMemoryFactory::InjectMock(shared_memory_factory_mock.get());

    ShmRingBuffer dummy_ring_buffer{"/shmem_test", 100};
    const auto create_result = dummy_ring_buffer.CreateOrOpen();
    ASSERT_FALSE(create_result.has_value());
    ASSERT_EQ(create_result.error(), ErrorCode::kRingBufferInvalidMemoryResourceRecoverable);
    score::memory::shared::SharedMemoryFactory::InjectMock(nullptr);
}

TEST(ShmRingBufferWithMockSharedMemory, OpenSharedMemoryGetUsableBaseAddressNull)
{
    // Prepare shared memory resource mock
    auto shared_memory_resource_mock = std::make_shared<score::memory::shared::SharedMemoryResourceMock>();
    EXPECT_CALL(*shared_memory_resource_mock, IsShmInTypedMemory).WillRepeatedly(Return(true));
    EXPECT_CALL(*shared_memory_resource_mock, getUsableBaseAddress()).WillOnce(Return(nullptr));
    // Prepare shared memory factory mock
    auto shared_memory_factory_mock = std::make_shared<score::memory::shared::SharedMemoryFactoryMock>();
    EXPECT_CALL(*shared_memory_factory_mock, Open).WillRepeatedly(Return(shared_memory_resource_mock));

    // inject shared memory factory mock
    score::memory::shared::SharedMemoryFactory::InjectMock(shared_memory_factory_mock.get());
    ShmRingBuffer dummy_ring_buffer{"/shmem_test", 100};

    const auto create_result = dummy_ring_buffer.CreateOrOpen();
    ASSERT_FALSE(create_result.has_value());
    ASSERT_EQ(create_result.error(), ErrorCode::kRingBufferInvalidMemoryResourceRecoverable);
    score::memory::shared::SharedMemoryFactory::InjectMock(nullptr);
}

TEST_P(ShmRingBufferFixture, CreateSharedMemoryStatisticsCreationGetUsableAddressNull)
{
    if (GetParam())
    {
        CreateRingBufferWithSize(number_of_elements);
        SharedMemoryFactoryMock shared_memory_factory;
        SharedMemoryFactory::InjectMock(&shared_memory_factory);
        auto shared_memory_resource_mock = std::make_shared<SharedMemoryResourceMock>();
        {
            InSequence seq;
            EXPECT_CALL(shared_memory_factory, Open(_, _, _)).WillOnce(Return(ByMove(shared_memory_resource_mock)));
            EXPECT_CALL(*shared_memory_resource_mock, getUsableBaseAddress()).Times(1).WillOnce(Return(nullptr));
        }
        const auto create_result = ring_buffer_->CreateOrOpen(number_of_elements);
        ASSERT_FALSE(create_result.has_value());
        ASSERT_EQ(create_result.error(), ErrorCode::kRingBufferInvalidMemoryResourceRecoverable);
        SharedMemoryFactory::InjectMock(nullptr);
    }
}

TEST_P(ShmRingBufferFixture, CreateSharedMemoryStatisticsCreationSuccess)
{
    if (GetParam())
    {
        auto mock_memory_resource1 = std::make_shared<score::memory::shared::SharedMemoryResourceHeapAllocatorMock>(1);
        auto mock_memory_resource2 = std::make_shared<score::memory::shared::SharedMemoryResourceHeapAllocatorMock>(2);
        SharedMemoryFactoryMock shared_memory_factory;
        {
            InSequence seq;
            SharedMemoryFactory::InjectMock(&shared_memory_factory);
            EXPECT_CALL(shared_memory_factory, Open(_, _, _)).WillOnce(Return(nullptr));
            EXPECT_CALL(shared_memory_factory, Remove(_)).Times(1);
            EXPECT_CALL(shared_memory_factory, Create(_, _, _, _, _)).WillOnce(Return(mock_memory_resource1));
            EXPECT_CALL(shared_memory_factory, Open(_, _, _)).WillOnce(Return(nullptr));
            EXPECT_CALL(shared_memory_factory, Remove(_)).Times(1);
            EXPECT_CALL(shared_memory_factory, Create(_, _, _, _, _)).WillOnce(Return(mock_memory_resource2));
        }
        ShmRingBuffer dummy_ring_buffer{"/shmem_test", number_of_elements, GetParam()};
        const auto create_result = dummy_ring_buffer.CreateOrOpen(true);
        ASSERT_TRUE(create_result.has_value());
        mock_memory_resource1.reset();
        mock_memory_resource2.reset();
        SharedMemoryFactory::InjectMock(nullptr);
    }
}

TEST_P(ShmRingBufferFixture, CreateOrOpenStatisticFailWhileOpeningShmRingBuffer)
{
    if (GetParam())
    {
        std::shared_ptr<LocalMemoryResource> memory_ = std::make_shared<LocalMemoryResource>();
        // auto mock_memory_resource1 = std::make_shared<score::memory::shared::SharedMemoryResourceHeapAllocatorMock>(1);
        auto mock_memory_resource2 = std::make_shared<score::memory::shared::SharedMemoryResourceHeapAllocatorMock>(2);
        SharedMemoryFactoryMock shared_memory_factory;
        {
            InSequence seq;
            SharedMemoryFactory::InjectMock(&shared_memory_factory);
            EXPECT_CALL(shared_memory_factory, Open(_, _, _)).WillOnce(Return(memory_));
            EXPECT_CALL(shared_memory_factory, Open(_, _, _)).WillOnce(Return(mock_memory_resource2));
        }
        ShmRingBuffer dummy_ring_buffer{"/shmem_test", number_of_elements, GetParam()};
        const auto create_result = dummy_ring_buffer.CreateOrOpen(true);
        ASSERT_FALSE(create_result.has_value());
        SharedMemoryFactory::InjectMock(nullptr);
    }
}

TEST_P(ShmRingBufferFixture, CreateSharedMemoryStatisticsOpenFailed)
{
    if (GetParam())
    {
        auto mock_memory_resource1 = std::make_shared<score::memory::shared::SharedMemoryResourceHeapAllocatorMock>(1);
        auto mock_memory_resource2 = std::make_shared<score::memory::shared::SharedMemoryResourceHeapAllocatorMock>(2);

        SharedMemoryFactoryMock shared_memory_factory;
        {
            InSequence seq;
            SharedMemoryFactory::InjectMock(&shared_memory_factory);
            EXPECT_CALL(shared_memory_factory, Open(_, _, _)).WillOnce(Return(nullptr));
            EXPECT_CALL(shared_memory_factory, Remove(_)).Times(1);
            EXPECT_CALL(shared_memory_factory, Create(_, _, _, _, _)).WillOnce(Return(mock_memory_resource1));
            EXPECT_CALL(shared_memory_factory, Open(_, _, _)).WillOnce(Return(mock_memory_resource2));
        }
        ShmRingBuffer dummy_ring_buffer{"/shmem_test", number_of_elements, GetParam()};
        const auto create_result = dummy_ring_buffer.CreateOrOpen(true);
        ASSERT_FALSE(create_result.has_value());
        mock_memory_resource1.reset();
        mock_memory_resource2.reset();

        SharedMemoryFactory::InjectMock(nullptr);
    }
}

TEST_P(ShmRingBufferFixture, CreateSharedMemoryStatisticsCreateFailed)
{
    if (GetParam())
    {
        auto mock_memory_resource1 = std::make_shared<score::memory::shared::SharedMemoryResourceHeapAllocatorMock>(1);
        auto mock_memory_resource2 = std::make_shared<score::memory::shared::SharedMemoryResourceHeapAllocatorMock>(2);

        SharedMemoryFactoryMock shared_memory_factory;
        {
            InSequence seq;
            SharedMemoryFactory::InjectMock(&shared_memory_factory);
            EXPECT_CALL(shared_memory_factory, Open(_, _, _)).WillOnce(Return(nullptr));
            EXPECT_CALL(shared_memory_factory, Remove(_)).Times(1);
            EXPECT_CALL(shared_memory_factory, Create(_, _, _, _, _)).WillOnce(Return(mock_memory_resource1));
            EXPECT_CALL(shared_memory_factory, Open(_, _, _)).WillOnce(Return(nullptr));
            EXPECT_CALL(shared_memory_factory, Remove(_)).Times(1);
            EXPECT_CALL(shared_memory_factory, Create(_, _, _, _, _)).WillOnce(Return(nullptr));
        }
        ShmRingBuffer dummy_ring_buffer{"/shmem_test", number_of_elements, GetParam()};
        const auto create_result = dummy_ring_buffer.CreateOrOpen(true);
        ASSERT_FALSE(create_result.has_value());
        mock_memory_resource1.reset();
        mock_memory_resource2.reset();

        SharedMemoryFactory::InjectMock(nullptr);
    }
}

TEST_P(ShmRingBufferFixture, TryFetchElementWhenNotInitialized)
{
    ShmRingBuffer shm_ring_buffer{kRingBufferSharedMemoryPathTest, 0};
    auto result = shm_ring_buffer.TryFetchElement();
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), ErrorCode::kRingBufferNotInitializedRecoverable);
}

TEST_P(ShmRingBufferFixture, TryFetchElementInvalidElement)
{
    CreateRingBufferWithSize(1);
    ring_buffer_->CreateOrOpen();
    auto element = ring_buffer_->GetEmptyElement();
    ASSERT_TRUE(element.has_value());
    element.value().get().global_context_id_ = {0, 0};
    element.value().get().status_ = TraceJobStatus::kInvalid;

    auto result = ring_buffer_->TryFetchElement();
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), ErrorCode::kRingBufferEmptyRecoverable);
}

TEST_P(ShmRingBufferFixture, TryFetchElementReadyElement)
{
    CreateRingBufferWithSize(1);
    ring_buffer_->CreateOrOpen();
    auto element = ring_buffer_->GetEmptyElement();
    ASSERT_TRUE(element.has_value());
    element.value().get().global_context_id_ = {0, 0};
    element.value().get().status_ = TraceJobStatus::kTraced;

    auto result = ring_buffer_->TryFetchElement();
    ASSERT_FALSE(result.has_value());
}

TEST_P(ShmRingBufferFixture, TryFetchMaxRetries)
{
    CreateRingBufferWithSize(number_of_elements);
    ring_buffer_->CreateOrOpen();
    for (std::uint16_t i = 0; i < number_of_elements; i++)
    {
        auto element = ring_buffer_->GetEmptyElement();
        ASSERT_TRUE(element.has_value());
        element.value().get().global_context_id_ = {0, i};
        element.value().get().status_ = TraceJobStatus::kInvalid;
    }

    auto result = ring_buffer_->TryFetchElement();
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), ErrorCode::kRingBufferNoReadyElementRecoverable);
}

TEST_P(ShmRingBufferFixture, BufferDoubleCreateOrOpenShallPass)
{
    CreateRingBufferWithSize(number_of_elements);
    auto result = ring_buffer_->CreateOrOpen();
    ASSERT_TRUE(result.has_value());

    result = ring_buffer_->CreateOrOpen();
    ASSERT_TRUE(result.has_value());
}

TEST_P(ShmRingBufferFixture, CreteOrOpenShallCreateAfterFirstCallAndOpenAfterSecondCall)
{
    CreateRingBufferWithSize(number_of_elements);
    auto result = ring_buffer_->CreateOrOpen();
    ASSERT_TRUE(result.has_value());
    ShmRingBuffer tmd_pool_slave{kRingBufferSharedMemoryPathTest, GetParam()};
    result = tmd_pool_slave.CreateOrOpen();
    ASSERT_TRUE(result.has_value());
}

TEST_P(ShmRingBufferFixture, CreateOrOpenShallCreateRingBufferAgainAfterItsClosedSuccessfully)
{
    CreateRingBufferWithSize(number_of_elements);
    auto result = ring_buffer_->CreateOrOpen();
    ASSERT_TRUE(result.has_value());
    ring_buffer_->Close();
    result = ring_buffer_->CreateOrOpen();
    ASSERT_TRUE(result.has_value());
}

TEST_P(ShmRingBufferFixture, CreateOrOpenShallSuccessButNotClearElementsVectorSinceItsOpenedFromTwoDifferentInstances)
{
    CreateRingBufferWithSize(number_of_elements);
    auto result = ring_buffer_->CreateOrOpen();
    ASSERT_TRUE(result.has_value());
    ShmRingBuffer tmd_pool_slave{kRingBufferSharedMemoryPathTest, GetParam()};
    result = tmd_pool_slave.CreateOrOpen();
    ASSERT_TRUE(result.has_value());
    ring_buffer_->Close();
    result = ring_buffer_->CreateOrOpen();
    ASSERT_TRUE(result.has_value());
}

TEST_P(ShmRingBufferFixture, ResetShallDoNothingIfRingBufferIsOpenedByTwoInstances)
{
    CreateRingBufferWithSize(number_of_elements);
    auto result = ring_buffer_->CreateOrOpen();
    ASSERT_TRUE(result.has_value());
    ShmRingBuffer tmd_pool_slave{kRingBufferSharedMemoryPathTest, GetParam()};
    result = tmd_pool_slave.CreateOrOpen();
    ASSERT_TRUE(result.has_value());
    ring_buffer_->Reset();
    result = ring_buffer_->CreateOrOpen();
    ASSERT_TRUE(result.has_value());
}

TEST_P(ShmRingBufferFixture, CreateOrOpenShallCreateWithTrueFlagForOwner)
{
    CreateRingBufferWithSize(number_of_elements);
    auto result = ring_buffer_->CreateOrOpen(true);
    ASSERT_TRUE(result.has_value());
    ShmRingBuffer tmd_pool_slave{kRingBufferSharedMemoryPathTest, GetParam()};
    result = tmd_pool_slave.CreateOrOpen();
    ASSERT_TRUE(result.has_value());
}

TEST_P(ShmRingBufferFixture, BufferCreateTooBig)
{
    CreateRingBufferWithSize(too_big_number_of_elements);
    auto result = ring_buffer_->CreateOrOpen();
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), ErrorCode::kRingBufferTooLargeRecoverable);
}

TEST_P(ShmRingBufferFixture, WriteDataUntilFull)
{
    CreateRingBufferWithSize(number_of_elements);
    auto result = ring_buffer_->CreateOrOpen();
    ASSERT_TRUE(result.has_value());

    GetElementsWithSuccess(*ring_buffer_, number_of_elements, empty);

    auto element_master = ring_buffer_->GetEmptyElement();
    ASSERT_FALSE(element_master.has_value());
    ASSERT_EQ(element_master.error(), ErrorCode::kRingBufferFullRecoverable);
}

TEST_P(ShmRingBufferFixture, WriteAndReadData)
{
    CreateRingBufferWithSize(number_of_elements);
    ShmRingBuffer tmd_pool_slave{kRingBufferSharedMemoryPathTest, number_of_elements, GetParam()};

    auto result = ring_buffer_->CreateOrOpen();
    ASSERT_TRUE(result.has_value());

    result = tmd_pool_slave.CreateOrOpen();

    ASSERT_TRUE(result.has_value());

    GetElementsWithSuccess(*ring_buffer_, number_of_elements, empty);

    auto element_master = ring_buffer_->GetEmptyElement();
    ASSERT_FALSE(element_master.has_value());

    GetElementsWithSuccess(tmd_pool_slave, number_of_elements, ready);

    auto element_slave = tmd_pool_slave.TryFetchElement();
    ASSERT_FALSE(element_slave.has_value());
    ASSERT_EQ(element_slave.error(), ErrorCode::kRingBufferEmptyRecoverable);
}

TEST_P(ShmRingBufferFixture, MixedOperations)
{
    CreateRingBufferWithSize(number_of_elements);
    ShmRingBuffer tmd_pool_slave{kRingBufferSharedMemoryPathTest, number_of_elements, GetParam()};
    auto result = ring_buffer_->CreateOrOpen();
    ASSERT_TRUE(result.has_value());
    result = tmd_pool_slave.CreateOrOpen();
    ASSERT_TRUE(result.has_value());

    GetElementsWithSuccess(*ring_buffer_, 5, empty);
    GetElementsWithSuccess(tmd_pool_slave, 4, ready);
    GetElementsWithSuccess(*ring_buffer_, 5, empty);
    GetElementsWithSuccess(tmd_pool_slave, 4, ready);
    GetElementsWithSuccess(*ring_buffer_, 5, empty);
    GetElementsWithSuccess(tmd_pool_slave, 4, ready);
    GetElementsWithSuccess(*ring_buffer_, 5, empty);
    GetElementsWithSuccess(tmd_pool_slave, 4, ready);
    GetElementsWithSuccess(tmd_pool_slave, 4, ready);

    auto element_master = ring_buffer_->TryFetchElement();
    ASSERT_FALSE(element_master.has_value());
}

TEST_P(ShmRingBufferFixture, MonitorBufferDepth)
{
    CreateRingBufferWithSize(number_of_elements);
    ShmRingBuffer tmd_pool_slave{kRingBufferSharedMemoryPathTest, number_of_elements};

    auto result = ring_buffer_->CreateOrOpen();
    ASSERT_TRUE(result.has_value());
    result = tmd_pool_slave.CreateOrOpen();
    ASSERT_TRUE(result.has_value());

    GetElementsWithSuccess(*ring_buffer_, number_of_elements - 2, empty);
    auto count = ring_buffer_->GetUseCount();
    ASSERT_TRUE(count.has_value());
    ASSERT_EQ(number_of_elements - 2, count.value());

    GetElementsWithSuccess(*ring_buffer_, number_of_elements - 2, ready);
    count = ring_buffer_->GetUseCount();
    ASSERT_TRUE(count.has_value());
    ASSERT_EQ(0, count.value());

    GetElementsWithSuccess(*ring_buffer_, number_of_elements - 3, empty);
    count = ring_buffer_->GetUseCount();
    ASSERT_TRUE(count.has_value());
    ASSERT_EQ(number_of_elements - 3, count.value());
}

TEST_P(AtomicIndirectorMockFixture, FailedAtomicExchange)
{
    CreateRingBufferWithSize(number_of_elements);
    auto result = ring_buffer_->CreateOrOpen();
    ASSERT_TRUE(result.has_value());
    {
        InSequence s;
        if (GetParam())
        {
            EXPECT_CALL(*atomic_statistics_mock_, fetch_add(_, _)).Times(1);
        }

        for (std::uint32_t trial = 0; trial < get_write_element_max_retries; ++trial)
        {
            EXPECT_CALL(*atomic_state_mock_, load(_)).Times(1).WillOnce(Return(ring_buffer_state_));
            EXPECT_CALL(*atomic_state_mock_, compare_exchange_weak(_, _, _)).Times(1).WillOnce(Return(false));
        }

        if (GetParam())
        {
            EXPECT_CALL(*atomic_statistics_mock_, fetch_add(_, _)).Times(3);
        }
    }

    auto element = ring_buffer_->GetEmptyElement();
    ASSERT_FALSE(element.has_value());
    ASSERT_EQ(element.error(), ErrorCode::kRingBufferNoEmptyElementRecoverable);
}

TEST_P(AtomicIndirectorMockFixture, ResetShallFailWhenAtomicUpdateFail)
{
    CreateRingBufferWithSize(number_of_elements);
    auto first_client_result = ring_buffer_->CreateOrOpen();
    auto second_client_result = ring_buffer_->CreateOrOpen();
    ASSERT_TRUE(first_client_result.has_value());
    ASSERT_TRUE(second_client_result.has_value());
    {
        EXPECT_CALL(*atomic_state_mock_, load(_)).Times(1).WillOnce(Return(ring_buffer_state_));
        InSequence s;
        for (std::uint32_t trial = 0; trial < get_write_element_max_retries; ++trial)
        {
            EXPECT_CALL(*atomic_state_mock_, compare_exchange_weak(_, _, _)).Times(1).WillOnce(Return(false));
        }
    }
    ring_buffer_->Reset();
}

TEST_P(AtomicIndirectorMockFixture, RetriedAtomicExchangeGetEmpty)
{
    CreateRingBufferWithSize(number_of_elements);
    auto result = ring_buffer_->CreateOrOpen();
    ASSERT_TRUE(result.has_value());

    {
        InSequence s;
        if (GetParam())
        {
            EXPECT_CALL(*atomic_statistics_mock_, fetch_add(_, _)).Times(1);
        }

        for (std::uint32_t trial = 0; trial < get_write_element_max_retries - 1; ++trial)
        {
            EXPECT_CALL(*atomic_state_mock_, load(_)).Times(1).WillOnce(Return(ring_buffer_state_));
            EXPECT_CALL(*atomic_state_mock_, compare_exchange_weak(_, _, _)).Times(1).WillOnce(Return(false));
        }
        EXPECT_CALL(*atomic_state_mock_, load(_)).Times(1).WillOnce(Return(ring_buffer_state_));
        EXPECT_CALL(*atomic_state_mock_, compare_exchange_weak(_, _, _)).Times(1).WillOnce(Return(true));
        EXPECT_CALL(*atomic_status_mock_, store(TraceJobStatus::kAllocated, _)).Times(1);

        if (GetParam())
        {
            EXPECT_CALL(*atomic_statistics_mock_, fetch_add(_, _)).Times(2);
        }
    }

    auto element = ring_buffer_->GetEmptyElement();
    ASSERT_TRUE(element.has_value());
    element.value().get().status_ = TraceJobStatus::kEmpty;
}

TEST_P(AtomicIndirectorMockFixture, TryFetchElementInvalidElementStateUpdateFails)
{
    CreateRingBufferWithSize(number_of_elements);
    auto result = ring_buffer_->CreateOrOpen();
    ASSERT_TRUE(result.has_value());

    GetElementsWithSuccess(*ring_buffer_, number_of_elements, empty, TraceJobStatus::kInvalid);
    {
        InSequence sequence;
        for (std::uint32_t trial = 0; trial < get_write_element_max_retries; ++trial)
        {
            EXPECT_CALL(*atomic_state_mock_, load(_)).Times(1).WillOnce(Return(ring_buffer_state_));
            EXPECT_CALL(*atomic_state_mock_, compare_exchange_weak(_, _, _)).Times(1).WillOnce(Return(false));
        }
    }

    auto element = ring_buffer_->TryFetchElement();
    ASSERT_FALSE(element.has_value());
    ASSERT_EQ(element.error(), ErrorCode::kRingBufferNoReadyElementRecoverable);
}

TEST_P(AtomicIndirectorMockFixture, GetStatistics)
{
    CreateRingBufferWithSize(number_of_elements);
    auto result = ring_buffer_->CreateOrOpen();
    ShmRingBufferImpl<AtomicIndirectorMock> tmd_pool_slave{kRingBufferSharedMemoryPathTest, number_of_elements};
    ASSERT_TRUE(result.has_value());
    result = tmd_pool_slave.CreateOrOpen();
    ASSERT_TRUE(result.has_value());

    std::uint64_t cas_trial = 0U;

    {
        InSequence s;
        EXPECT_CALL(*atomic_statistics_mock_, load(_)).Times(1).WillOnce(Return(cas_trial));
        EXPECT_CALL(*atomic_statistics_mock_, load(_)).Times(1).WillOnce(Return(cas_trial));
        EXPECT_CALL(*atomic_statistics_mock_, load(_)).Times(1).WillOnce(Return(cas_trial));
        EXPECT_CALL(*atomic_statistics_mock_, load(_)).Times(1).WillOnce(Return(cas_trial));
        EXPECT_CALL(*atomic_statistics_mock_, load(_)).Times(1).WillOnce(Return(cas_trial));
    }

    EXPECT_TRUE(tmd_pool_slave.GetStatistics().has_value());
}

TEST_P(AtomicIndirectorMockFixture, RetriedAtomicExchangeGetReady)
{
    CreateRingBufferWithSize(number_of_elements);
    auto result = ring_buffer_->CreateOrOpen();
    ASSERT_TRUE(result.has_value());

    GetElementsWithSuccess(*ring_buffer_, 1, empty);

    {
        InSequence s;
        for (std::uint32_t trial = 0; trial < get_write_element_max_retries - 1; ++trial)
        {
            EXPECT_CALL(*atomic_state_mock_, load(_)).Times(1).WillOnce(Return(ring_buffer_state_));
            EXPECT_CALL(*atomic_state_mock_, compare_exchange_weak(_, _, _)).Times(1).WillOnce(Return(false));
        }
        EXPECT_CALL(*atomic_state_mock_, load(_)).Times(1).WillOnce(Return(ring_buffer_state_));
        EXPECT_CALL(*atomic_state_mock_, compare_exchange_weak(_, _, _)).Times(1).WillOnce(Return(true));
    }

    auto element = ring_buffer_->TryFetchElement();
    ASSERT_TRUE(element.has_value());
}

TEST_P(ShmRingBufferFixture, CrateOrOpenShallFailWhenSharedMemoryFactoryFailToOpen)
{
    mock_shared_memory_factory_ = std::make_shared<SharedMemoryFactoryMock>();
    SharedMemoryFactory::InjectMock(mock_shared_memory_factory_.get());
    ShmRingBuffer tmd_pool_master{kRingBufferSharedMemoryPathTest, number_of_elements};
    EXPECT_CALL(*mock_shared_memory_factory_, Open).WillOnce(Return(mock_shared_memory_resource_));
    auto result = tmd_pool_master.CreateOrOpen();
    ASSERT_FALSE(result.has_value());
    SharedMemoryFactory::InjectMock(nullptr);
}

TEST_P(ShmRingBufferFixture, CrateOrOpenShallFailWhenSharedMemoryFactoryFailToCreate)
{
    mock_shared_memory_factory_ = std::make_shared<SharedMemoryFactoryMock>();
    SharedMemoryFactory::InjectMock(mock_shared_memory_factory_.get());
    ShmRingBuffer tmd_pool_master{kRingBufferSharedMemoryPathTest, number_of_elements};
    EXPECT_CALL(*mock_shared_memory_factory_, Create).WillOnce(Return(nullptr));
    auto result = tmd_pool_master.CreateOrOpen();
    ASSERT_FALSE(result.has_value());
    SharedMemoryFactory::InjectMock(nullptr);
}
INSTANTIATE_TEST_SUITE_P(ShmRingBufferTests, AtomicIndirectorMockFixture, testing::Values(true, false));
INSTANTIATE_TEST_SUITE_P(ShmRingBufferTests, ShmRingBufferFixture, testing::Values(true, false));
