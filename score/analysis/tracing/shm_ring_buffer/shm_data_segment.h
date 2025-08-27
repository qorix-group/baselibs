///
/// @file shm_data_segment.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#ifndef GENERIC_TRACE_API_SHM_DATA_SEGMENT_H
#define GENERIC_TRACE_API_SHM_DATA_SEGMENT_H

#include "score/analysis/tracing/shm_ring_buffer/shm_ring_buffer_element.h"
#include "score/analysis/tracing/shm_ring_buffer/shm_ring_buffer_state.h"
#include "score/memory/shared/memory_resource_proxy.h"
#include "score/memory/shared/offset_ptr.h"
#include "score/memory/shared/polymorphic_offset_ptr_allocator.h"
#include "score/memory/shared/vector.h"
#include <atomic>
#include <cstdint>

namespace score
{
namespace analysis
{
namespace tracing
{

using ShmVector = score::memory::shared::Vector<ShmRingBufferElement>;

/// @brief ShmDataSegment struct
///
/// Struct used to store shared-memory resources
class ShmDataSegment
{
  public:
    // No harm to define it as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    std::atomic<ShmRingBufferState> state_;  ///< State of the ring buffer
    // No harm to define it as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    ShmVector vector_;  ///< Vector with data
    // No harm to define it as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    std::atomic_uint32_t use_count_{0u};

    ShmDataSegment(std::size_t size, const score::memory::shared::MemoryResourceProxy* const memory)
        : state_{ShmRingBufferState{1U, 0U, 0U}}, vector_{size, memory}
    {
    }
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_SHM_DATA_SEGMENT_H
