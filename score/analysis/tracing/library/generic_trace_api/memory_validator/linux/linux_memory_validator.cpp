#include "score/analysis/tracing/library/generic_trace_api/memory_validator/linux/linux_memory_validator.h"

namespace score
{
namespace analysis
{
namespace tracing
{

MemoryValidator::MemoryValidator() noexcept : IMemoryValidator{} {}

auto MemoryValidator::IsSharedMemoryTyped(const std::int32_t) const noexcept -> Result<bool>
{
    return true;
}

auto MemoryValidator::IsSharedMemoryTyped(const std::string&) const noexcept -> Result<bool>
{
    return true;
}

auto MemoryValidator::GetFileDescriptorFromMemoryPath(const std::string&) const noexcept -> Result<std::int32_t>
{
    return 0;
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
