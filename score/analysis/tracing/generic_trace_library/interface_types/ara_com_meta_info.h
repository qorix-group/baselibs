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
#ifndef GENERIC_TRACE_API_ARA_COM_META_INFO_H
#define GENERIC_TRACE_API_ARA_COM_META_INFO_H

#include "ara_com_properties.h"
#include "meta_info.h"

namespace score
{
namespace analysis
{
namespace tracing
{

using AraComMetaInfo = MetaInfo<AraComProperties>;

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_ARA_COM_META_INFO_H
