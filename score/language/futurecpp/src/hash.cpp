/********************************************************************************
 * Copyright (c) 2023, Contributors to the Eclipse Foundation
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

///
/// @copyright Copyright (c) 2023, Contributors to the Eclipse Foundation
///

#include <score/hash.hpp>

namespace score::cpp
{
namespace detail
{

constexpr std::uint32_t fnv_params<4U>::prime;
constexpr std::uint32_t fnv_params<4U>::offset_basis;

constexpr std::uint64_t fnv_params<8U>::prime;
constexpr std::uint64_t fnv_params<8U>::offset_basis;

} // namespace detail
} // namespace score::cpp
