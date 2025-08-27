#ifndef GENERIC_TRACE_API_TRACE_SHM_OBJECT_HANDLE_ELEMENT_H
#define GENERIC_TRACE_API_TRACE_SHM_OBJECT_HANDLE_ELEMENT_H

#include "score/analysis/tracing/common/interface_types/types.h"

#include <atomic>

namespace score
{
namespace analysis
{
namespace tracing
{

class ShmObjectHandleElement
{
  public:
    ShmObjectHandleElement();
    ShmObjectHandleElement(const ShmObjectHandle local_handle,
                           const ShmObjectHandle handle,
                           std::int32_t file_descriptor);
    // No harm from not declaring private members here
    // coverity[autosar_cpp14_m11_0_1_violation]
    ShmObjectHandle local_handle_;
    // No harm from not declaring private members here
    // coverity[autosar_cpp14_m11_0_1_violation]
    ShmObjectHandle handle_;
    // No harm from not declaring private members here
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::int32_t file_descriptor_;
};

bool operator==(const ShmObjectHandleElement& lhs, const ShmObjectHandleElement& rhs) noexcept;

using ShmObjectHandleElementRef = std::reference_wrapper<ShmObjectHandleElement>;
using ShmObjectHandleElementConstRef = std::reference_wrapper<const ShmObjectHandleElement>;
using OptionalShmObjectHandleElementRef = std::optional<ShmObjectHandleElementRef>;
using OptionalShmObjectHandleElementConstRef = std::optional<ShmObjectHandleElementConstRef>;

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_TRACE_SHM_OBJECT_HANDLE_ELEMENT_H
