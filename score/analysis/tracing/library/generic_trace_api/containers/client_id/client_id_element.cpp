///
/// @file trace_job_container_element.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API trace job allocator interface source file
///

#include "score/analysis/tracing/library/generic_trace_api/containers/client_id/client_id_element.h"

#include <array>

namespace score
{
namespace analysis
{
namespace tracing
{

ClientIdElement::ClientIdElement()
    : ClientIdElement(kInvalidTraceClientId, kInvalidTraceClientId, BindingType::kUndefined, AppIdType{})
{
}

ClientIdElement::ClientIdElement(const TraceClientId client_id,
                                 const TraceClientId local_client_id,
                                 const BindingType binding,
                                 const AppIdType app_id)
    : client_id_{client_id},
      local_client_id_{local_client_id},
      binding_{binding},
      app_id_{app_id},
      pending_error_{std::nullopt}
{
}

bool operator==(const ClientIdElement& lhs, const ClientIdElement& rhs) noexcept
{
    return (((lhs.client_id_ == rhs.client_id_) && (lhs.local_client_id_ == rhs.local_client_id_)) &&
            ((lhs.app_id_ == rhs.app_id_) && (lhs.binding_ == rhs.binding_)));
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
