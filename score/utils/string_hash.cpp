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
#include "score/utils/string_hash.h"

namespace score
{
namespace common
{

std::size_t get_string_as_hash(const std::string& name)
{
    std::hash<std::string> hash_fn;
    return hash_fn(name);
}

}  // namespace common
}  // namespace score
