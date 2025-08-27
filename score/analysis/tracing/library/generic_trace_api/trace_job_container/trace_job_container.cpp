///
/// @file trace_job_container.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API Thread-safe container holding references to ShmRingBufferElements source file
///

#include "score/analysis/tracing/library/generic_trace_api/trace_job_container/trace_job_container.h"
#include <algorithm>

namespace score
{
namespace analysis
{
namespace tracing
{

bool TraceJobContainer::Add(TraceJobContainerElement element)
{
    auto container_element = container_.GetEmptyElement();
    if (container_element.has_value())
    {
        container_element.value().get().data_.chunk_list_ = element.chunk_list_;
        container_element.value().get().data_.job_type_ = element.job_type_;
        container_element.value().get().data_.original_trace_context_id_ = element.original_trace_context_id_;
        container_element.value().get().data_.ring_buffer_element_ = element.ring_buffer_element_;
        container_element.value().get().is_ready_ = true;
        return true;
    }
    return false;
}

AtomicRingBuffer<TraceJobContainerElement, kTraceJobContainerSize>::AtomicRingBufferResult
TraceJobContainer::GetReadyElement() noexcept
{
    return container_.GetReadyElement();
}

bool TraceJobContainer::ReleaseReadyElement() noexcept
{
    return container_.ReleaseReadyElement();
}

std::size_t TraceJobContainer::Size() noexcept
{
    return static_cast<std::size_t>(container_.Size());
}

std::size_t TraceJobContainer::GetEmptyElementsCount() noexcept
{
    return static_cast<size_t>(container_.GetEmptyElementsCount());
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
