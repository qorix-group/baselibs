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
#ifndef SCORE_LIB_OS_CPUID_H
#define SCORE_LIB_OS_CPUID_H

#include "score/os/ObjectSeam.h"

#include <cstdint>

namespace score
{
namespace os
{

class CpuId : public ObjectSeam<CpuId>
{
  public:
    /// \brief thread-safe singleton accessor
    /// \return Either concrete OS-dependent instance or respective set mock instance
    static CpuId& instance() noexcept;

    virtual void cpuid(std::uint32_t leaf,
                       std::uint32_t& eax,
                       std::uint32_t& ebx,
                       std::uint32_t& ecx,
                       std::uint32_t& edx) const noexcept = 0;

    virtual ~CpuId() = default;

  protected:
    CpuId() = default;
    CpuId(const CpuId&) = default;
    CpuId(CpuId&&) = default;
    CpuId& operator=(const CpuId&) = default;
    CpuId& operator=(CpuId&&) = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_CPUID_H
