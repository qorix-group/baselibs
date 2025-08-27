///
/// @file shm_ring_buffer_element.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#ifndef GENERIC_TRACE_API_RING_BUFFER_ELEMENT_H
#define GENERIC_TRACE_API_RING_BUFFER_ELEMENT_H

#include "score/analysis/tracing/common/interface_types/shared_memory_location.h"
#include "score/analysis/tracing/common/interface_types/types.h"
#include "score/analysis/tracing/shm_ring_buffer/trace_job_status.h"
#include <atomic>
#include <cstdint>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief ShmRingBufferElement class
///
/// Class used to store single ring buffer element
// NOLINTBEGIN(score-struct-usage-compliance): Intended struct semantic
// Suppress "AUTOSAR C++14 A11-0-2" rule finding. This rule states: "A type defined as struct shall:
// (1) provide only public data members, (2) not provide any special member functions or methods,
// (3) not be a base of another struct or class, (4) not inherit from another struct or class."
// ShmRingBufferElement has public data members and special method
// coverity[autosar_cpp14_a11_0_2_violation]
struct ShmRingBufferElement
{
  public:
    // No harm to define it as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    GlobalTraceContextId global_context_id_;
    // No harm to define it as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    std::atomic<TraceJobStatus> status_;
    // No harm to define it as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    SharedMemoryLocation chunk_list_;

    ShmRingBufferElement() noexcept : global_context_id_{0U, 0U}, status_{TraceJobStatus::kEmpty}, chunk_list_{0, 0U} {}
};
// NOLINTEND(score-struct-usage-compliance): Intended struct semantic

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_RING_BUFFER_ELEMENT_H
