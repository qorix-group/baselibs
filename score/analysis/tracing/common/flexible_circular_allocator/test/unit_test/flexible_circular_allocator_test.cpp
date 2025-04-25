/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
#include "score/analysis/tracing/common/flexible_circular_allocator/flexible_circular_allocator.h"
#include "score/analysis/tracing/common/flexible_circular_allocator/custom_polymorphic_offset_ptr_allocator.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>

using namespace score::analysis::tracing;
static constexpr std::size_t kFlexibleAllocatorSize = 1000;
static constexpr std::size_t kSingleElementSize = 100;

struct xy
{
    std::uint64_t x;
    float y;
};
using CustomVector = std::vector<xy, CustomPolymorphicOffsetPtrAllocator<xy>>;

class FlexibleCircularAllocatorFixture : public ::testing::Test
{
  public:
    std::unique_ptr<std::uint8_t[]> memory_pointer_;
    std::shared_ptr<FlexibleCircularAllocator> flexible_allocator_;

  protected:
    void SetUp() override
    {
        memory_pointer_ = std::make_unique<std::uint8_t[]>(2 * kFlexibleAllocatorSize);
        flexible_allocator_ =
            std::make_shared<FlexibleCircularAllocator>(memory_pointer_.get(), kFlexibleAllocatorSize);
    }

    bool VerifyAddressWithinAllocatedMemoryRange(void* address)
    {
        return ((address >= reinterpret_cast<void*>(memory_pointer_.get())) &&
                (address < reinterpret_cast<void*>(memory_pointer_.get() + kFlexibleAllocatorSize)));
    }
    void TearDown() override {}
};

TEST_F(FlexibleCircularAllocatorFixture, WhenAllocateForFirstTimeShallGetBaseAddress)
{
    std::uint8_t* allocated_address = static_cast<uint8_t*>(flexible_allocator_->Allocate(kSingleElementSize));
    EXPECT_EQ(allocated_address, memory_pointer_.get());
}

TEST_F(FlexibleCircularAllocatorFixture, AllocateFollowingDealloacteYieldsSameAddress)
{
    std::uint8_t* allocated_address = static_cast<uint8_t*>(flexible_allocator_->Allocate(kSingleElementSize));
    EXPECT_EQ(allocated_address, memory_pointer_.get());
    flexible_allocator_->Deallocate(allocated_address, kSingleElementSize);
    std::uint8_t* allocated_address2 = static_cast<uint8_t*>(flexible_allocator_->Allocate(kSingleElementSize));
    EXPECT_EQ(allocated_address2, memory_pointer_.get());
}

TEST_F(FlexibleCircularAllocatorFixture, CustomAllocatorVectorShallBeAllocatedInsideProvidedMemory)
{
    void* const vector_shm_raw_pointer =
        flexible_allocator_->Allocate(sizeof(std::vector<xy, CustomPolymorphicOffsetPtrAllocator<xy>>));

    CustomVector* const vector =
        new (vector_shm_raw_pointer) CustomVector(3, CustomPolymorphicOffsetPtrAllocator<xy>(flexible_allocator_));
    vector->at(0).x = 1;
    vector->at(0).y = 1.1f;
    EXPECT_EQ(vector->at(0).x, 1);
    EXPECT_EQ(vector->at(0).y, 1.1f);

    // check if the vector itself allocated in the defined range
    EXPECT_TRUE(VerifyAddressWithinAllocatedMemoryRange(vector));

    // check if that element 0 is allocated withing the memory range
    EXPECT_TRUE(VerifyAddressWithinAllocatedMemoryRange(&vector->at(0)));

    // check if that element 2 is allocated withing the memory range
    EXPECT_TRUE(VerifyAddressWithinAllocatedMemoryRange(&vector->at(2)));

    vector->~vector();
    flexible_allocator_->Deallocate(vector_shm_raw_pointer, sizeof(vector_shm_raw_pointer));
}

TEST_F(FlexibleCircularAllocatorFixture, AllocationShallBeIncrementingOrder)
{
    std::uint8_t* allocated_address = static_cast<uint8_t*>(flexible_allocator_->Allocate(kSingleElementSize));
    std::uint8_t* allocated_address2 = static_cast<uint8_t*>(flexible_allocator_->Allocate(kSingleElementSize));
    EXPECT_TRUE(allocated_address2 > allocated_address);

    EXPECT_EQ(allocated_address, memory_pointer_.get());
}

