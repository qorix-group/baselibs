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
#include "score/analysis/tracing/common/flexible_circular_allocator/lockless_flexible_circular_allocator_factory.h"
#include "gtest/gtest.h"

#include <array>  // Include std::array

namespace score
{
namespace analysis
{
namespace tracing
{

// Mock memory buffer for testing
constexpr std::size_t kTestBufferSize = 1024;
std::array<char, kTestBufferSize> testBuffer;

class LocklessFlexibleCircularAllocatorFactoryTest : public ::testing::Test
{

  protected:
    void SetUp() override
    {
        allocator_factory_ = std::make_unique<LocklessFlexibleCircularAllocatorFactory>();
    }

    std::unique_ptr<IFlexibleCircularAllocatorFactory> allocator_factory_;
};

// Test case for creating a LockFree allocator
TEST_F(LocklessFlexibleCircularAllocatorFactoryTest, CreateAllocator)
{
    auto allocator = allocator_factory_->CreateAllocator(testBuffer.data(), kTestBufferSize).value();
    ASSERT_NE(allocator, nullptr);
    EXPECT_NE(
        dynamic_cast<LocklessFlexibleCircularAllocator<score::memory::shared::AtomicIndirectorReal>*>(allocator.get()),
        nullptr);
}

// Test case for handling a nullptr base address
TEST_F(LocklessFlexibleCircularAllocatorFactoryTest, CreateAllocatorWithNullBaseAddress)
{
    auto allocator = allocator_factory_->CreateAllocator(nullptr, kTestBufferSize);
    EXPECT_EQ(allocator.has_value(), false);
    ASSERT_TRUE(allocator.error().Message().compare("Base address is nullptr") == 0);
}

// Test case for handling a size of 0
TEST_F(LocklessFlexibleCircularAllocatorFactoryTest, CreateAllocatorWithZeroSize)
{
    auto allocator = allocator_factory_->CreateAllocator(testBuffer.data(), 0);
    EXPECT_EQ(allocator.has_value(), false);
    ASSERT_TRUE(allocator.error().Message().compare("Size is zero") == 0);
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
