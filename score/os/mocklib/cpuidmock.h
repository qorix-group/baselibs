/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
#ifndef SCORE_LIB_OS_MOCKLIB_CPUID_H
#define SCORE_LIB_OS_MOCKLIB_CPUID_H

#include "score/os/cpuid.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class CpuIdMock : public CpuId
{
  public:
    MOCK_METHOD(void,
                cpuid,
                (std::uint32_t, std::uint32_t&, std::uint32_t&, std::uint32_t&, std::uint32_t&),
                (const, noexcept, override));

    void SetExpextedCallIsQemu();
    void SetExpextedCallIsHw();
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_CPUID_H