TEST_F(FlexibleCircularAllocatorFixture, AllocationShallBeDoneInAscendingOrderToAvoidHoles)
{
    std::uint8_t* allocated_address = static_cast<uint8_t*>(flexible_allocator_->Allocate(kSingleElementSize));
    std::uint8_t* allocated_address2 = static_cast<uint8_t*>(flexible_allocator_->Allocate(kSingleElementSize));
    std::uint8_t* allocated_address3 = static_cast<uint8_t*>(flexible_allocator_->Allocate(kSingleElementSize));
    flexible_allocator_->Deallocate(allocated_address2, sizeof(allocated_address2));
    std::uint8_t* allocated_address4 = static_cast<uint8_t*>(flexible_allocator_->Allocate(kSingleElementSize));

    EXPECT_TRUE(allocated_address4 > allocated_address);
    EXPECT_TRUE(allocated_address3 > allocated_address);

    EXPECT_EQ(allocated_address, memory_pointer_.get());
}

TEST_F(FlexibleCircularAllocatorFixture, FlexibleAllocatorShallReturnNullptrIfNoAlignedSpaceAvailable)
{
    std::ignore = static_cast<uint8_t*>(flexible_allocator_->Allocate(900));
    std::uint8_t* allocated_address2 = static_cast<uint8_t*>(flexible_allocator_->Allocate(kSingleElementSize));

    EXPECT_EQ(allocated_address2, nullptr);
}

TEST_F(FlexibleCircularAllocatorFixture, AllocatorShallWrapAroundWhenNoSpaceAvailableAtTheEnd)
{
    // consume most of the memory
    std::uint8_t* allocated_address = static_cast<uint8_t*>(flexible_allocator_->Allocate(320));
    std::ignore = static_cast<uint8_t*>(flexible_allocator_->Allocate(320));
    std::ignore = static_cast<uint8_t*>(flexible_allocator_->Allocate(320));
    // deallocate the first allocated part
    flexible_allocator_->Deallocate(allocated_address, sizeof(allocated_address));
    // should start over again
    std::uint8_t* allocated_address2 = static_cast<uint8_t*>(flexible_allocator_->Allocate(160));

    EXPECT_EQ(allocated_address2, memory_pointer_.get());
}

TEST_F(FlexibleCircularAllocatorFixture, WhenAllocateWithNoSplit)
{
    // consume most of the memory
    std::uint8_t* allocated_address = static_cast<uint8_t*>(flexible_allocator_->Allocate(320));
    std::ignore = static_cast<uint8_t*>(flexible_allocator_->Allocate(320));
    std::ignore = static_cast<uint8_t*>(flexible_allocator_->Allocate(320));
    // deallocate the first allocated part
    flexible_allocator_->Deallocate(allocated_address, sizeof(allocated_address));
    // should start over again
    std::uint8_t* second_allocated_address = static_cast<uint8_t*>(flexible_allocator_->Allocate(320));

    EXPECT_EQ(second_allocated_address, memory_pointer_.get());
}

TEST_F(FlexibleCircularAllocatorFixture, FlexibleAllocatorShallReturnNullptrIfRequestedMoreThanTotalSpace)
{
    auto allocated_address = static_cast<uint8_t*>(flexible_allocator_->Allocate(kFlexibleAllocatorSize));

    EXPECT_EQ(allocated_address, nullptr);
}

TEST_F(FlexibleCircularAllocatorFixture, FlexibleAllocatorShallReturnFalseIfRequestedToDeallocatePointerNotInItsList)
{
    int* ptr = new int;
    auto is_deallocated = flexible_allocator_->Deallocate(ptr, 1);

    EXPECT_FALSE(is_deallocated);
    delete ptr;
}

TEST_F(FlexibleCircularAllocatorFixture, TestFlexibleAllocatorGetters)
{
    EXPECT_EQ(memory_pointer_.get(), flexible_allocator_->GetBaseAddress());
    EXPECT_EQ(kFlexibleAllocatorSize, flexible_allocator_->GetSize());
    std::uint8_t* allocated_address = static_cast<uint8_t*>(flexible_allocator_->Allocate(kSingleElementSize));
    EXPECT_TRUE(flexible_allocator_->IsInBounds(allocated_address, kSingleElementSize));
    EXPECT_FALSE(flexible_allocator_->IsInBounds(nullptr, kSingleElementSize));
}

TEST_F(FlexibleCircularAllocatorFixture, TestGetAlignedSize)
{
    std::size_t non_aligned_address = 1000;
    std::size_t alignment = 0;
    EXPECT_EQ(nullptr, flexible_allocator_->Allocate(non_aligned_address, alignment));
}
