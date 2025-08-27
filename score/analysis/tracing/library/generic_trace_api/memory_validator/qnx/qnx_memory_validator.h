#ifndef SCORE_ANALYSIS_TRACING_LIB_GENERIC_TRACE_API_QNX_MEMORY_VALIDATOR_H
#define SCORE_ANALYSIS_TRACING_LIB_GENERIC_TRACE_API_QNX_MEMORY_VALIDATOR_H

#include "score/analysis/tracing/library/generic_trace_api/memory_validator/i_memory_validator.h"
#include "score/os/qnx/mman_impl.h"

namespace score
{
namespace analysis
{
namespace tracing
{

// No harm here since we have memory validator for both QNX and linux, m3_2_2, m3_2_3
// coverity[autosar_cpp14_m3_2_2_violation]
// coverity[autosar_cpp14_m3_2_3_violation]
class MemoryValidator : public IMemoryValidator
{
  public:
    explicit MemoryValidator(
        std::unique_ptr<const os::qnx::MmanQnx> mman = std::make_unique<const os::qnx::MmanQnxImpl>());
    ~MemoryValidator() override = default;
    MemoryValidator(const MemoryValidator&) = delete;
    MemoryValidator& operator=(const MemoryValidator&) = delete;
    MemoryValidator(MemoryValidator&&) noexcept = delete;
    MemoryValidator& operator=(MemoryValidator&&) noexcept = delete;

    auto IsSharedMemoryTyped(const std::int32_t file_descriptor) const noexcept -> Result<bool> override;
    auto IsSharedMemoryTyped(const std::string& path) const noexcept -> Result<bool> override;
    auto GetFileDescriptorFromMemoryPath(const std::string& path) const noexcept -> Result<std::int32_t> override;

  private:
    std::unique_ptr<const os::qnx::MmanQnx> mman_;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_LIB_GENERIC_TRACE_API_QNX_MEMORY_VALIDATOR_H
