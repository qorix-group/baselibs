#ifndef SCORE_ANALYSIS_TRACING_LIB_GENERIC_TRACE_API_I_MEMORY_VALIDATOR_H
#define SCORE_ANALYSIS_TRACING_LIB_GENERIC_TRACE_API_I_MEMORY_VALIDATOR_H

#include "score/result/result.h"

namespace score
{
namespace analysis
{
namespace tracing
{

class IMemoryValidator
{
  public:
    IMemoryValidator() noexcept = default;
    virtual ~IMemoryValidator() = default;
    IMemoryValidator(const IMemoryValidator&) = delete;
    IMemoryValidator(IMemoryValidator&&) noexcept = delete;
    IMemoryValidator& operator=(const IMemoryValidator&) = delete;
    IMemoryValidator& operator=(IMemoryValidator&&) noexcept = delete;

    virtual auto IsSharedMemoryTyped(const std::int32_t file_descriptor) const noexcept -> Result<bool> = 0;
    virtual auto IsSharedMemoryTyped(const std::string& path) const noexcept -> Result<bool> = 0;
    virtual auto GetFileDescriptorFromMemoryPath(const std::string& path) const noexcept -> Result<std::int32_t> = 0;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_LIB_GENERIC_TRACE_API_I_MEMORY_VALIDATOR_H
