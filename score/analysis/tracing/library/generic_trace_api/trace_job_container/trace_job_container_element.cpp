///
/// @file trace_job_container_element.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API trace job allocator interface source file
///

#include "score/analysis/tracing/library/generic_trace_api/trace_job_container/trace_job_container_element.h"

namespace score
{
namespace analysis
{
namespace tracing
{

namespace
{
// No harm as it is used only as dummy reference to create empty objects.
// coverity[autosar_cpp14_a3_3_2_violation]
ShmRingBufferElement empty_ring_buffer_element_{};
}  // namespace

bool operator==(const TraceJobContainerElement& lhs, const TraceJobContainerElement& rhs) noexcept
{
    return (lhs.original_trace_context_id_.context_id_ == rhs.original_trace_context_id_.context_id_) &&
           (lhs.original_trace_context_id_.client_id_ == rhs.original_trace_context_id_.client_id_);
}

TraceJobContainerElement::TraceJobContainerElement()
    : TraceJobContainerElement(empty_ring_buffer_element_, {0U, 0U}, TraceJobType::kLocalJob, SharedMemoryLocation{})
{
}

TraceJobContainerElement::TraceJobContainerElement(std::reference_wrapper<ShmRingBufferElement> ring_buffer_element,
                                                   GlobalTraceContextId original_trace_context_id,
                                                   TraceJobType job_type,
                                                   SharedMemoryLocation chunk_list)
    : ring_buffer_element_{ring_buffer_element},
      original_trace_context_id_{original_trace_context_id},
      job_type_{job_type},
      chunk_list_{chunk_list}
{
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
