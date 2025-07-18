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
#include "shared_memory_chunk.h"

namespace score
{
namespace analysis
{
namespace tracing
{
// Define the equality operator
bool operator==(const SharedMemoryChunk& lhs, const SharedMemoryChunk& rhs) noexcept
{
    return (lhs.size_ == rhs.size_) && (lhs.start_ == rhs.start_);
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
