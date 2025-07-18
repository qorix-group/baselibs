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
#ifndef GENERIC_TRACE_API_DLT_PROPERTIES_H
#define GENERIC_TRACE_API_DLT_PROPERTIES_H

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief DltProperties struct
///
/// Struct used to store properties specifed to Dlt protocol
struct DltProperties
{
    friend bool operator==(const DltProperties&, const DltProperties&) noexcept
    {
        return true;
    }
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_DLT_PROPERTIES_H
