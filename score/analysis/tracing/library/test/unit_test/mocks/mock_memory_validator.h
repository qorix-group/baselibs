///
/// @file mock_memory_validator.h
/// @copyright Copyright (C) 2025, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Memory validator mock header file
///

#ifndef GENERIC_TRACE_API_MOCK_MEMORY_VALIDATOR_H
#define GENERIC_TRACE_API_MOCK_MEMORY_VALIDATOR_H

#include "score/analysis/tracing/library/generic_trace_api/memory_validator/i_memory_validator.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace analysis
{
namespace tracing
{

class MockMemoryValidator : public IMemoryValidator
{
  public:
    ~MockMemoryValidator() override = default;
    MOCK_METHOD((Result<bool>), IsSharedMemoryTyped, (const std::int32_t), (const, noexcept, override));
    MOCK_METHOD((Result<bool>), IsSharedMemoryTyped, (const std::string&), (const, noexcept, override));
    MOCK_METHOD((Result<std::int32_t>),
                GetFileDescriptorFromMemoryPath,
                (const std::string&),
                (const, noexcept, override));
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_MOCK_MEMORY_VALIDATOR_H
