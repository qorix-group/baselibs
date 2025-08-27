#ifndef GENERIC_TRACE_API_TRACE_CLIENT_ID_CONTAINER_H
#define GENERIC_TRACE_API_TRACE_CLIENT_ID_CONTAINER_H

#include "score/analysis/tracing/common/interface_types/types.h"
#include "score/analysis/tracing/library/generic_trace_api/containers/atomic_container/atomic_container.h"
#include "score/analysis/tracing/library/generic_trace_api/containers/client_id/client_id_element.h"

#include <atomic>

namespace score
{
namespace analysis
{
namespace tracing
{

// Variable is used in ClientIdAtomicContainer alias
//  coverity[autosar_cpp14_a0_1_1_violation]
static constexpr std::size_t kClientIdContainerSize = 20U;
using ClientIdAtomicContainer = AtomicContainer<ClientIdElement, kClientIdContainerSize>;

class ClientIdContainer
{
  public:
    std::optional<TraceClientId> GetTraceClientId(const BindingType binding,
                                                  const AppIdType app_identifier) const noexcept;
    void ResetClientError(TraceClientId trace_client_id) noexcept;
    void SetClientError(ErrorCode error, TraceClientId trace_client_id) noexcept;
    OptionalClientIdElementConstRef GetTraceClientById(const TraceClientId trace_client_id) const noexcept;
    Result<ClientIdElementRef> RegisterLocalTraceClient(const BindingType binding,
                                                        const AppIdType& app_instance_identifier) noexcept;
    Result<TraceClientId> GetLocalTraceClientId(const TraceClientId ltpm_registered_trace_client_id) const noexcept;
    std::size_t Size() const noexcept;
    ClientIdElementConstRef operator[](const std::size_t index) const noexcept;
    ClientIdElementRef operator[](const std::size_t index) noexcept;
    void InvalidateRemoteRegistrationOfAllClientIds() noexcept;

  private:
    TraceClientId GetNextLocalTraceClientId();

    TraceClientId next_local_id_{0U};
    ClientIdAtomicContainer container_;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_TRACE_CLIENT_ID_CONTAINER_H
