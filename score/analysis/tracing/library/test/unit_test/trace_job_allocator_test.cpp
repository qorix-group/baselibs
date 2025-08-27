///
/// @file shm_data_chunk_list_test.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API shm_data_chunk_list test source
///

#include "score/analysis/tracing/library/generic_trace_api/trace_job_allocator/trace_job_allocator.h"
#include "score/analysis/tracing/common/flexible_circular_allocator/lockless_flexible_circular_allocator.h"
#include "score/analysis/tracing/common/flexible_circular_allocator/test/mocks/flexible_circular_allocator_mock.h"
#include "score/analysis/tracing/common/interface_types/shared_memory_location_helpers.h"
#include "score/analysis/tracing/library/generic_trace_api/chunk_list/local_data_chunk_list.h"
#include "score/analysis/tracing/library/generic_trace_api/chunk_list/shm_data_chunk_list.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_container/trace_job_container.h"
#include "score/analysis/tracing/library/test/unit_test/local_memory_resource.h"
#include "score/analysis/tracing/plugin/ipc_trace_plugin/interface/ara_com_meta_info_trace_format.h"
#include "score/analysis/tracing/shm_ring_buffer/mock_shm_ring_buffer.h"
#include "score/memory/shared/shared_memory_resource_mock.h"
#include "score/result/result.h"
#include "platform/aas/mw/time/HWLoggerTime/receiver/eptm_receiver_mock.h"
#include "platform/aas/mw/time/HighPrecisionLocalSteadyClock/high_precision_local_steady_clock.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>
#include <utility>
#include <vector>

using namespace score::analysis::tracing;
using namespace score::memory::shared;
using testing::_;
using ::testing::Return;
using ::testing::StrictMock;

static constexpr std::size_t kCircularAllocatorSize = 5000;
static constexpr std::size_t kAllocatedMemorySize = 10000;

class TraceJobAllocatorFixture : public ::testing::Test
{
  public:
    static constexpr std::size_t number_of_elements = 8u;
    static constexpr std::size_t additional_elements = 2u;
    static constexpr std::size_t total_size = 186u;
    static constexpr std::size_t test_value = 10u;

    // deamon_ring_buffer to be used with allocator with no logger time
    std::unique_ptr<MockShmRingBuffer> daemon_ring_buffer_;
    MockShmRingBuffer* daemon_ring_buffer_raw_;

    // deamon_ring_buffer to be used with allocator with logger time
    std::unique_ptr<MockShmRingBuffer> daemon_ring_buffer2_;
    MockShmRingBuffer* daemon_ring_buffer_raw2_;

    // deamon_ring_buffer to be used with allocator with full container
    std::unique_ptr<MockShmRingBuffer> daemon_ring_buffer3_;
    MockShmRingBuffer* daemon_ring_buffer_raw3_;

    std::uint8_t* memory_block_ptr;
    std::shared_ptr<LocalMemoryResource> memory_;
    std::shared_ptr<score::memory::shared::SharedMemoryResourceMock> memory_mock_;
    std::unique_ptr<TraceJobAllocator> allocator_, allocator_logger_time_, allocator_full_container_;
    std::shared_ptr<StrictMock<score::mw::time::hwloggertime::EptmReceiverMock>> time{};
    ShmObjectHandle memory_handle_{1};
    ShmDataChunkList shm_data_chunk_list_{};
    SharedMemoryChunk shm_chunk_{};
    LocalDataChunkList local_data_chunk_list_{};
    LocalDataChunk local_chunk_{};
    std::shared_ptr<TraceJobContainer> container_ = std::make_shared<TraceJobContainer>();
    std::shared_ptr<TraceJobContainer> full_container_ = std::make_shared<TraceJobContainer>();
    TraceClientId trace_client_id_{0x55};
    TraceContextId trace_context_id_{0x1234};
    TracePointType trace_point_type_{TracePointType::SKEL_EVENT_SND};
    ServiceInstanceElement service_instance_element_{
        0,
        0,
        0,
        0,
        ServiceInstanceElement::VariantType{score::cpp::in_place_type<ServiceInstanceElement::EventIdType>,
                                            ServiceInstanceElement::EventIdType{0U}}};
    AraComProperties::TracePointDataId trace_point_data_id_{0};
    AraComProperties properties_{trace_point_type_, service_instance_element_, trace_point_data_id_};
    AraComMetaInfo meta_info_{properties_};
    BindingType binding_type = BindingType::kVector;
    AppIdType app_identifier{"AppId_1"};
    AraComMetaInfoTraceFormat meta_info_for_trace_{meta_info_, binding_type, app_identifier};
    std::uint8_t local_chunk_list_data_start[total_size];
    std::shared_ptr<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>> flexible_allocator_;

    void PrepareLocalChunkList()
    {
        std::uint8_t* pointer = local_chunk_list_data_start;
        for (std::uint8_t i = 0; i < number_of_elements; i++)
        {
            std::size_t size = i + test_value;
            memset(pointer, i, size);
            local_chunk_.size_ = size;
            local_chunk_.start_ = pointer;
            local_data_chunk_list_.Append(local_chunk_);
            pointer += size;
        }
        return;
    }

