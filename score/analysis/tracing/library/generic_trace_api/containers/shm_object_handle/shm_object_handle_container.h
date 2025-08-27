#ifndef GENERIC_TRACE_API_TRACE_SHM_OBJECT_HANDLE_CONTAINER_H
#define GENERIC_TRACE_API_TRACE_SHM_OBJECT_HANDLE_CONTAINER_H

#include "score/analysis/tracing/common/interface_types/types.h"
#include "score/analysis/tracing/library/generic_trace_api/containers/atomic_container/atomic_container.h"
#include "score/analysis/tracing/library/generic_trace_api/containers/shm_object_handle/shm_object_handle_element.h"

#include <atomic>
#include <type_traits>

namespace score
{
namespace analysis
{
namespace tracing
{

// Variable is used in ShmObjectHandleAtomicContainer alias
//  coverity[autosar_cpp14_a0_1_1_violation]
static constexpr std::size_t kShmObjectHandleContainerSize = 200U;
using ShmObjectHandleAtomicContainer = AtomicContainer<ShmObjectHandleElement, kShmObjectHandleContainerSize>;

class ShmObjectHandleContainer
{
  public:
    Result<ShmObjectHandleElementRef> RegisterLocalShmObjectHandle(const std::int32_t shm_object_fd) noexcept;
    void DeregisterLocalShmObject(const ShmObjectHandle handle) noexcept;
    bool IsShmObjectAlreadyRegistered(const std::int32_t shm_object_fd) const noexcept;
    Result<ShmObjectHandle> GetLtpmDaemonRegisteredShmObjectHandle(const ShmObjectHandle handle) const noexcept;
    OptionalShmObjectHandleElementConstRef FindIf(
        const std::function<bool(const ShmObjectHandleElement&)>& predicate) const noexcept;
    OptionalShmObjectHandleElementRef FindIf(
        const std::function<bool(const ShmObjectHandleElement&)>& predicate) noexcept;
    void Release(ShmObjectHandleElementRef shm_object_handle_element);
    std::size_t Size() const noexcept;
    ShmObjectHandleElementConstRef operator[](const std::size_t index) const noexcept;
    ShmObjectHandleElementRef operator[](const std::size_t index) noexcept;
    void InvalidateRemoteRegistrationOfAllShmObjects() noexcept;

  private:
    ShmObjectHandle GetNextLocalShmObjectHandle();

    ShmObjectHandle next_shm_object_handle_{0};
    ShmObjectHandleAtomicContainer container_;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_TRACE_SHM_OBJECT_HANDLE_CONTAINER_H
