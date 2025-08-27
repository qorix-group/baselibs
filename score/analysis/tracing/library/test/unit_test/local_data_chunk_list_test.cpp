///
/// @file local_data_chunk_list_test.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API local_data_chunk_list test source
///

#include "score/analysis/tracing/library/generic_trace_api/chunk_list/local_data_chunk_list.h"
#include "score/analysis/tracing/common/flexible_circular_allocator/lockless_flexible_circular_allocator.h"
#include "score/analysis/tracing/common/flexible_circular_allocator/test/mocks/flexible_circular_allocator_mock.h"
#include "score/analysis/tracing/common/interface_types/shared_memory_location_helpers.h"
#include "score/analysis/tracing/library/generic_trace_api/chunk_list/shm_data_chunk_list.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"
#include "score/analysis/tracing/library/test/unit_test/local_memory_resource.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>

using namespace score::memory::shared;
using namespace score::analysis::tracing;
using ::testing::_;

static constexpr std::size_t kCircularAllocatorSize = 5000;

class LocalDataChunkListFixture : public ::testing::Test
{
  public:
    static constexpr std::size_t kAllocatedMemorySize = 10000;
    static constexpr std::size_t number_of_elements = 10;
    static constexpr std::size_t exceeding_number_of_elements = 12;
    static constexpr ShmObjectHandle handle = 1;
    static constexpr std::size_t total_size = 186u;

    std::uint8_t* memory_block_ptr;
    std::shared_ptr<LocalMemoryResource> memory_;

    LocalDataChunkList local_data_chunk_list_{};
    LocalDataChunk local_chunk_{};
    std::shared_ptr<IFlexibleCircularAllocator> flexible_allocator_;
    std::shared_ptr<FlexibleCircularAllocatorMock> flexible_allocator_mock_;

