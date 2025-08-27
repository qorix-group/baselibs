///
/// @file trace_job_container.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API Thread-safe container holding references to ShmRingBufferElements header file
///

#ifndef GENERIC_TRACE_API_TRACE_JOB_CONTAINER_H
#define GENERIC_TRACE_API_TRACE_JOB_CONTAINER_H

#include "score/analysis/tracing/library/generic_trace_api/trace_job_container/i_trace_job_container.h"

namespace score
{
namespace analysis
{
namespace tracing
{
// False positive, variable is used.
// coverity[autosar_cpp14_a0_1_1_violation]
class TraceJobContainer : public ITraceJobContainer
{
  public:
    TraceJobContainer() = default;
    // coverity[autosar_cpp14_a10_3_1_violation] Destructor is not virtual method.
    ~TraceJobContainer() = default;
    TraceJobContainer(const TraceJobContainer& element) = delete;
    TraceJobContainer& operator=(const TraceJobContainer&) = delete;
    TraceJobContainer(TraceJobContainer&&) = delete;
    TraceJobContainer& operator=(TraceJobContainer&&) = delete;
    bool Add(TraceJobContainerElement element) override;
    AtomicRingBuffer<TraceJobContainerElement, kTraceJobContainerSize>::AtomicRingBufferResult
    GetReadyElement() noexcept override;
    bool ReleaseReadyElement() noexcept override;
    std::size_t Size() noexcept override;
    std::size_t GetEmptyElementsCount() noexcept override;

  private:
    AtomicRingBuffer<TraceJobContainerElement, kTraceJobContainerSize> container_;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_TRACE_JOB_CONTAINER_H
