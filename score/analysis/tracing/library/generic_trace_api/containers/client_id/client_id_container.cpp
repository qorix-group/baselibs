#include "score/analysis/tracing/library/generic_trace_api/containers/client_id/client_id_container.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"

#include <iostream>

namespace score
{
namespace analysis
{
namespace tracing
{
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
std::optional<TraceClientId> ClientIdContainer::GetTraceClientId(const BindingType binding,
                                                                 const AppIdType app_identifier) const noexcept
{
    const auto found = container_.FindIf([binding, app_identifier](const auto& element) {
        return (element.binding_ == binding) && (element.app_id_ == app_identifier);
    });
    return found.has_value() ? std::optional<TraceClientId>(found.value().get().local_client_id_) : std::nullopt;
}
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
void ClientIdContainer::SetClientError(ErrorCode error, TraceClientId trace_client_id) noexcept
{
    auto found_element = container_.FindIf([trace_client_id](const auto& element) {
        return (element.local_client_id_ == trace_client_id);
    });
    if (found_element.has_value())
    {
        found_element.value().get().pending_error_ = error;
    }
}
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
void ClientIdContainer::ResetClientError(TraceClientId trace_client_id) noexcept
{
    auto found_element = container_.FindIf([trace_client_id](const auto& element) {
        return (element.local_client_id_ == trace_client_id);
    });
    if (found_element.has_value())
    {
        found_element.value().get().pending_error_.reset();
    }
}

OptionalClientIdElementConstRef ClientIdContainer::GetTraceClientById(
    const TraceClientId trace_client_id) const noexcept
{
    // clang-format off
    return container_.FindIf(
    [trace_client_id](const auto& element) {
        return (element.local_client_id_ == trace_client_id);
    });
    // clang-format on
}
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
Result<ClientIdElementRef> ClientIdContainer::RegisterLocalTraceClient(
    const BindingType binding,
    const AppIdType& app_instance_identifier) noexcept
{
    const auto local_client_id = GetNextLocalTraceClientId();
    auto client_id_element = container_.Acquire();
    if (!client_id_element.has_value())
    {
        std::cerr << "debug(\"LIB\"): GenericTraceAPIImpl::RegisterLocalClient: Exceeded max number of clients, No "
                     "more clients can be registered"
                  // No harm from calling the function in that format
                  //   coverity[autosar_cpp14_m8_4_4_violation]
                  << std::endl;
        return MakeUnexpected(ErrorCode::kNoMoreSpaceForNewClientFatal);
    }

    auto& client_id = client_id_element.value().get();
    client_id.local_client_id_ = local_client_id;
    client_id.binding_ = binding;
    client_id.app_id_ = app_instance_identifier;
    return client_id;
}
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
Result<TraceClientId> ClientIdContainer::GetLocalTraceClientId(
    const TraceClientId ltpm_registered_trace_client_id) const noexcept
{
    const auto found = container_.FindIf([ltpm_registered_trace_client_id](const auto& element) {
        return (element.client_id_ == ltpm_registered_trace_client_id);
    });
    return found.has_value() ? Result<TraceClientId>(found.value().get().local_client_id_)
                             : MakeUnexpected(ErrorCode::kClientNotFoundRecoverable);
}

TraceClientId ClientIdContainer::GetNextLocalTraceClientId()
{
    // Will never exceed kClientIdContainerSize, less than UINT8_MAX
    //  coverity[autosar_cpp14_a4_7_1_violation]
    return ++next_local_id_;
}

std::size_t ClientIdContainer::Size() const noexcept
{
    return container_.Size();
}

ClientIdElementConstRef ClientIdContainer::operator[](const std::size_t index) const noexcept
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index): bounds already checked
    return container_[index];
}

ClientIdElementRef ClientIdContainer::operator[](const std::size_t index) noexcept
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index): bounds already checked
    return container_[index];
}

void ClientIdContainer::InvalidateRemoteRegistrationOfAllClientIds() noexcept
{
    std::size_t index = 0U;
    while (index < container_.Size())
    {
        auto& element = container_[index];
        if (element.local_client_id_ != kInvalidTraceClientId)
        {
            // Keep local data but invalidate the daemon-side handle
            element.client_id_ = kInvalidTraceClientId;
        }
        ++index;
    }
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