    void PrepareChunkList(std::uint8_t** data_start)
    {
        *data_start = static_cast<std::uint8_t*>(malloc(total_size));
        std::uint8_t* pointer = *data_start;
        ASSERT_NE(nullptr, *data_start);

        for (std::uint8_t i = 0; i < number_of_elements; i++)
        {
            std::size_t size = i + 10u;
            memset(pointer, i, size);
            local_chunk_.size_ = size;
            local_chunk_.start_ = pointer;
            local_data_chunk_list_.Append(local_chunk_);
            pointer += size;
        }
        return;
    }

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

constexpr std::size_t LocalDataChunkListFixture::number_of_elements;
constexpr ShmObjectHandle LocalDataChunkListFixture::handle;
constexpr std::size_t LocalDataChunkListFixture::total_size;

TEST_F(LocalDataChunkListFixture, EmptyTest)
{
    ASSERT_EQ(local_data_chunk_list_.Size(), 0);
}

TEST_F(LocalDataChunkListFixture, ConstructListWithRootChunk)
{
    LocalDataChunkList root_chunk_list{LocalDataChunk{}};
    ASSERT_EQ(root_chunk_list.Size(), 1);
}

TEST_F(LocalDataChunkListFixture, DefaultConstructorTest)
{
    LocalDataChunkList default_chunk_list{};
    ASSERT_EQ(default_chunk_list.Size(), 0);
}

TEST_F(LocalDataChunkListFixture, GetList)
{
    auto list = local_data_chunk_list_.GetList();
    ASSERT_EQ(list.size(), kMaxChunksPerOneTraceRequest);
}

TEST_F(LocalDataChunkListFixture, ConstGetList)
{
    const LocalDataChunkList const_local_data_chunk_list_{};

    auto list = const_local_data_chunk_list_.GetList();
    ASSERT_EQ(list.size(), kMaxChunksPerOneTraceRequest);
}

TEST_F(LocalDataChunkListFixture, AppendTest)
{
    for (std::size_t i = 0; i < number_of_elements; i++)
    {
        ASSERT_EQ(local_data_chunk_list_.Size(), i);
        local_data_chunk_list_.Append(local_chunk_);
    }

    ASSERT_EQ(local_data_chunk_list_.Size(), number_of_elements);
}

TEST_F(LocalDataChunkListFixture, ClearTest)
{
    for (std::size_t i = 0; i < number_of_elements; i++)
    {
        ASSERT_EQ(local_data_chunk_list_.Size(), i);
        local_data_chunk_list_.Append(local_chunk_);
    }

    ASSERT_EQ(local_data_chunk_list_.Size(), number_of_elements);
    local_data_chunk_list_.Clear();
    ASSERT_EQ(local_data_chunk_list_.Size(), 0);
}

TEST_F(LocalDataChunkListFixture, ChunkListFailingEqualityTestEmptyList)
{
    LocalDataChunkList second_local_data_chunk_list{};

    ASSERT_TRUE(local_data_chunk_list_ == second_local_data_chunk_list);
}

TEST_F(LocalDataChunkListFixture, ChunkListFailingEqualityTestSizeMismatch)
{
    for (std::size_t i = 0; i < number_of_elements; i++)
    {
        ASSERT_EQ(local_data_chunk_list_.Size(), i);
        local_data_chunk_list_.Append(local_chunk_);
    }
    LocalDataChunkList second_local_data_chunk_list{};

    ASSERT_FALSE(local_data_chunk_list_ == second_local_data_chunk_list);
}

TEST_F(LocalDataChunkListFixture, AppendFrontLocalChunkListExceedingTheLimit)
{
    for (std::size_t i = 0; i < number_of_elements + 1; i++)
    {
        local_data_chunk_list_.AppendFront(local_chunk_);
    }

    ASSERT_EQ(local_data_chunk_list_.Size(), number_of_elements);
}

TEST_F(LocalDataChunkListFixture, ChunkListSuccessfulEqualityTest)
{
    LocalDataChunkList second_local_data_chunk_list{};
    for (std::size_t i = 0; i < number_of_elements; i++)
    {
        local_data_chunk_list_.Append(local_chunk_);
        second_local_data_chunk_list.Append(local_chunk_);
    }

    ASSERT_EQ(local_data_chunk_list_, second_local_data_chunk_list);
}

TEST_F(LocalDataChunkListFixture, ChunkListFailingEqualityTestElementMismatch)
{
    LocalDataChunkList second_local_data_chunk_list{};
    for (std::size_t i = 0; i < number_of_elements; i++)
    {
        local_data_chunk_list_.Append(local_chunk_);
        if (i == number_of_elements - 1)
        {
            break;
        }
        second_local_data_chunk_list.Append(local_chunk_);
    }
    LocalDataChunk local_chunk_temp{};
    local_chunk_temp.size_ = 123;

    second_local_data_chunk_list.Append(local_chunk_temp);

    ASSERT_FALSE(local_data_chunk_list_ == second_local_data_chunk_list);
}

TEST_F(LocalDataChunkListFixture, SaveVectorTest)
{
    // Prepare contents of chunk list
    std::uint8_t* data;
    PrepareChunkList(&data);

    // Save chunk list to vector placed in LocalMemoryResource
    auto result = local_data_chunk_list_.SaveToSharedMemory(memory_, handle, flexible_allocator_);
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value().shm_object_handle_, handle);

    free(data);
    // Read vector from LocalMemoryResource and verify contents
    ShmChunkVector* vector = GetPointerFromLocation<ShmChunkVector>(result.value(), memory_);
    ASSERT_EQ(vector->size(), number_of_elements);

    std::size_t i = 0;
    for (auto el : *vector)
    {
        std::uint8_t* data_start = GetPointerFromLocation<std::uint8_t>(el.start_, memory_);
        ASSERT_EQ(el.size_, i + 10);
        ASSERT_EQ(el.start_.shm_object_handle_, handle);
        std::uint8_t* data_ptr = data_start;
        for (std::size_t j = 0; j < i + 10; j++)
        {
            ASSERT_EQ(*data_ptr, i);
            data_ptr++;
        }
        flexible_allocator_->Deallocate(data_start, el.size_);
        i++;
    }

    // Cleanup
    vector->~List();
    flexible_allocator_->Deallocate(vector, sizeof(ShmChunkVector));
}

