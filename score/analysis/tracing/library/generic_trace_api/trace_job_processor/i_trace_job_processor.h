///
/// @file i_trace_job_processor.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API trace job processor interface header file
///

#ifndef GENERIC_TRACE_API_I_TRACE_JOB_PROCESSOR_H
#define GENERIC_TRACE_API_I_TRACE_JOB_PROCESSOR_H

#include "score/analysis/tracing/common/interface_types/shared_memory_location.h"
#include "score/analysis/tracing/common/interface_types/types.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_container/trace_job_type.h"
#include "score/result/result.h"

namespace score
{
namespace analysis
{
namespace tracing
{

using TraceJobDeallocator = score::cpp::callback<TraceResult(SharedMemoryLocation, TraceJobType)>;
// Suppress "AUTOSAR C++14 M3-2-3" rule finding: "A type, object or function that is used in multiple translation units
// shall be declared in one and only one file.".
// This is false positive. ITraceJobProcessor is declared only once in the library side.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
class ITraceJobProcessor
{
  public:
    virtual ~ITraceJobProcessor() = default;
    ITraceJobProcessor() noexcept = default;
    ITraceJobProcessor(const ITraceJobProcessor&) noexcept = delete;
    ITraceJobProcessor(ITraceJobProcessor&&) noexcept = delete;
    ITraceJobProcessor& operator=(const ITraceJobProcessor&) = delete;
    ITraceJobProcessor& operator=(ITraceJobProcessor&&) = delete;
    virtual ResultBlank ProcessJobs() noexcept = 0;
    virtual ResultBlank CleanPendingJobs() noexcept = 0;
    virtual ResultBlank SaveCallback(const TraceClientId client_id, const TraceDoneCallBackType callback) noexcept = 0;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_I_TRACE_JOB_PROCESSOR_H
