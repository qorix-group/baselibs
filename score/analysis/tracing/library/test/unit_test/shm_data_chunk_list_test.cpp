///
/// @file shm_data_chunk_list_test.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API shm_data_chunk_list test source
///

#include "score/analysis/tracing/library/generic_trace_api/chunk_list/shm_data_chunk_list.h"
#include "score/analysis/tracing/common/flexible_circular_allocator/lockless_flexible_circular_allocator.h"
#include "score/analysis/tracing/common/flexible_circular_allocator/test/mocks/flexible_circular_allocator_mock.h"
#include "score/analysis/tracing/common/interface_types/shared_memory_location_helpers.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"
#include "score/analysis/tracing/library/test/unit_test/local_memory_resource.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>

using namespace score::memory::shared;
using namespace score::analysis::tracing;
using ::testing::_;

static constexpr std::size_t kCircularAllocatorSize = 5000;

class ShmDataChunkListFixture : public ::testing::Test
{
  public:
    static constexpr std::size_t kAllocatedMemorySize = 10000;
    static constexpr std::size_t number_of_elements = 10;
    static constexpr std::size_t exceeding_number_of_elements = 12;
    static constexpr ShmObjectHandle handle = 1;

    std::uint8_t* memory_block_ptr;
    std::shared_ptr<LocalMemoryResource> memory_;
    ShmDataChunkList shm_data_chunk_list_{};
    SharedMemoryChunk shm_chunk_{};
    std::shared_ptr<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>> flexible_allocator_;

    std::shared_ptr<FlexibleCircularAllocatorMock> flexible_allocator_mock_;

  protected:
    void SetUp() override
    {
        memory_ = std::make_shared<LocalMemoryResource>();
        memory_block_ptr =
            static_cast<std::uint8_t*>(memory_->allocate(kAllocatedMemorySize, alignof(std::max_align_t)));
        flexible_allocator_ = std::make_shared<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>>(
            memory_block_ptr, kCircularAllocatorSize);
        flexible_allocator_mock_ = std::make_shared<FlexibleCircularAllocatorMock>();
    }

    void TearDown() override
    {
        memory_->deallocate(memory_block_ptr, kAllocatedMemorySize);
    }
};

constexpr std::size_t ShmDataChunkListFixture::number_of_elements;
constexpr ShmObjectHandle ShmDataChunkListFixture::handle;

TEST_F(ShmDataChunkListFixture, ShmDataChunkListEmpty)
{
    ASSERT_EQ(shm_data_chunk_list_.Size(), 0);
}

TEST_F(ShmDataChunkListFixture, ConstructListWithRootChunk)
{
    ShmDataChunkList root_chunk_list{SharedMemoryChunk{}};
    ASSERT_EQ(root_chunk_list.Size(), 1);
}

TEST_F(ShmDataChunkListFixture, DefaultConstructorTest)
{
    ShmDataChunkList default_chunk_list{};
    ASSERT_EQ(default_chunk_list.Size(), 0);
}

TEST_F(ShmDataChunkListFixture, GetList)
{
    auto list = shm_data_chunk_list_.GetList();
    ASSERT_EQ(list.size(), kMaxChunksPerOneTraceRequest);
}

TEST_F(ShmDataChunkListFixture, ConstGetList)
{
    const ShmDataChunkList const_shm_data_chunk_list_{};

    auto list = const_shm_data_chunk_list_.GetList();
    ASSERT_EQ(list.size(), kMaxChunksPerOneTraceRequest);
}

TEST_F(ShmDataChunkListFixture, AppendFrontShmChunkListExceedingTheLimit)
{
    for (std::size_t i = 0; i < number_of_elements + 1; i++)
    {
        shm_data_chunk_list_.AppendFront(shm_chunk_);
    }

    ASSERT_EQ(shm_data_chunk_list_.Size(), number_of_elements);
}