TEST_F(LocalDataChunkListFixture, SaveListInMemoryWithAllInvalidElements)
{
    std::size_t memory_before = flexible_allocator_->GetAvailableMemory();

    LocalDataChunk invalid_chunk{};
    invalid_chunk.size_ = 0U;
    invalid_chunk.start_ = nullptr;

    for (std::size_t i = 0; i < number_of_elements; i++)
    {
        local_data_chunk_list_.Append(invalid_chunk);
    }

    ASSERT_EQ(memory_before, flexible_allocator_->GetAvailableMemory());
    auto result = local_data_chunk_list_.SaveToSharedMemory(memory_, handle, flexible_allocator_);
    ASSERT_NE(memory_before, flexible_allocator_->GetAvailableMemory());

    ShmChunkVector* vector = GetPointerFromLocation<ShmChunkVector>(result.value(), memory_);
    ASSERT_EQ(vector->size(), 0U);

    vector->~List();
    flexible_allocator_->Deallocate(vector, sizeof(ShmChunkVector));
    ASSERT_EQ(memory_before, flexible_allocator_->GetAvailableMemory());
}

TEST_F(LocalDataChunkListFixture, SaveVectorMemoryTest)
{
    std::size_t memory_before = flexible_allocator_->GetAvailableMemory();

    std::uint8_t* data;
    PrepareChunkList(&data);

    ASSERT_EQ(memory_before, flexible_allocator_->GetAvailableMemory());
    auto result = local_data_chunk_list_.SaveToSharedMemory(memory_, handle, flexible_allocator_);
    ASSERT_NE(memory_before, flexible_allocator_->GetAvailableMemory());

    free(data);
    ShmChunkVector* vector = GetPointerFromLocation<ShmChunkVector>(result.value(), memory_);

    for (auto el : *vector)
    {
        std::uint8_t* data_ptr = GetPointerFromLocation<std::uint8_t>(el.start_, memory_);
        flexible_allocator_->Deallocate(data_ptr, el.size_);
    }

    vector->~List();
    flexible_allocator_->Deallocate(vector, sizeof(ShmChunkVector));
    ASSERT_EQ(memory_before, flexible_allocator_->GetAvailableMemory());
}

TEST_F(LocalDataChunkListFixture, SaveVectorNullMemoryTest)
{
    std::size_t memory_before = flexible_allocator_->GetAvailableMemory();

    for (std::uint8_t i = 0; i < number_of_elements; i++)
    {
        local_data_chunk_list_.Append(local_chunk_);
    }

    ASSERT_EQ(memory_before, flexible_allocator_->GetAvailableMemory());
    auto result = local_data_chunk_list_.SaveToSharedMemory(nullptr, handle, flexible_allocator_);
    ASSERT_EQ(memory_before, flexible_allocator_->GetAvailableMemory());
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), ErrorCode::kInvalidArgumentFatal);
}

TEST_F(LocalDataChunkListFixture, SaveVectorFailedToAllocateMemoryTest)
{
    // reduce the available memory to make sure that no more space are available to allocate
    auto reserved_buffer = flexible_allocator_->Allocate(4200U);
    std::size_t memory_before = flexible_allocator_->GetAvailableMemory();

    for (std::size_t i = 0; i < exceeding_number_of_elements; i++)
    {
        local_data_chunk_list_.Append(local_chunk_);
    }

    memory_->is_allocation_possible_ = false;
    ASSERT_EQ(memory_before, flexible_allocator_->GetAvailableMemory());
    auto result = local_data_chunk_list_.SaveToSharedMemory(memory_, handle, flexible_allocator_);
    ASSERT_EQ(memory_before, flexible_allocator_->GetAvailableMemory());
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), ErrorCode::kNotEnoughMemoryRecoverable);

    score::cpp::ignore = flexible_allocator_->Deallocate(reserved_buffer, 4200U);
}

