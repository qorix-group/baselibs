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
#ifndef AAS_ANALYSIS_TRACING_COMMON_SHARED_MEMORY_CHUNK_H
#define AAS_ANALYSIS_TRACING_COMMON_SHARED_MEMORY_CHUNK_H

#include "shared_memory_location.h"
#include "types.h"

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief SharedMemoryChunk class
///
/// class used to store chunks of data residing in shared-memory.
/// Stores the data location and it's size.
class SharedMemoryChunk
{
  public:
    SharedMemoryLocation start_;  ///< Object that stores locators to the data that needs to be traced
    std::size_t size_;            ///< Size of the data that needs to be traced
    friend bool operator==(const SharedMemoryChunk& lhs, const SharedMemoryChunk& rhs) noexcept;
};

static_assert(std::is_trivial<SharedMemoryChunk>::value == true, "SharedMemoryChunk is not trivial");

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // AAS_ANALYSIS_TRACING_COMMON_SHARED_MEMORY_CHUNK_H
