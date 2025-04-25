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
#ifndef SCORE_ANALYSIS_TRACING_COMMON_FLEXIBLE_CIRCULAR_ALLOCATOR_FACTORY_INTERFACE_H
#define SCORE_ANALYSIS_TRACING_COMMON_FLEXIBLE_CIRCULAR_ALLOCATOR_FACTORY_INTERFACE_H
#include "score/analysis/tracing/common/flexible_circular_allocator/flexible_circular_allocator_interface.h"

namespace score
{
namespace analysis
{
namespace tracing
{

using AllocatorResult = Result<std::shared_ptr<IFlexibleCircularAllocator>>;

class IFlexibleCircularAllocatorFactory
{

  protected:
    IFlexibleCircularAllocatorFactory(const IFlexibleCircularAllocatorFactory&) noexcept = default;

    IFlexibleCircularAllocatorFactory(IFlexibleCircularAllocatorFactory&&) noexcept = default;

    IFlexibleCircularAllocatorFactory& operator=(const IFlexibleCircularAllocatorFactory&) = default;

    IFlexibleCircularAllocatorFactory& operator=(IFlexibleCircularAllocatorFactory&&) = default;

  public:
    virtual ~IFlexibleCircularAllocatorFactory() = default;

    IFlexibleCircularAllocatorFactory() = default;

    virtual AllocatorResult CreateAllocator(void* base_address, std::size_t size) const = 0;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_COMMON_FLEXIBLE_CIRCULAR_ALLOCATOR_FACTORY_INTERFACE_H