TEST_F(LocalDataChunkListFixture, SaveVectorFailedToAllocateAnyElements)
{
    std::uint8_t* data;
    PrepareChunkList(&data);
    // GetAvailableMemory return a big number to make sure it fits
    ON_CALL(*flexible_allocator_mock_, GetAvailableMemory).WillByDefault(testing::Return(2e8));
    ON_CALL(*flexible_allocator_mock_, Allocate).WillByDefault(testing::Return(nullptr));

    auto result = local_data_chunk_list_.SaveToSharedMemory(memory_, handle, flexible_allocator_mock_);
    free(data);
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), ErrorCode::kNotEnoughMemoryRecoverable);
}

TEST_F(LocalDataChunkListFixture, HasEnoughMemoryWithNullAllocator)
{
    auto result = local_data_chunk_list_.HasEnoughMemory(nullptr, 1);
    EXPECT_FALSE(result);
}

TEST_F(LocalDataChunkListFixture, SaveVectorFailedToAllocateSomeElements)
{
    std::uint8_t* data;
    PrepareChunkList(&data);
    // adding an empty element before the end
    local_chunk_.size_ = 0;
    local_chunk_.start_ = nullptr;
    local_data_chunk_list_.Append(local_chunk_);

    // adding an extra element at the end
    local_chunk_.size_ = 22;
    local_chunk_.start_ = data;
    local_data_chunk_list_.Append(local_chunk_);

    // GetAvailableMemory return a big number to make sure it fits
    ON_CALL(*flexible_allocator_mock_, GetAvailableMemory).WillByDefault(testing::Return(2e8));

    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _))
        .Times(3 + number_of_elements)
        .WillRepeatedly([this](const std::size_t a, const std::size_t b) {
            static uint8_t i = 0;
            ++i;
            if (i < 13)
            {
                return flexible_allocator_->Allocate(a, b);
            }
            else
            {
                return static_cast<void*>(nullptr);
            }
        });

    auto result = local_data_chunk_list_.SaveToSharedMemory(memory_, handle, flexible_allocator_mock_);
    free(data);
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), ErrorCode::kNotEnoughMemoryRecoverable);
}

TEST_F(LocalDataChunkListFixture, SaveVectorFailedToFillVectorInSharedMemory)
{
    std::uint8_t allocation_count = 0U;
    std::uint8_t* data;
    PrepareChunkList(&data);
    // GetAvailableMemory return a big number to make sure it fits
    ON_CALL(*flexible_allocator_mock_, GetAvailableMemory).WillByDefault(testing::Return(2e8));

    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _))
        .WillRepeatedly([&allocation_count](const std::size_t size, const std::size_t) {
            if (allocation_count != 1)
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

    auto result = local_data_chunk_list_.SaveToSharedMemory(memory_, handle, flexible_allocator_mock_);
    free(data);
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), ErrorCode::kNotEnoughMemoryRecoverable);
}

TEST_F(LocalDataChunkListFixture, SaveVectorWithOneInvalidElementTest)
{
    std::size_t memory_before = flexible_allocator_->GetAvailableMemory();

    std::uint8_t* data;
    PrepareChunkList(&data);
    LocalDataChunk local_chunk{};
    local_chunk.size_ = 32U;
    local_chunk.start_ = static_cast<void*>(nullptr);
    local_data_chunk_list_.Append(local_chunk);

    ASSERT_EQ(memory_before, flexible_allocator_->GetAvailableMemory());
    auto result = local_data_chunk_list_.SaveToSharedMemory(memory_, handle, flexible_allocator_);
    ASSERT_NE(memory_before, flexible_allocator_->GetAvailableMemory());

    free(data);
    ShmChunkVector* vector = GetPointerFromLocation<ShmChunkVector>(result.value(), memory_);

    for (auto el : *vector)
    {
        std::uint8_t* data_ptr = GetPointerFromLocation<std::uint8_t>(el.start_, memory_);
        flexible_allocator_->Deallocate(data_ptr, el.size_);
    }

    vector->~List();
    flexible_allocator_->Deallocate(vector, sizeof(ShmChunkVector));
    ASSERT_EQ(memory_before, flexible_allocator_->GetAvailableMemory());
}