TEST_F(ShmDataChunkListFixture, ShmDataChunkEqualityTotalMismatch)
{
    for (std::size_t i = 0; i < number_of_elements; i++)
    {
        ASSERT_EQ(shm_data_chunk_list_.Size(), i);
        shm_data_chunk_list_.Append(shm_chunk_);
    }
    ShmDataChunkList shm_data_chunk_list_1{};

    ASSERT_FALSE(shm_data_chunk_list_ == shm_data_chunk_list_1);
}

TEST_F(ShmDataChunkListFixture, ShmDataChunkEqualityTestMatch)
{
    ShmDataChunkList shm_data_chunk_list_1{};
    for (std::size_t i = 0; i < number_of_elements; i++)
    {
        shm_data_chunk_list_.Append(shm_chunk_);
        shm_data_chunk_list_1.Append(shm_chunk_);
    }

    ASSERT_TRUE(shm_data_chunk_list_ == shm_data_chunk_list_1);
}

TEST_F(ShmDataChunkListFixture, EqualityTestMismatchOneElement)
{
    ShmDataChunkList shm_data_chunk_list_1{};
    for (std::size_t i = 0; i < number_of_elements; i++)
    {
        shm_data_chunk_list_.Append(shm_chunk_);
        if (i == number_of_elements - 1)
        {
            break;
        }
        shm_data_chunk_list_1.Append(shm_chunk_);
    }
    SharedMemoryChunk shm_chunk_1{};
    shm_chunk_1.size_ = 123;

    shm_data_chunk_list_1.Append(shm_chunk_1);

    ASSERT_FALSE(shm_data_chunk_list_ == shm_data_chunk_list_1);
}

TEST_F(ShmDataChunkListFixture, ShmDataChunkAppend)
{
    for (std::size_t i = 0; i < number_of_elements; i++)
    {
        ASSERT_EQ(shm_data_chunk_list_.Size(), i);
        shm_data_chunk_list_.Append(shm_chunk_);
    }

    ASSERT_EQ(shm_data_chunk_list_.Size(), number_of_elements);
}

TEST_F(ShmDataChunkListFixture, ShmDataChunkClear)
{
    for (std::size_t i = 0; i < number_of_elements; i++)
    {
        ASSERT_EQ(shm_data_chunk_list_.Size(), i);
        shm_data_chunk_list_.Append(shm_chunk_);
    }

    ASSERT_EQ(shm_data_chunk_list_.Size(), number_of_elements);
    shm_data_chunk_list_.Clear();
    ASSERT_EQ(shm_data_chunk_list_.Size(), 0);
}

TEST_F(ShmDataChunkListFixture, ShmDataChunkSaveVectorTest)
{
    SharedMemoryLocation shm_offset_ptr{};

    for (std::size_t i = 0; i < number_of_elements; i++)
    {
        shm_offset_ptr.offset_ = i;
        shm_offset_ptr.shm_object_handle_ = static_cast<ShmObjectHandle>(i);
        shm_chunk_.size_ = i;
        shm_chunk_.start_ = shm_offset_ptr;
        shm_data_chunk_list_.Append(shm_chunk_);
    }

    // Save chunk list to vector placed in LocalMemoryResource
    auto result = shm_data_chunk_list_.SaveToSharedMemory(memory_, handle, flexible_allocator_);
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value().shm_object_handle_, handle);

    // Read vector from LocalMemoryResource and verify contents
    ShmChunkVector* vector = GetPointerFromLocation<ShmChunkVector>(result.value(), memory_);
    ASSERT_EQ(vector->size(), number_of_elements);

    std::size_t i = 0;
    for (auto el : *vector)
    {
        ASSERT_EQ(el.start_.offset_, i);
        ASSERT_EQ(el.start_.shm_object_handle_, i);
        ASSERT_EQ(el.size_, i);
        i++;
    }

    // Cleanup
    vector->~List();
}