    void PrepareShmChunkList()
    {
        for (std::uint8_t i = 0; i < number_of_elements; i++)
        {
            std::size_t size = i + test_value;
            void* data_pointer = memory_->getMemoryResourceProxy()->allocate(size, alignof(std::max_align_t));
            memset(data_pointer, i, size);
            shm_chunk_.size_ = size;
            shm_chunk_.start_.shm_object_handle_ = memory_handle_;
            shm_chunk_.start_.offset_ = GetOffsetFromPointer(data_pointer, memory_).value();
            shm_data_chunk_list_.Append(shm_chunk_);
        }
        return;
    }

    void VerifyChunkList(SharedMemoryLocation location)
    {
        std::uint8_t* data_ptr = nullptr;
        AraComMetaInfoTraceFormat* received_meta_info = nullptr;
        ShmChunkVector* vector = GetPointerFromLocation<ShmChunkVector>(location, memory_);
        ASSERT_EQ(vector->size(), number_of_elements + additional_elements);

        for (std::size_t i = 0; i < number_of_elements; i++)
        {
            switch (i)
            {
                case 0:
                    ASSERT_EQ(vector->at(i).value().size_,
                              sizeof(score::mw::time::HighPrecisionLocalSteadyClock::time_point));
                    break;
                case 1:
                    ASSERT_EQ(vector->at(i).value().size_, sizeof(AraComMetaInfoTraceFormat));
                    data_ptr = GetPointerFromLocation<uint8_t>(vector->at(i).value().start_, memory_);
                    received_meta_info = reinterpret_cast<AraComMetaInfoTraceFormat*>(data_ptr);
                    ASSERT_EQ(meta_info_for_trace_, *received_meta_info);
                    break;
                default:
                    ASSERT_EQ(vector->at(i).value().size_, (i - additional_elements) + test_value);
                    ASSERT_EQ(vector->at(i).value().size_, (i - additional_elements) + test_value);
                    data_ptr = GetPointerFromLocation<uint8_t>(vector->at(i).value().start_, memory_);
                    for (std::size_t j = 0; j < (i - additional_elements) + test_value; j++)
                    {
                        ASSERT_EQ(*data_ptr, (i - additional_elements));
                        data_ptr++;
                    }
                    break;
            }
        }
        return;
    }

    void VerifyContainer(TraceJobContainerElement container)
    {
        ASSERT_EQ(container.original_trace_context_id_.client_id_, trace_client_id_);
        GlobalTraceContextId global_trace_context_id = container.ring_buffer_element_.get().global_context_id_;
        ASSERT_EQ(container.original_trace_context_id_.client_id_, global_trace_context_id.client_id_);
        ASSERT_EQ(container.original_trace_context_id_.context_id_, global_trace_context_id.context_id_);
        ASSERT_EQ(container.ring_buffer_element_.get().chunk_list_, container.chunk_list_);
        ASSERT_EQ(container.ring_buffer_element_.get().chunk_list_.shm_object_handle_, memory_handle_);
        ASSERT_EQ(container.ring_buffer_element_.get().status_, TraceJobStatus::kReady);
    }

  protected:
    void SetUp() override
    {
        memory_mock_ = std::make_shared<score::memory::shared::SharedMemoryResourceMock>();

        memory_ = std::make_shared<LocalMemoryResource>();

        daemon_ring_buffer_ = std::make_unique<MockShmRingBuffer>();
        daemon_ring_buffer_raw_ = daemon_ring_buffer_.get();

        daemon_ring_buffer2_ = std::make_unique<MockShmRingBuffer>();
        daemon_ring_buffer_raw2_ = daemon_ring_buffer2_.get();

        daemon_ring_buffer3_ = std::make_unique<MockShmRingBuffer>();
        daemon_ring_buffer_raw3_ = daemon_ring_buffer3_.get();

        ON_CALL(*daemon_ring_buffer_, Open()).WillByDefault(Return(score::Blank{}));
        ON_CALL(*daemon_ring_buffer2_, Open()).WillByDefault(Return(score::Blank{}));
        ON_CALL(*daemon_ring_buffer3_, Open()).WillByDefault(Return(score::Blank{}));

        meta_info_.trace_status_ = 0;
        memory_block_ptr = static_cast<std::uint8_t*>(
            memory_->getMemoryResourceProxy()->allocate(kAllocatedMemorySize, alignof(std::max_align_t)));
        flexible_allocator_ = std::make_shared<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>>(
            memory_block_ptr, kCircularAllocatorSize);

        allocator_ = std::make_unique<TraceJobAllocator>(
            container_, memory_, flexible_allocator_, std::move(daemon_ring_buffer_));
        allocator_->SetTraceMetaDataShmObjectHandle(memory_handle_);
        time = std::make_shared<StrictMock<score::mw::time::hwloggertime::EptmReceiverMock>>();
        allocator_logger_time_ = std::make_unique<TraceJobAllocator>(
            container_, memory_, flexible_allocator_, std::move(daemon_ring_buffer2_), time);
        allocator_logger_time_->SetTraceMetaDataShmObjectHandle(memory_handle_);
        allocator_->ResetRingBuffer();
        allocator_->CloseRingBuffer();
    }

    void TearDown() override
    {
        memory_->getMemoryResourceProxy()->deallocate(memory_block_ptr, kAllocatedMemorySize);
    }
};

