///
/// @file trace_job_container_element.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API trace job container element type
///

#ifndef GENERIC_TRACE_API_TRACE_JOB_CONTAINER_ELEMENT_H
#define GENERIC_TRACE_API_TRACE_JOB_CONTAINER_ELEMENT_H

#include "score/analysis/tracing/library/generic_trace_api/trace_job_container/trace_job_type.h"
#include "score/analysis/tracing/shm_ring_buffer/shm_ring_buffer_element.h"
#include <atomic>
#include <functional>

namespace score
{
namespace analysis
{
namespace tracing
{

class TraceJobContainerElement
{
  public:
    TraceJobContainerElement();
    TraceJobContainerElement(std::reference_wrapper<ShmRingBufferElement> ring_buffer_element,
                             GlobalTraceContextId original_trace_context_id,
                             TraceJobType job_type,
                             SharedMemoryLocation chunk_list);
    TraceJobContainerElement(const TraceJobContainerElement& element) = default;
    TraceJobContainerElement& operator=(const TraceJobContainerElement&) = delete;
    TraceJobContainerElement(TraceJobContainerElement&&) = default;
    TraceJobContainerElement& operator=(TraceJobContainerElement&&) = delete;
    ~TraceJobContainerElement() = default;
    // No harm from not declaring private members here
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::reference_wrapper<ShmRingBufferElement> ring_buffer_element_;
    // No harm from not declaring private members here
    // coverity[autosar_cpp14_m11_0_1_violation]
    GlobalTraceContextId original_trace_context_id_;
    // No harm from not declaring private members here
    // coverity[autosar_cpp14_m11_0_1_violation]
    TraceJobType job_type_;
    // No harm from not declaring private members here
    // coverity[autosar_cpp14_m11_0_1_violation]
    SharedMemoryLocation chunk_list_;
};

bool operator==(const TraceJobContainerElement& lhs, const TraceJobContainerElement& rhs) noexcept;

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_TRACE_JOB_CONTAINER_ELEMENT_H
