///
/// @file trace_job_status.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#ifndef GENERIC_TRACE_API_RING_BUFFER_STATUS_H
#define GENERIC_TRACE_API_RING_BUFFER_STATUS_H

#include <cstdint>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief Current state of trace job
enum class TraceJobStatus : std::uint8_t
{
    kEmpty = 0x00U,      ///< Element is empty
    kAllocated = 0x01U,  ///< Element is allocated and has owner but is not yet filled with data
    kReady = 0x03U,      ///< Element is ready to be traced
    kTraced = 0x04U,     ///< Element is traced
    kInvalid = 0x05U     ///< element is invalid, can be omitted
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_RING_BUFFER_STATUS_H
