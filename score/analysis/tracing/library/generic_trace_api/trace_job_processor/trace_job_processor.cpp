///
/// @file trace_job_processor.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API trace job processor source file
///

#include "score/analysis/tracing/library/generic_trace_api/trace_job_processor/trace_job_processor.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"

namespace score
{
namespace analysis
{
namespace tracing
{
template <template <class> class AtomicIndirectorType>
TraceJobProcessor<AtomicIndirectorType>::TraceJobProcessor(const ClientIdContainer& client_id_container,
                                                           std::shared_ptr<ITraceJobContainer> job_container,
                                                           TraceJobDeallocator deallocator_function,
                                                           const score::cpp::stop_token& stop_token)
    : ITraceJobProcessor(),
      client_id_container_{client_id_container},
      job_container_{job_container},
      deallocator_function_{std::move(deallocator_function)},
      stop_token_{stop_token}
{
}

template <template <class> class AtomicIndirectorType>
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
ResultBlank TraceJobProcessor<AtomicIndirectorType>::SaveCallback(const TraceClientId client_id,
                                                                  TraceDoneCallBackType callback) noexcept
{
    if (!callback)
    {
        return MakeUnexpected(ErrorCode::kInvalidArgumentFatal);
    }

    // Check if the client ID is already registered
    auto client_find_result = callback_container_.FindIf([client_id](const CallbackElement& element) {
        return element.trace_client_id_ == client_id;
    });
    if (client_find_result.has_value())
    {
        return MakeUnexpected(ErrorCode::kCallbackAlreadyRegisteredRecoverable);
    }

    // Try to obtain free slot in container
    auto callback_element = callback_container_.Acquire();
    if (callback_element.has_value())
    {
        callback_element.value().get().trace_client_id_ = client_id;
        callback_element.value().get().callback_ = std::move(callback);
        return {};
    }

    // No free slot found to save the call back
    return MakeUnexpected(ErrorCode::kNoFreeSlotToSaveTheCallbackRecoverable);
}

template <template <class> class AtomicIndirectorType>
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
ResultBlank TraceJobProcessor<AtomicIndirectorType>::ProcessJobs() noexcept
{
    if (deallocator_function_ == nullptr)
    {
        return MakeUnexpected(ErrorCode::kNoDeallocatorCallbackRegisteredFatal);
    }

    auto element = job_container_->GetReadyElement();
    while (!stop_token_.stop_requested() && element.has_value())  // LCOV_EXCL_LINE This condition cannot be analyzed
    {
        if (element.value().get().data_.original_trace_context_id_.context_id_ !=
            element.value().get().data_.ring_buffer_element_.get().global_context_id_.context_id_)
        {
            auto job_result = DeallocateElement(element.value());
            if (!job_result.has_value())
            {
                return job_result;
            }
        }
        else
        {
            break;
        }
        element = job_container_->GetReadyElement();
    }
    return {};
}

template <template <class> class AtomicIndirectorType>
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
ResultBlank TraceJobProcessor<AtomicIndirectorType>::DeallocateElement(
    std::reference_wrapper<AtomicRingBufferElement> element)
{
    ResultBlank result = {};
    for (std::uint32_t retry_counter = 0U; retry_counter < 10U; retry_counter++)
    {
        auto expected_state = true;
        if (AtomicIndirectorType<bool>::compare_exchange_strong(
                element.get().is_ready_, expected_state, false, std::memory_order_seq_cst) == true)
        {
            result = deallocator_function_(element.get().data_.chunk_list_, element.get().data_.job_type_);

            if (element.get().data_.job_type_ == TraceJobType::kShmJob)
            {
                CallClientCallback(element.get());
            }
            while (job_container_->ReleaseReadyElement() != true)  // LCOV_EXCL_LINE This condition cannot be analyzed
            {
                if (stop_token_.stop_requested())
                {
                    break;
                }
            }
            break;
        }
    }
    // Suppress "AUTOSAR C++14 A8-5-0", The rule states: "All memory shall be initialized before it is read.".
    // False positive, it was initialized when defined
    // coverity[autosar_cpp14_a8_5_0_violation]
    return result;
}

template <template <class> class AtomicIndirectorType>
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
void TraceJobProcessor<AtomicIndirectorType>::CallClientCallback(AtomicRingBufferElement& element) noexcept
{
    // search for the local trace client id equivalent since callbacks were registered with that
    const auto local_client_id =
        client_id_container_.GetLocalTraceClientId(element.data_.original_trace_context_id_.client_id_);
    if (local_client_id.has_value())
    {
        auto client_find_result =
            callback_container_.FindIf([local_client_id](const CallbackElement& callback_element) {
                return callback_element.trace_client_id_ ==
                       local_client_id;  // LCOV_EXCL_LINE This condition cannot be analyzed
            });
        if (client_find_result.has_value())
        {
            score::cpp::ignore =
                client_find_result.value().get().callback_(element.data_.original_trace_context_id_.context_id_);
        }
    }
}

template <template <class> class AtomicIndirectorType>
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
ResultBlank TraceJobProcessor<AtomicIndirectorType>::CleanPendingJobs() noexcept
{
    if (deallocator_function_ == nullptr)
    {
        return MakeUnexpected(ErrorCode::kNoDeallocatorCallbackRegisteredFatal);
    }

    auto element = job_container_->GetReadyElement();
    while (!stop_token_.stop_requested() && element.has_value())  // LCOV_EXCL_LINE This condition cannot be analyzed
    {
        score::cpp::ignore = DeallocateElement(element.value());
        element = job_container_->GetReadyElement();
    }
    return {};
}
template class TraceJobProcessor<score::memory::shared::AtomicIndirectorReal>;
template class TraceJobProcessor<score::memory::shared::AtomicIndirectorMock>;
}  // namespace tracing
}  // namespace analysis
}  // namespace score
