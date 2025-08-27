///
/// @file atomic_ring_buffer_state.h
/// @copyright Copyright (C) 2025, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#ifndef SCORE_ANALYSIS_TRACING_LIBRARY_ATOMIC_RING_BUFFER_STATE_H
#define SCORE_ANALYSIS_TRACING_LIBRARY_ATOMIC_RING_BUFFER_STATE_H

#include <cstdint>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief AtomicRingBufferState struct
///
/// Struct used to store ring buffer state
struct AtomicRingBufferState
{
    uint8_t empty : 1;         ///< Empty flag
    std::uint16_t start : 15;  ///< Start index
    std::uint16_t end : 16;    ///< End index - 15 bits in use but 16-bit width to fill full 4 bytes in structure. This
                               ///< prevents from undefined behaviour when structs are compared.
};

static_assert(sizeof(AtomicRingBufferState) == 4, "Wrong size of AtomicRingBufferState, should be 4 bytes");

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_LIBRARY_ATOMIC_RING_BUFFER_STATE_H
