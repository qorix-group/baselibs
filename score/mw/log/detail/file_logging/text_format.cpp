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
#include "score/mw/log/detail/file_logging/text_format.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

std::size_t GetSpanSizeCasted(const score::cpp::span<Byte> buffer) noexcept
{
    return GetBufferSizeCasted(buffer.size());
}

std::size_t FormattingFunctionReturnCast(const std::int32_t i) noexcept
{
    if (i > 0)
    {
        return GetBufferSizeCasted(i);
    }
    return 0U;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