TEST_F(ShmDataChunkListFixture, ShmDataChunkSaveVectorMemoryTest)
{
    std::size_t memory_before = flexible_allocator_->GetAvailableMemory();

    for (std::size_t i = 0; i < number_of_elements; i++)
    {
        shm_data_chunk_list_.Append(shm_chunk_);
    }

    ASSERT_EQ(memory_before, flexible_allocator_->GetAvailableMemory());
    auto result = shm_data_chunk_list_.SaveToSharedMemory(memory_, handle, flexible_allocator_);
    ASSERT_NE(memory_before, flexible_allocator_->GetAvailableMemory());

    ShmChunkVector* vector = GetPointerFromLocation<ShmChunkVector>(result.value(), memory_);

    vector->~List();
    flexible_allocator_->Deallocate(vector, sizeof(ShmChunkVector));
    ASSERT_EQ(memory_before, flexible_allocator_->GetAvailableMemory());
}

TEST_F(ShmDataChunkListFixture, SaveVectorNullMemoryTest)
{
    std::size_t memory_before = flexible_allocator_->GetAvailableMemory();

    for (std::size_t i = 0; i < number_of_elements; i++)
    {
        shm_data_chunk_list_.Append(shm_chunk_);
    }

    ASSERT_EQ(memory_before, flexible_allocator_->GetAvailableMemory());
    auto result = shm_data_chunk_list_.SaveToSharedMemory(nullptr, handle, flexible_allocator_);
    ASSERT_EQ(memory_before, flexible_allocator_->GetAvailableMemory());
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), ErrorCode::kInvalidArgumentFatal);
}

TEST_F(ShmDataChunkListFixture, SaveVectorFailedToAllocateMemoryTest)
{
    // reduce the available memory to make sure that no more space are available to allocate
    auto reserved_buffer = flexible_allocator_->Allocate(4200U, alignof(std::max_align_t));
    std::size_t memory_before = flexible_allocator_->GetAvailableMemory();

    for (std::size_t i = 0; i < exceeding_number_of_elements; i++)
    {
        shm_data_chunk_list_.Append(shm_chunk_);
    }

    ASSERT_EQ(memory_before, flexible_allocator_->GetAvailableMemory());
    auto result = shm_data_chunk_list_.SaveToSharedMemory(memory_, handle, flexible_allocator_);
    ASSERT_EQ(memory_before, flexible_allocator_->GetAvailableMemory());
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), ErrorCode::kNotEnoughMemoryRecoverable);
    score::cpp::ignore = flexible_allocator_->Deallocate(reserved_buffer, 4200U);
}

TEST_F(ShmDataChunkListFixture, SaveVectorAllocateFailure)
{
    for (std::size_t i = 0; i < number_of_elements; i++)
    {
        shm_data_chunk_list_.Append(shm_chunk_);
    }
    // GetAvailableMemory return a big number to make sure it fits
    ON_CALL(*flexible_allocator_mock_, GetAvailableMemory).WillByDefault(testing::Return(2e8));
    ON_CALL(*flexible_allocator_mock_, Allocate).WillByDefault(testing::Return(nullptr));

    auto result = shm_data_chunk_list_.SaveToSharedMemory(memory_, handle, flexible_allocator_mock_);

    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), ErrorCode::kNotEnoughMemoryRecoverable);
}

TEST_F(ShmDataChunkListFixture, ShmDataChunkFailtoEmplaceElementInTheList)
{
    std::uint8_t allocation_count = 0U;

    for (std::size_t i = 0; i < number_of_elements; i++)
    {
        shm_data_chunk_list_.Append(shm_chunk_);
    }

    ON_CALL(*flexible_allocator_mock_, GetAvailableMemory).WillByDefault(testing::Return(2e8));

    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _))
        .WillRepeatedly([&allocation_count](const std::size_t size, const std::size_t) {
            if (allocation_count != number_of_elements - 1)
            {
                auto allocated_memory = malloc(size);
                allocation_count++;
                return allocated_memory;
            }
            else
            {
                return static_cast<void*>(nullptr);
            }
        });

    EXPECT_CALL(*flexible_allocator_mock_, Deallocate(_, _)).WillRepeatedly([](void* const address, const std::size_t) {
        if (address)
            free(address);  // Simulate deallocation
        return true;
    });

    auto result = shm_data_chunk_list_.SaveToSharedMemory(memory_, handle, flexible_allocator_mock_);

    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), ErrorCode::kNotEnoughMemoryRecoverable);
}
