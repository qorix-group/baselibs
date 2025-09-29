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

#ifndef SCORE_LIB_RESULT_ERROR_CODE_CPP_H
#define SCORE_LIB_RESULT_ERROR_CODE_CPP_H

#include "score/result/result.h"
#include "score/result/result_cxx.h"

#include <rust/cxx.h>

#include <cstdint>
#include <memory>

using IntResult = ::score::Result<std::int32_t>;

struct OpaqueInt
{
    explicit OpaqueInt(std::int32_t v) : value(v) {}
    std::int32_t value;
    [[nodiscard]] std::int32_t get_i32() const
    {
        return value;
    }
};

using UniquePtrResult = ::score::Result<std::unique_ptr<OpaqueInt>>;

IntResult ExecuteCppFunction(rust::Str input);
UniquePtrResult CreateUniquePtrResult();

#endif  // SCORE_LIB_RESULT_ERROR_CODE_CPP_H