constexpr std::size_t TraceJobAllocatorFixture::number_of_elements;
constexpr std::size_t TraceJobAllocatorFixture::total_size;
constexpr std::size_t TraceJobAllocatorFixture::additional_elements;
constexpr std::size_t TraceJobAllocatorFixture::test_value;

TEST_F(TraceJobAllocatorFixture, AllocateLocalJobTest)
{
    ShmRingBufferElement element;
    std::reference_wrapper<ShmRingBufferElement> shm_element_ref = element;

    // The one call for GetEmptyElement will return reference to a new element
    ON_CALL(*daemon_ring_buffer_raw_, GetEmptyElement()).WillByDefault(Return(shm_element_ref));

    // The one call for GetReadyElement will return reference to the same element returned by GetEmptyElement
    ON_CALL(*daemon_ring_buffer_raw_, GetReadyElement()).WillByDefault(Return(shm_element_ref));

    // Prepare contents of chunk list
    PrepareLocalChunkList();
    // Allocate trace job
    auto allocator_allocate_result = allocator_->AllocateLocalJob(
        trace_client_id_, meta_info_, binding_type, AppIdType{"AppId_1"}, local_data_chunk_list_);
    ASSERT_TRUE(allocator_allocate_result.has_value());
    // Verify container and check status of trace job
    auto container_element = container_->GetReadyElement();
    VerifyContainer(container_element.value().get().data_);
    ASSERT_EQ(container_element.value().get().data_.original_trace_context_id_.context_id_, 0);
    ASSERT_EQ(container_element.value().get().data_.job_type_, TraceJobType::kLocalJob);
    // Verify chunk list and set trace job done
    auto daemon_ring_result = daemon_ring_buffer_raw_->GetReadyElement();
    ASSERT_TRUE(daemon_ring_result.has_value());
    VerifyChunkList(daemon_ring_result.value().get().chunk_list_);
    daemon_ring_result.value().get().global_context_id_ = {0, 0};
    daemon_ring_result.value().get().status_ = TraceJobStatus::kEmpty;

    // Call deallocator for job
    auto deallocator_result = allocator_->DeallocateJob(container_element.value().get().data_.chunk_list_,
                                                        container_element.value().get().data_.job_type_);
    ASSERT_TRUE(deallocator_result.has_value());
}

TEST_F(TraceJobAllocatorFixture, AllocateLocalJobTestEmptyMetaInfo)
{
    ShmRingBufferElement element;
    std::reference_wrapper<ShmRingBufferElement> shm_element_ref = element;

    // The one call for GetEmptyElement will return reference to a new element
    ON_CALL(*daemon_ring_buffer_raw_, GetEmptyElement()).WillByDefault(Return(shm_element_ref));

    // The one call for GetReadyElement will return reference to the same element returned by GetEmptyElement
    ON_CALL(*daemon_ring_buffer_raw_, GetReadyElement()).WillByDefault(Return(shm_element_ref));

    MetaInfoVariants::type meta_info = DltMetaInfo(DltProperties());
    // Allocate trace job
    auto allocator_allocate_result = allocator_->AllocateLocalJob(
        trace_client_id_, meta_info, binding_type, AppIdType{"AppId_1"}, local_data_chunk_list_);
    ASSERT_FALSE(allocator_allocate_result.has_value());
    ASSERT_EQ(allocator_allocate_result.error(), ErrorCode::kNoMetaInfoProvidedRecoverable);
}

TEST_F(TraceJobAllocatorFixture, AllocateLocalJobWithLoggerTimeTest)
{
    RecordProperty("Verifies", "SCR-39766279");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if ChunkList prepared by library contains 3 types of chunks in correct order");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    ShmRingBufferElement shm_element;
    std::reference_wrapper<ShmRingBufferElement> shm_element_ref = shm_element;

    // The one call for GetEmptyElement will return reference to a new element
    ON_CALL(*daemon_ring_buffer_raw2_, GetEmptyElement()).WillByDefault(Return(shm_element_ref));

    // The one call for GetReadyElement will return reference to the same element returned by GetEmptyElement
    ON_CALL(*daemon_ring_buffer_raw2_, GetReadyElement()).WillByDefault(Return(shm_element_ref));

    // Prepare contents of chunk list
    PrepareLocalChunkList();

    EXPECT_CALL(*time.get(), Now());

    // Allocate trace job
    auto allocator_allocate_result = allocator_logger_time_->AllocateLocalJob(
        trace_client_id_, meta_info_, binding_type, AppIdType{"AppId_1"}, local_data_chunk_list_);
    ASSERT_TRUE(allocator_allocate_result.has_value());

    // Verify container and check status of trace job
    auto container_element = container_->GetReadyElement();
    VerifyContainer(container_element.value().get().data_);
    ASSERT_EQ(container_element.value().get().data_.original_trace_context_id_.context_id_, 0);
    ASSERT_EQ(container_element.value().get().data_.job_type_, TraceJobType::kLocalJob);

    // Verify chunk list and set trace job done
    auto daemon_ring_result = daemon_ring_buffer_raw2_->GetReadyElement();
    ASSERT_TRUE(daemon_ring_result.has_value());
    VerifyChunkList(daemon_ring_result.value().get().chunk_list_);
    daemon_ring_result.value().get().global_context_id_ = {0, 0};
    daemon_ring_result.value().get().status_ = TraceJobStatus::kEmpty;

    // Call deallocator for job
    auto deallocator_result = allocator_logger_time_->DeallocateJob(container_element.value().get().data_.chunk_list_,
                                                                    container_element.value().get().data_.job_type_);
    ASSERT_TRUE(deallocator_result.has_value());
}

