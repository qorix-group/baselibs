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
#ifndef GENERIC_TRACE_API_META_INFO_VARIANTS_H
#define GENERIC_TRACE_API_META_INFO_VARIANTS_H

#include "ara_com_meta_info.h"
#include "dlt_meta_info.h"
#include <score/variant.hpp>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief MetaInfoVariants class
class MetaInfoVariants
{
  public:
    /// @brief Datatype used to store meta info data in std::variant
    using Type = score::cpp::variant<AraComMetaInfo, DltMetaInfo>;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_META_INFO_VARIANTS_H
