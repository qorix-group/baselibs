///
/// @file trace_job_container_element.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API trace job container element type
///

#ifndef GENERIC_TRACE_API_TRACE_CLIENT_ID_ELEMENT_H
#define GENERIC_TRACE_API_TRACE_CLIENT_ID_ELEMENT_H

#include "score/analysis/tracing/common/interface_types/types.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"

#include <atomic>

namespace score
{
namespace analysis
{
namespace tracing
{

static constexpr TraceClientId kInvalidTraceClientId = 0U;

class ClientIdElement
{
  public:
    ClientIdElement(const TraceClientId client_id,
                    const TraceClientId local_client_id,
                    const BindingType binding,
                    const AppIdType app_id);
    ClientIdElement();
    ClientIdElement(const ClientIdElement& element) = delete;
    ClientIdElement& operator=(const ClientIdElement&) = delete;
    ClientIdElement(ClientIdElement&&) = delete;
    ClientIdElement& operator=(ClientIdElement&&) = delete;
    ~ClientIdElement() = default;
    // No harm from not declaring private members here
    // coverity[autosar_cpp14_m11_0_1_violation]
    TraceClientId client_id_;
    // No harm from not declaring private members here
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::atomic<TraceClientId> local_client_id_;
    // No harm from not declaring private members here
    // coverity[autosar_cpp14_m11_0_1_violation]
    BindingType binding_;
    // No harm from not declaring private members here
    // coverity[autosar_cpp14_m11_0_1_violation]
    AppIdType app_id_;
    // No harm from not declaring private members here
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::optional<ErrorCode> pending_error_;
};

bool operator==(const ClientIdElement& lhs, const ClientIdElement& rhs) noexcept;

using ClientIdElementRef = std::reference_wrapper<ClientIdElement>;
using ClientIdElementConstRef = std::reference_wrapper<const ClientIdElement>;
using OptionalClientIdElementRef = std::optional<ClientIdElementRef>;
using OptionalClientIdElementConstRef = std::optional<ClientIdElementConstRef>;

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_TRACE_CLIENT_ID_ELEMENT_H
