///
/// @file trace_job_container_element.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API trace job allocator interface source file
///

#include "score/analysis/tracing/library/generic_trace_api/containers/shm_object_handle/shm_object_handle_element.h"

#include <array>

namespace score
{
namespace analysis
{
namespace tracing
{

ShmObjectHandleElement::ShmObjectHandleElement()
    : ShmObjectHandleElement(kInvalidSharedObjectIndex, kInvalidSharedObjectIndex, {})
{
}

ShmObjectHandleElement::ShmObjectHandleElement(const ShmObjectHandle local_handle,
                                               const ShmObjectHandle handle,
                                               std::int32_t file_descriptor)
    : local_handle_{local_handle}, handle_{handle}, file_descriptor_{file_descriptor}
{
}

bool operator==(const ShmObjectHandleElement& lhs, const ShmObjectHandleElement& rhs) noexcept
{
    return ((lhs.file_descriptor_ == rhs.file_descriptor_) && (lhs.handle_ == rhs.handle_)) &&
           (lhs.local_handle_ == rhs.local_handle_);
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