TEST_F(TraceJobAllocatorFixture, AllocateShmJobTest)
{
    RecordProperty("Verifies", "SCR-39766279");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if ChunkList prepared by library contains 3 types of chunks in correct order");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    ShmRingBufferElement shm_element;
    std::reference_wrapper<ShmRingBufferElement> shm_element_ref = shm_element;

    // The one call for GetEmptyElement will return reference to a new element
    ON_CALL(*daemon_ring_buffer_raw_, GetEmptyElement()).WillByDefault(Return(shm_element_ref));

    // The one call for GetReadyElement will return reference to the same element returned by GetEmptyElement
    ON_CALL(*daemon_ring_buffer_raw_, GetReadyElement()).WillByDefault(Return(shm_element_ref));

    // Prepare contents of chunk list
    PrepareShmChunkList();

    // Allocate trace job
    auto allocator_allocate_result = allocator_->AllocateShmJob(
        trace_client_id_, meta_info_, binding_type, AppIdType{"AppId_1"}, shm_data_chunk_list_, trace_context_id_);
    ASSERT_TRUE(allocator_allocate_result.has_value());

    // Verify container and check status of trace job
    auto container_element = container_->GetReadyElement();
    VerifyContainer(container_element.value().get().data_);
    ASSERT_EQ(container_element.value().get().data_.original_trace_context_id_.context_id_, trace_context_id_);
    ASSERT_EQ(container_element.value().get().data_.job_type_, TraceJobType::kShmJob);

    // Verify chunk list, set trace job done and mark element in ring as empty.
    auto daemon_ring_result = daemon_ring_buffer_raw_->GetReadyElement();
    ASSERT_TRUE(daemon_ring_result.has_value());
    VerifyChunkList(daemon_ring_result.value().get().chunk_list_);
    daemon_ring_result.value().get().global_context_id_ = {0, 0};
    daemon_ring_result.value().get().status_ = TraceJobStatus::kEmpty;

    // Deallocate shared memory
    ShmChunkVector* vector =
        GetPointerFromLocation<ShmChunkVector>(container_element.value().get().data_.chunk_list_, memory_);
    for (std::size_t i = 2; i < vector->size(); i++)
    {
        void* data_ptr = GetPointerFromLocation<void>(vector->at(i).value().start_, memory_);
        memory_->getMemoryResourceProxy()->deallocate(data_ptr, vector->at(i).value().size_);
    }

    // Call deallocator for job
    auto deallocator_result = allocator_->DeallocateJob(container_element.value().get().data_.chunk_list_,
                                                        container_element.value().get().data_.job_type_);
    ASSERT_TRUE(deallocator_result.has_value());
}

TEST_F(TraceJobAllocatorFixture, AllocateShmJobTestFullContainer)
{
    std::shared_ptr<FlexibleCircularAllocatorMock> flexible_allocator_mock_ =
        std::make_shared<FlexibleCircularAllocatorMock>();
    std::vector<void*> allocated_addresses{};
    ON_CALL(*flexible_allocator_mock_, GetAvailableMemory()).WillByDefault([]() {
        return 0xABCD;
    });

    ON_CALL(*flexible_allocator_mock_, Allocate(_, _)).WillByDefault([&](const std::size_t size, const std::size_t) {
        auto allocated_memory = malloc(size);
        allocated_addresses.emplace_back(allocated_memory);
        return allocated_memory;
    });

    ON_CALL(*flexible_allocator_mock_, Deallocate(_, _)).WillByDefault([&](void* const address, const std::size_t) {
        for (auto it = allocated_addresses.begin(); it != allocated_addresses.end(); it++)
        {
            if (*it == address)
            {
                it = allocated_addresses.erase(it);
                free(address);  // Simulate deallocation
                return true;
            }
        }
        return true;
    });

    while (full_container_->Add({}))
        ;

    allocator_full_container_ = std::make_unique<TraceJobAllocator>(
        full_container_, memory_, flexible_allocator_mock_, std::move(daemon_ring_buffer3_));
    allocator_full_container_->SetTraceMetaDataShmObjectHandle(memory_handle_);
    ShmRingBufferElement shm_element;
    std::reference_wrapper<ShmRingBufferElement> shm_element_ref = shm_element;

    // The one call for GetEmptyElement will return reference to a new element
    ON_CALL(*daemon_ring_buffer_raw3_, GetEmptyElement()).WillByDefault(Return(shm_element_ref));

    // The one call for GetReadyElement will return reference to the same element returned by GetEmptyElement
    ON_CALL(*daemon_ring_buffer_raw3_, GetReadyElement()).WillByDefault(Return(shm_element_ref));

    // Allocate trace job
    auto allocator_allocate_result = allocator_full_container_->AllocateShmJob(
        trace_client_id_, meta_info_, binding_type, AppIdType{"AppId_1"}, shm_data_chunk_list_, trace_context_id_);
    ASSERT_FALSE(allocator_allocate_result.has_value());
    ASSERT_EQ(allocator_allocate_result.error(), ErrorCode::kNotEnoughMemoryRecoverable);

    allocator_allocate_result = allocator_full_container_->AllocateLocalJob(
        trace_client_id_, meta_info_, binding_type, AppIdType{"AppId_1"}, local_data_chunk_list_);
    ASSERT_FALSE(allocator_allocate_result.has_value());
    ASSERT_EQ(allocator_allocate_result.error(), ErrorCode::kNotEnoughMemoryRecoverable);

    allocator_full_container_.reset();
    for (auto it = allocated_addresses.begin(); it != allocated_addresses.end(); it++)
    {
        free(*it);
    }
}

