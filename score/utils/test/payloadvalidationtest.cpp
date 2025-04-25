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
#include "score/utils/src/PayloadValidation.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

namespace score
{
namespace utils
{

TEST(PayloadValidation, PayloadValidationTestValidData)
{
    std::vector<std::uint8_t> payload{0x0A, 0x0B, 0x0C, 0x0D, 0x0E};
    EXPECT_FALSE(IsPayloadInvalid(payload));
}

TEST(PayloadValidation, PayloadValidationTestInvalidData)
{
    std::vector<std::uint8_t> payload{0x00, 0x00, 0x00, 0x00, 0x00};
    EXPECT_TRUE(IsPayloadInvalid(payload));
}

}  // namespace utils
}  // namespace score
