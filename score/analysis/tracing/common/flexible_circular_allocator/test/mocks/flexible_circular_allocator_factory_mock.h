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
#ifndef SCORE_ANALYSIS_TRACING_COMMON_FLEXIBLE_CIRCULAR_ALLOCATOR_TEST_MOCKS_FLEXIBLE_CIRCULAR_ALLOCATOR_FACTORY_MOCK_H
#define SCORE_ANALYSIS_TRACING_COMMON_FLEXIBLE_CIRCULAR_ALLOCATOR_TEST_MOCKS_FLEXIBLE_CIRCULAR_ALLOCATOR_FACTORY_MOCK_H

#include "score/analysis/tracing/common/flexible_circular_allocator/flexible_circular_allocator_factory_interface.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace analysis
{
namespace tracing
{
namespace test
{

class FlexibleCircularAllocatorFactoryMock final : public IFlexibleCircularAllocatorFactory
{
  public:
    MOCK_METHOD(AllocatorResult, CreateAllocator, (void* base_address, std::size_t size), (const, noexcept, override));
};

}  // namespace test
}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_COMMON_FLEXIBLE_CIRCULAR_ALLOCATOR_TEST_MOCKS_FLEXIBLE_CIRCULAR_ALLOCATOR_FACTORY_MOCK_H
