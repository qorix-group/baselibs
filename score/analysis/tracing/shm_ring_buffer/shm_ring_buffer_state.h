///
/// @file shm_ring_buffer_state.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#ifndef GENERIC_TRACE_API_SHM_RING_BUFFER_STATE_H
#define GENERIC_TRACE_API_SHM_RING_BUFFER_STATE_H

#include "score/analysis/tracing/shm_ring_buffer/shm_ring_buffer_element.h"
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

/// @brief ShmRingBufferState struct
///
/// Struct used to store ring buffer state
struct ShmRingBufferState
{
    std::uint8_t empty : 1;    ///< Empty flag
    std::uint16_t start : 15;  ///< Start index
    std::uint16_t end : 16;    ///< End index - 15 bits in use but 16-bit width to fill full 4 bytes in structure. This
                               ///< prevents from undefined behaviour when structs are compared.
};

static_assert(sizeof(ShmRingBufferState) == 4, "Wrong size of ShmRingBufferState, should be 4 bytes");

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_SHM_RING_BUFFER_STATE_H
