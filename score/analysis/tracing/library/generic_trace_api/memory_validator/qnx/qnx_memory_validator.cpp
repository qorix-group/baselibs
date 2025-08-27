#include "score/analysis/tracing/library/generic_trace_api/memory_validator/qnx/qnx_memory_validator.h"
#include "score/analysis/tracing/common/interface_types/types.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"

#include <fcntl.h>

namespace score
{
namespace analysis
{
namespace tracing
{

MemoryValidator::MemoryValidator(std::unique_ptr<const os::qnx::MmanQnx> mman)
    : IMemoryValidator{}, mman_{std::move(mman)}
{
}

auto MemoryValidator::IsSharedMemoryTyped(const std::int32_t file_descriptor) const noexcept -> Result<bool>
{
    if (file_descriptor < 0)
    {
        return MakeUnexpected(ErrorCode::kBadFileDescriptorFatal);
    }

    const auto flags = mman_->shm_ctl(file_descriptor, SHMCTL_GET_FLAGS, 0U, 0U);
    if (!flags.has_value())
    {
        return MakeUnexpected(ErrorCode::kSharedMemoryObjectFlagsRetrievalFailedFatal);
    }
    return ((static_cast<std::uint32_t>(flags.value()) & static_cast<std::uint32_t>(SHMCTL_PHYS)) != 0U);
}

auto MemoryValidator::IsSharedMemoryTyped(const std::string& path) const noexcept -> Result<bool>
{
    auto file_descriptor = GetFileDescriptorFromMemoryPath(path);

    if (!file_descriptor.has_value())
    {
        return MakeUnexpected<bool>(file_descriptor.error());
    }
    return IsSharedMemoryTyped(file_descriptor.value());
}

auto MemoryValidator::GetFileDescriptorFromMemoryPath(const std::string& path) const noexcept -> Result<std::int32_t>
{
    if (path.empty())
    {
        return MakeUnexpected(ErrorCode::kInvalidArgumentFatal);
    }

    const auto shm_open_result =
        // No harm from implicit integral conversion here
        // coverity[autosar_cpp14_m5_0_4_violation]
        mman_->shm_open(path.c_str(), kSharedMemoryObjectOpenFlags, kSharedMemoryObjectOpenModes);
    if (!shm_open_result.has_value())
    {
        return MakeUnexpected(ErrorCode::kBadFileDescriptorFatal);
    }

    return shm_open_result.value();
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
