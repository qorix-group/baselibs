///
/// @file trace_job_container.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API Thread-safe container holding references to ShmRingBufferElements header file
///

#ifndef GENERIC_TRACE_API_I_TRACE_JOB_CONTAINER_H
#define GENERIC_TRACE_API_I_TRACE_JOB_CONTAINER_H

#include "score/analysis/tracing/library/generic_trace_api/containers/atomic_ring_buffer/atomic_ring_buffer.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_container/trace_job_container_element.h"

namespace score
{
namespace analysis
{
namespace tracing
{
// Rationale: False positive, variable is used.
// coverity[autosar_cpp14_a0_1_1_violation]
static constexpr std::size_t kTraceJobContainerSize = 500U;

class ITraceJobContainer
{
  public:
    ITraceJobContainer() = default;
    virtual ~ITraceJobContainer() = default;  // Virtual destructor for proper cleanup of derived classes
    ITraceJobContainer(const ITraceJobContainer& element) = delete;
    ITraceJobContainer& operator=(const ITraceJobContainer&) = delete;
    ITraceJobContainer(ITraceJobContainer&&) = delete;
    ITraceJobContainer& operator=(ITraceJobContainer&&) = delete;
    virtual bool Add(TraceJobContainerElement element) = 0;

    virtual AtomicRingBuffer<TraceJobContainerElement, kTraceJobContainerSize>::AtomicRingBufferResult
    GetReadyElement() noexcept = 0;

    virtual bool ReleaseReadyElement() noexcept = 0;

    virtual std::size_t Size() noexcept = 0;

    virtual std::size_t GetEmptyElementsCount() noexcept = 0;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_I_TRACE_JOB_CONTAINER_H
