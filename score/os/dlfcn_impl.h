/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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

#ifndef SCORE_LIB_OS_DLFCN_IMPL_H
#define SCORE_LIB_OS_DLFCN_IMPL_H

#include "score/os/dlfcn.h"

namespace score
{
namespace os
{

class DlfcnImpl final : public Dlfcn
{
  public:
    constexpr DlfcnImpl() = default;

    score::cpp::expected<void*, Error> dlopen(const char* const file_name, const DlOpenFlag flags) const noexcept override;
    score::cpp::expected<void*, Error> dlsym(void* const handle, const char* const symbol_name) const noexcept override;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_DLFCN_IMPL_H
