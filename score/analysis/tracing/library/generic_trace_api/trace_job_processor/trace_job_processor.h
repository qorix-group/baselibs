///
/// @file trace_job_processor.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API trace job processor header file
///

#ifndef GENERIC_TRACE_API_TRACE_JOB_PROCESSOR_H
#define GENERIC_TRACE_API_TRACE_JOB_PROCESSOR_H

#include "score/analysis/tracing/library/generic_trace_api/containers/atomic_container/atomic_container.h"
#include "score/analysis/tracing/library/generic_trace_api/containers/atomic_ring_buffer/atomic_ring_buffer.h"
#include "score/analysis/tracing/library/generic_trace_api/containers/client_id/client_id_container.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_container/trace_job_container.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_processor/i_trace_job_processor.h"
#include "score/memory/shared/atomic_indirector.h"
#include <array>
#include <atomic>
namespace score
{
namespace analysis
{
namespace tracing
{
using AtomicRingBufferElement =
    AtomicRingBuffer<TraceJobContainerElement, kTraceJobContainerSize>::AtomicRingBufferElement;
constexpr std::size_t kCallbackMaxNumber = 10U;

// Suppress "AUTOSAR C++14 M3-2-3" rule finding: "A type, object or function that is used in multiple translation units
// shall be declared in one and only one file.".
// This is false positive. TraceJobProcessor is declared only once in the library side.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <template <class> class AtomicIndirectorType = score::memory::shared::AtomicIndirectorReal>
class TraceJobProcessor : public ITraceJobProcessor
{
  public:
    explicit TraceJobProcessor(const ClientIdContainer& client_id_container,
                               std::shared_ptr<ITraceJobContainer> job_container,
                               TraceJobDeallocator deallocator_function,
                               const score::cpp::stop_token& stop_token);

    TraceJobProcessor() = delete;
    TraceJobProcessor(const TraceJobProcessor&) = delete;
    TraceJobProcessor& operator=(const TraceJobProcessor&) = delete;
    TraceJobProcessor(TraceJobProcessor&&) = delete;
    TraceJobProcessor& operator=(TraceJobProcessor&&) = delete;
    // No harm from using more than overloaded function
    // coverity[autosar_cpp14_m3_9_1_violation]
    ResultBlank SaveCallback(const TraceClientId client_id, TraceDoneCallBackType callback) noexcept override;
    ResultBlank ProcessJobs() noexcept override;
    ResultBlank CleanPendingJobs() noexcept override;
    // No harm to define default destructor
    //  coverity[autosar_cpp14_a10_3_1_violation]
    ~TraceJobProcessor() = default;

  private:
    struct CallbackElement
    {
        // No harm from not declaring private members here
        // coverity[autosar_cpp14_m11_0_1_violation]
        TraceClientId trace_client_id_{0u};
        // No harm from not declaring private members here
        // coverity[autosar_cpp14_m11_0_1_violation]
        TraceDoneCallBackType callback_{};
    };

    ResultBlank DeallocateElement(std::reference_wrapper<AtomicRingBufferElement> element);
    void CallClientCallback(AtomicRingBufferElement& element) noexcept;

    const ClientIdContainer& client_id_container_;
    std::shared_ptr<ITraceJobContainer> job_container_;
    TraceJobDeallocator deallocator_function_;
    AtomicContainer<CallbackElement, kCallbackMaxNumber> callback_container_;
    score::cpp::stop_token stop_token_;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_TRACE_JOB_PROCESSOR_H
