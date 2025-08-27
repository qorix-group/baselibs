#include "score/analysis/tracing/library/generic_trace_api/containers/shm_object_handle/shm_object_handle_container.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"

#include <iostream>

namespace score
{
namespace analysis
{
namespace tracing
{
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// std::terminate() will not  implicitly be called from MakeUnexpected as it declared as noexcept
// coverity[autosar_cpp14_a15_5_3_violation]
Result<ShmObjectHandleElementRef> ShmObjectHandleContainer::RegisterLocalShmObjectHandle(
    const std::int32_t shm_object_fd) noexcept
{
    const auto local_shm_object_handle = GetNextLocalShmObjectHandle();
    auto shm_object_handle_element = container_.Acquire();
    if (!shm_object_handle_element.has_value())
    {
        std::cerr << "debug(\"LIB\"): GenericTraceAPIImpl::RegisterLocalShmObjectHandle: No more shared memory objects "
                     "can be registered"
                  // No harm from calling the function in that format
                  //   coverity[autosar_cpp14_m8_4_4_violation]
                  << std::endl;
        return MakeUnexpected(ErrorCode::kNoMoreSpaceForNewShmObjectFatal);
    }
    shm_object_handle_element.value().get().local_handle_ = local_shm_object_handle;
    shm_object_handle_element.value().get().file_descriptor_ = shm_object_fd;
    return shm_object_handle_element.value().get();
}

void ShmObjectHandleContainer::DeregisterLocalShmObject(const ShmObjectHandle handle) noexcept
{
    for (std::size_t shm_object_handle_index = 0UL; shm_object_handle_index < container_.Size();
         shm_object_handle_index++)
    {
        auto& current_shm_object_handle = container_[shm_object_handle_index];
        if (current_shm_object_handle.local_handle_ == handle)
        {
            current_shm_object_handle.local_handle_ = kInvalidSharedObjectIndex;
            container_.Release(shm_object_handle_index);
            return;
        }
    }
}

bool ShmObjectHandleContainer::IsShmObjectAlreadyRegistered(const std::int32_t shm_object_fd) const noexcept
{
    return container_
        .FindIf([shm_object_fd](const auto& element) {
            // No harm as there are already parenthesis around the logical operators
            // coverity[autosar_cpp14_a5_2_6_violation]
            return (element.handle_ != kInvalidSharedObjectIndex) && (element.file_descriptor_ == shm_object_fd);
        })
        .has_value();
}
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// std::terminate() will not  implicitly be called from FindIf as it declared as noexcept
// coverity[autosar_cpp14_a15_5_3_violation]
Result<ShmObjectHandle> ShmObjectHandleContainer::GetLtpmDaemonRegisteredShmObjectHandle(
    const ShmObjectHandle handle) const noexcept
{
    const auto found = container_.FindIf([handle](const auto& element) {
        return (element.local_handle_ == handle) && (element.handle_ != kInvalidSharedObjectIndex);
    });
    if (!found.has_value())
    {
        return MakeUnexpected(ErrorCode::kInvalidShmObjectHandleFatal);
    }
    return found.value().get().handle_;
}

ShmObjectHandle ShmObjectHandleContainer::GetNextLocalShmObjectHandle()
{
    // Will never exceed kShmObjectHandleContainerSize, less than INT32_MAX
    //  coverity[autosar_cpp14_a4_7_1_violation]
    return ++next_shm_object_handle_;
}

OptionalShmObjectHandleElementConstRef ShmObjectHandleContainer::FindIf(
    const std::function<bool(const ShmObjectHandleElement&)>& predicate) const noexcept
{
    return container_.FindIf(predicate);
}

OptionalShmObjectHandleElementRef ShmObjectHandleContainer::FindIf(
    const std::function<bool(const ShmObjectHandleElement&)>& predicate) noexcept
{
    return container_.FindIf(predicate);
}

void ShmObjectHandleContainer::Release(ShmObjectHandleElementRef shm_object_handle_element)
{
    container_.Release(shm_object_handle_element.get());
}

std::size_t ShmObjectHandleContainer::Size() const noexcept
{
    return container_.Size();
}

ShmObjectHandleElementRef ShmObjectHandleContainer::operator[](const std::size_t index) noexcept
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index): bounds already checked
    return container_[index];
}

ShmObjectHandleElementConstRef ShmObjectHandleContainer::operator[](const std::size_t index) const noexcept
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index): bounds already checked
    return container_[index];
}

void ShmObjectHandleContainer::InvalidateRemoteRegistrationOfAllShmObjects() noexcept
{
    std::size_t index = 0U;
    while (index < container_.Size())
    {
        auto& element = container_[index];
        if (element.local_handle_ != kInvalidSharedObjectIndex)
        {
            // Keep local data but invalidate the daemon-side handle
            element.handle_ = kInvalidSharedObjectIndex;
        }
        ++index;
    }
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