TEST_F(TraceJobAllocatorFixture, AllocateShmJobWithLoggerTimeTest)
{
    RecordProperty("Verifies", "SCR-39766279");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if ChunkList prepared by library contains 3 types of chunks in correct order");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    ShmRingBufferElement shm_element;
    std::reference_wrapper<ShmRingBufferElement> shm_element_ref = shm_element;

    // The one call for GetEmptyElement will return reference to a new element
    ON_CALL(*daemon_ring_buffer_raw2_, GetEmptyElement()).WillByDefault(Return(shm_element_ref));

    // The one call for GetReadyElement will return reference to the same element returned by GetEmptyElement
    ON_CALL(*daemon_ring_buffer_raw2_, GetReadyElement()).WillByDefault(Return(shm_element_ref));

    // Prepare contents of chunk list
    PrepareShmChunkList();

    EXPECT_CALL(*time.get(), Now());

    // Allocate trace job
    auto allocator_allocate_result = allocator_logger_time_->AllocateShmJob(
        trace_client_id_, meta_info_, binding_type, AppIdType{"AppId_1"}, shm_data_chunk_list_, trace_context_id_);
    ASSERT_TRUE(allocator_allocate_result.has_value());

    // Verify container and check status of trace job
    auto container_element = container_->GetReadyElement();
    VerifyContainer(container_element.value().get().data_);
    ASSERT_EQ(container_element.value().get().data_.original_trace_context_id_.context_id_, trace_context_id_);
    ASSERT_EQ(container_element.value().get().data_.job_type_, TraceJobType::kShmJob);

    // Verify chunk list, set trace job done and mark element in ring as empty.
    auto daemon_ring_result = daemon_ring_buffer_raw2_->GetReadyElement();
    ASSERT_TRUE(daemon_ring_result.has_value());
    VerifyChunkList(daemon_ring_result.value().get().chunk_list_);
    daemon_ring_result.value().get().global_context_id_ = {0, 0};
    daemon_ring_result.value().get().status_ = TraceJobStatus::kEmpty;

    // Deallocate shared memory
    ShmChunkVector* vector =
        GetPointerFromLocation<ShmChunkVector>(container_element.value().get().data_.chunk_list_, memory_);
    for (std::size_t i = 2; i < vector->size(); i++)
    {
        void* data_ptr = GetPointerFromLocation<void>(vector->at(i).value().start_, memory_);
        memory_->getMemoryResourceProxy()->deallocate(data_ptr, vector->at(i).value().size_);
    }

    // Call deallocator for job
    auto deallocator_result = allocator_logger_time_->DeallocateJob(container_element.value().get().data_.chunk_list_,
                                                                    container_element.value().get().data_.job_type_);
    ASSERT_TRUE(deallocator_result.has_value());
}

TEST_F(TraceJobAllocatorFixture, AllocateShmJobEmptyMetaInfo)
{
    ShmRingBufferElement shm_element;
    std::reference_wrapper<ShmRingBufferElement> shm_element_ref = shm_element;

    // The one call for GetEmptyElement will return reference to a new element
    ON_CALL(*daemon_ring_buffer_raw2_, GetEmptyElement()).WillByDefault(Return(shm_element_ref));

    // The one call for GetReadyElement will return reference to the same element returned by GetEmptyElement
    ON_CALL(*daemon_ring_buffer_raw2_, GetReadyElement()).WillByDefault(Return(shm_element_ref));

    MetaInfoVariants::type meta_info = DltMetaInfo(DltProperties());

    // Allocate trace job
    auto allocator_allocate_result = allocator_logger_time_->AllocateShmJob(
        trace_client_id_, meta_info, binding_type, AppIdType{"AppId_1"}, shm_data_chunk_list_, trace_context_id_);
    ASSERT_FALSE(allocator_allocate_result.has_value());
    ASSERT_EQ(allocator_allocate_result.error(), ErrorCode::kNoMetaInfoProvidedRecoverable);
}

TEST_F(TraceJobAllocatorFixture, TestEmptyTraceJobAllocatorConstructor)
{
    TraceJobAllocator traceJobAllocator;
    EXPECT_FALSE(traceJobAllocator.IsLoggerTimeAvailable());
}

