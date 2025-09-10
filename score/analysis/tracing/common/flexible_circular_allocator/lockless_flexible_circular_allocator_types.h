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
#ifndef SCORE_ANALYSIS_TRACING_COMMON_LOCKLESS_FLEXIBLE_CIRCULAR_ALLOCATOR_TYPES_H
#define SCORE_ANALYSIS_TRACING_COMMON_LOCKLESS_FLEXIBLE_CIRCULAR_ALLOCATOR_TYPES_H

namespace score
{
namespace analysis
{
namespace tracing
{
// Suppress "AUTOSAR C++14 A0-1-1" rule finds: "A project shall not contain instances of non-volatile variables
// being given values that are not subsequently used"
// False positive, variable is used.
// coverity[autosar_cpp14_a0_1_1_violation : FALSE]
constexpr std::uint32_t kListEntryArraySize = 2u * 4096u;

enum class ListEntryFlag : std::uint8_t
{
    kInUse = 0,
    kFree = 1,
};

struct alignas(std::max_align_t) BufferBlock
{
    std::uint32_t list_entry_offset;
    std::uint32_t block_length;
};

struct ListEntry
{
    std::uint32_t offset;
    std::uint16_t length;
    uint8_t flags;
};

inline std::size_t GetAlignedSize(const std::size_t non_aligned_size, std::size_t alignment)
{
    if (alignment == static_cast<std::size_t>(0U))
    {
        alignment = alignof(std::max_align_t);
    }
    const std::size_t remainder = non_aligned_size % alignment;
    if (remainder == static_cast<std::size_t>(0U))
    {
        return non_aligned_size;
    }
    // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall
    // not lead to data loss.".
    // Rationale: The arithmetic to compute the aligned size is performed entirely with std::size_t values,
    // ensuring that no truncation occurs. The expression 'non_aligned_size + alignment - remainder' calculates
    // the next multiple of 'alignment' correctly, assuming valid input, without any type conversions that could
    // result in data loss.
    // coverity[autosar_cpp14_a4_7_1_violation]
    return non_aligned_size + alignment - remainder;
}
}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_COMMON_LOCKLESS_FLEXIBLE_CIRCULAR_ALLOCATOR_TYPES_H
