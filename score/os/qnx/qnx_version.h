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
#ifndef SCORE_LIB_OS_QNX_QNX_VERSION_H
#define SCORE_LIB_OS_QNX_QNX_VERSION_H

#include <cstdint>

#if __QNX__ < 800 && __has_include(<sys/nto_version.h>)
#include <sys/nto_version.h>
#endif

namespace detail
{
constexpr std::uint32_t get_qnx_version()
{
#if __QNX__ >= 800
    return __QNX__;
#else
    return _NTO_VERSION;
#endif
}
}  // namespace detail

inline constexpr std::uint32_t _SPP_QNX_VERSION_ = detail::get_qnx_version();

#endif  // SCORE_LIB_OS_QNX_QNX_VERSION_H