TEST_F(TraceJobAllocatorFixture, AllocateShmJobFailedRingBufferNotInitialized)
{
    EXPECT_CALL(*daemon_ring_buffer_raw_, GetEmptyElement)
        .WillOnce(Return(score::MakeUnexpected(ErrorCode::kRingBufferNotInitializedRecoverable)));
    // Allocate trace job
    auto allocator_allocate_result = allocator_->AllocateShmJob(
        trace_client_id_, meta_info_, binding_type, AppIdType{"AppId_1"}, shm_data_chunk_list_, trace_context_id_);
    ASSERT_FALSE(allocator_allocate_result.has_value());
    ASSERT_EQ(allocator_allocate_result.error(), ErrorCode::kRingBufferNotInitializedRecoverable);
}

TEST_F(TraceJobAllocatorFixture, AllocateShmJobFailedAllocator)
{
    std::shared_ptr<FlexibleCircularAllocatorMock> flexible_allocator_mock =
        std::make_shared<FlexibleCircularAllocatorMock>();

    ShmRingBufferElement shm_element;
    std::reference_wrapper<ShmRingBufferElement> shm_element_ref = shm_element;
    std::unique_ptr<MockShmRingBuffer> daemon_ring_buffer_1_ = std::make_unique<MockShmRingBuffer>();
    auto daemon_ring_buffer_raw_1_ = daemon_ring_buffer_1_.get();

    std::unique_ptr<TraceJobAllocator> allocator_1_ = std::make_unique<TraceJobAllocator>(
        container_, memory_, flexible_allocator_mock, std::move(daemon_ring_buffer_1_));
    allocator_1_->SetTraceMetaDataShmObjectHandle(memory_handle_);
    // The one call for GetEmptyElement will return reference to a new element
    ON_CALL(*daemon_ring_buffer_raw_1_, GetEmptyElement()).WillByDefault(Return(shm_element_ref));
    EXPECT_CALL(*flexible_allocator_mock, Allocate).WillOnce(Return(nullptr));
    // Allocate trace job
    auto allocator_allocate_result = allocator_1_->AllocateShmJob(
        trace_client_id_, meta_info_, binding_type, AppIdType{"AppId_1"}, shm_data_chunk_list_, trace_context_id_);
    ASSERT_FALSE(allocator_allocate_result.has_value());
    ASSERT_EQ(allocator_allocate_result.error(), ErrorCode::kNotEnoughMemoryRecoverable);
}

TEST_F(TraceJobAllocatorFixture, AllocateShmJobFailedMemoryResource)
{
    ShmRingBufferElement shm_element;
    std::reference_wrapper<ShmRingBufferElement> shm_element_ref = shm_element;
    std::unique_ptr<MockShmRingBuffer> daemon_ring_buffer_custom_ = std::make_unique<MockShmRingBuffer>();
    auto daemon_ring_buffer_raw_custom_ = daemon_ring_buffer_custom_.get();

    // passing memory resource as a nullptr
    std::unique_ptr<TraceJobAllocator> allocator_1_ = std::make_unique<TraceJobAllocator>(
        container_, nullptr, flexible_allocator_, std::move(daemon_ring_buffer_custom_));
    allocator_1_->SetTraceMetaDataShmObjectHandle(memory_handle_);
    ON_CALL(*daemon_ring_buffer_raw_custom_, GetEmptyElement()).WillByDefault(Return(shm_element_ref));

    // Allocate trace job
    auto allocator_allocate_result = allocator_1_->AllocateShmJob(
        trace_client_id_, meta_info_, binding_type, AppIdType{"AppId_1"}, shm_data_chunk_list_, trace_context_id_);
    ASSERT_FALSE(allocator_allocate_result.has_value());
    ASSERT_EQ(allocator_allocate_result.error(), ErrorCode::kNotEnoughMemoryRecoverable);
}

TEST_F(TraceJobAllocatorFixture, AllocateShmJobFailedSecondAllocator)
{
    std::shared_ptr<FlexibleCircularAllocatorMock> flexible_allocator_mock =
        std::make_shared<FlexibleCircularAllocatorMock>();

    ShmRingBufferElement shm_element;
    std::reference_wrapper<ShmRingBufferElement> shm_element_ref = shm_element;
    std::unique_ptr<MockShmRingBuffer> daemon_ring_buffer_custom_ = std::make_unique<MockShmRingBuffer>();
    auto daemon_ring_buffer_raw_custom_ = daemon_ring_buffer_custom_.get();

    // passing memory resource as a nullptr
    std::unique_ptr<TraceJobAllocator> allocator_custom_ = std::make_unique<TraceJobAllocator>(
        container_, memory_, flexible_allocator_mock, std::move(daemon_ring_buffer_custom_));
    allocator_custom_->SetTraceMetaDataShmObjectHandle(memory_handle_);
    EXPECT_CALL(*daemon_ring_buffer_raw_custom_, GetEmptyElement()).WillOnce(Return(shm_element_ref));

    EXPECT_CALL(*flexible_allocator_mock, Allocate)
        .WillOnce([&](const std::size_t size, const std::size_t count) {
            return flexible_allocator_->Allocate(size, count);
        })
        .WillOnce(Return(nullptr));

    EXPECT_CALL(*flexible_allocator_mock, Deallocate).WillRepeatedly([&](void* const addr, const std::size_t count) {
        return flexible_allocator_->Deallocate(addr, count);
    });

    // Allocate trace job
    auto allocator_allocate_result = allocator_custom_->AllocateShmJob(
        trace_client_id_, meta_info_, binding_type, AppIdType{"AppId_1"}, shm_data_chunk_list_, trace_context_id_);
    ASSERT_FALSE(allocator_allocate_result.has_value());
    ASSERT_EQ(allocator_allocate_result.error(), ErrorCode::kNotEnoughMemoryRecoverable);
}

