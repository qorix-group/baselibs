///
/// @file trace_job_type.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#ifndef GENERIC_TRACE_API_TRACE_JOB_TYPE_H
#define GENERIC_TRACE_API_TRACE_JOB_TYPE_H

#include <cstdint>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief Type of trace job
enum class TraceJobType : std::uint8_t
{
    kLocalJob = 0x00U,  ///< Local trace job
    kShmJob = 0x01U,    ///< Shared-memory trace job
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_TRACE_JOB_TYPE_H