TEST_F(TraceJobAllocatorFixture, AllocateShmJobFailedSaveMemory)
{
    ShmRingBufferElement shm_element;
    std::reference_wrapper<ShmRingBufferElement> shm_element_ref = shm_element;
    std::unique_ptr<MockShmRingBuffer> daemon_ring_buffer_custom_ = std::make_unique<MockShmRingBuffer>();
    auto daemon_ring_buffer_raw_custom_ = daemon_ring_buffer_custom_.get();

    // passing memory handle as a -1
    std::unique_ptr<TraceJobAllocator> allocator_custom_ = std::make_unique<TraceJobAllocator>(
        container_, memory_, flexible_allocator_, std::move(daemon_ring_buffer_custom_));
    allocator_custom_->SetTraceMetaDataShmObjectHandle(-1);
    EXPECT_CALL(*daemon_ring_buffer_raw_custom_, GetEmptyElement()).WillOnce(Return(shm_element_ref));

    // Allocate trace job
    auto allocator_allocate_result = allocator_custom_->AllocateShmJob(
        trace_client_id_, meta_info_, binding_type, AppIdType{"AppId_1"}, shm_data_chunk_list_, trace_context_id_);
    ASSERT_FALSE(allocator_allocate_result.has_value());
    ASSERT_EQ(allocator_allocate_result.error(), ErrorCode::kInvalidArgumentFatal);
}

TEST_F(TraceJobAllocatorFixture, AllocateLocalJobFailedEmptyElements)
{
    // The one call for GetEmptyElement will return reference to a new element
    ON_CALL(*daemon_ring_buffer_raw_, GetEmptyElement())
        .WillByDefault(Return(score::MakeUnexpected(ErrorCode::kRingBufferNotInitializedRecoverable)));

    // Allocate trace job
    auto allocator_allocate_result = allocator_->AllocateLocalJob(
        trace_client_id_, meta_info_, binding_type, AppIdType{"AppId_1"}, local_data_chunk_list_);
    ASSERT_FALSE(allocator_allocate_result.has_value());
    ASSERT_EQ(allocator_allocate_result.error(), ErrorCode::kRingBufferNotInitializedRecoverable);
}

TEST_F(TraceJobAllocatorFixture, AllocateLocalJobFailedtoGetOffsetFromPointer)
{
    ShmRingBufferElement shm_element;
    std::reference_wrapper<ShmRingBufferElement> shm_element_ref = shm_element;

    EXPECT_CALL(*memory_mock_, getBaseAddress)
        .WillOnce(Return(&shm_element))
        .WillOnce(Return(&shm_element))
        .WillOnce(Return(nullptr));

    std::unique_ptr<MockShmRingBuffer> daemon_ring_buffer_custom_ = std::make_unique<MockShmRingBuffer>();
    auto daemon_ring_buffer_raw_custom_ = daemon_ring_buffer_custom_.get();
    // The one call for GetEmptyElement will return reference to a new element
    ON_CALL(*daemon_ring_buffer_raw_custom_, GetEmptyElement()).WillByDefault(Return(shm_element_ref));

    // passing memory resource as a nullptr
    std::unique_ptr<TraceJobAllocator> allocator_custom_ = std::make_unique<TraceJobAllocator>(
        container_, memory_mock_, flexible_allocator_, std::move(daemon_ring_buffer_custom_));
    allocator_custom_->SetTraceMetaDataShmObjectHandle(memory_handle_);
    // Allocate trace job
    auto allocator_allocate_result = allocator_custom_->AllocateShmJob(
        trace_client_id_, meta_info_, binding_type, AppIdType{"AppID"}, shm_data_chunk_list_, trace_context_id_);
    ASSERT_FALSE(allocator_allocate_result.has_value());
    ASSERT_EQ(allocator_allocate_result.error(), ErrorCode::kNotEnoughMemoryRecoverable);
}

TEST_F(TraceJobAllocatorFixture, AllocateLocalJobFailedSaveToSharedMemory)
{

    ShmRingBufferElement shm_element;
    std::reference_wrapper<ShmRingBufferElement> shm_element_ref = shm_element;

    std::unique_ptr<MockShmRingBuffer> daemon_ring_buffer_custom_ = std::make_unique<MockShmRingBuffer>();
    auto daemon_ring_buffer_raw_custom_ = daemon_ring_buffer_custom_.get();
    // The one call for GetEmptyElement will return reference to a new element
    ON_CALL(*daemon_ring_buffer_raw_custom_, GetEmptyElement()).WillByDefault(Return(shm_element_ref));

    // passing memory resource as a nullptr
    std::unique_ptr<TraceJobAllocator> allocator_custom_ = std::make_unique<TraceJobAllocator>(
        container_, nullptr, flexible_allocator_, std::move(daemon_ring_buffer_custom_));
    allocator_custom_->SetTraceMetaDataShmObjectHandle(memory_handle_);
    // Allocate trace job
    auto allocator_allocate_result = allocator_custom_->AllocateLocalJob(
        trace_client_id_, meta_info_, binding_type, AppIdType{"AppId_1"}, local_data_chunk_list_);
    ASSERT_FALSE(allocator_allocate_result.has_value());
    ASSERT_EQ(allocator_allocate_result.error(), ErrorCode::kInvalidArgumentFatal);
}

TEST_F(TraceJobAllocatorFixture, DeallocateInvalidHandler)
{
    ShmRingBufferElement element;
    std::reference_wrapper<ShmRingBufferElement> shm_element_ref = element;

    // The one call for GetEmptyElement will return reference to a new element
    ON_CALL(*daemon_ring_buffer_raw_, GetEmptyElement()).WillByDefault(Return(shm_element_ref));

    // The one call for GetReadyElement will return reference to the same element returned by GetEmptyElement
    ON_CALL(*daemon_ring_buffer_raw_, GetReadyElement()).WillByDefault(Return(shm_element_ref));

    // Allocate trace job
    auto allocator_allocate_result = allocator_->AllocateLocalJob(
        trace_client_id_, meta_info_, binding_type, AppIdType{"AppId_1"}, local_data_chunk_list_);
    ASSERT_TRUE(allocator_allocate_result.has_value());

    SharedMemoryLocation wrong_shared_meomry_location;
    wrong_shared_meomry_location.shm_object_handle_ = 0;

    const auto container_element = container_->GetReadyElement();
    // Testing the equal operator
    const auto first_element = container_->GetReadyElement();
    const auto second_element = container_->GetReadyElement();
    ASSERT_EQ(first_element.value().get().data_, second_element.value().get().data_);

    // Verify chunk list and set trace job done
    auto daemon_ring_result = daemon_ring_buffer_raw_->GetReadyElement();
    ASSERT_TRUE(daemon_ring_result.has_value());
    daemon_ring_result.value().get().global_context_id_ = {0, 0};
    daemon_ring_result.value().get().status_ = TraceJobStatus::kEmpty;

    auto deallocator_result =
        allocator_->DeallocateJob(wrong_shared_meomry_location, container_element.value().get().data_.job_type_);

    ASSERT_FALSE(deallocator_result.has_value());
    ASSERT_EQ(deallocator_result.error(), ErrorCode::kWrongHandleRecoverable);

    deallocator_result = allocator_->DeallocateJob(container_element.value().get().data_.chunk_list_,
                                                   container_element.value().get().data_.job_type_);
    ASSERT_TRUE(deallocator_result.has_value());
}

TEST_F(TraceJobAllocatorFixture, DeallocateSuccessfulTest)
{
    ShmRingBufferElement element;
    std::reference_wrapper<ShmRingBufferElement> shm_element_ref = element;

    // The one call for GetEmptyElement will return reference to a new element
    ON_CALL(*daemon_ring_buffer_raw_, GetEmptyElement()).WillByDefault(Return(shm_element_ref));

    // The one call for GetReadyElement will return reference to the same element returned by GetEmptyElement
    ON_CALL(*daemon_ring_buffer_raw_, GetReadyElement()).WillByDefault(Return(shm_element_ref));

    // Allocate trace job
    auto allocator_allocate_result = allocator_->AllocateLocalJob(
        trace_client_id_, meta_info_, binding_type, AppIdType{"AppId_1"}, local_data_chunk_list_);
    ASSERT_TRUE(allocator_allocate_result.has_value());

    const auto container_element = container_->GetReadyElement();

    auto deallocator_result = allocator_->DeallocateJob(container_element.value().get().data_.chunk_list_,
                                                        container_element.value().get().data_.job_type_);
    ASSERT_TRUE(deallocator_result.has_value());
}

TEST_F(TraceJobAllocatorFixture, DeallocateFailedGetPointer)
{
    ShmRingBufferElement shm_element;
    std::reference_wrapper<ShmRingBufferElement> shm_element_ref = shm_element;

    std::unique_ptr<MockShmRingBuffer> daemon_ring_buffer_custom_ = std::make_unique<MockShmRingBuffer>();
    auto daemon_ring_buffer_raw_custom_ = daemon_ring_buffer_custom_.get();
    // The one call for GetEmptyElement will return reference to a new element
    ON_CALL(*daemon_ring_buffer_raw_custom_, GetEmptyElement()).WillByDefault(Return(shm_element_ref));

    // passing memory resource as a nullptr
    std::unique_ptr<TraceJobAllocator> allocator_custom_ = std::make_unique<TraceJobAllocator>(
        container_, nullptr, flexible_allocator_, std::move(daemon_ring_buffer_custom_));
    allocator_custom_->SetTraceMetaDataShmObjectHandle(memory_handle_);
    SharedMemoryLocation temp;
    temp.shm_object_handle_ = 1;

    TraceJobType tempJob = TraceJobType::kLocalJob;

    auto deallocator_result = allocator_custom_->DeallocateJob(temp, tempJob);
    ASSERT_FALSE(deallocator_result.has_value());
    ASSERT_EQ(deallocator_result.error(), ErrorCode::kInvalidArgumentFatal);
}
