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
#ifndef LIB_UTILS_SRC_PAYLOADVALIDATION_H
#define LIB_UTILS_SRC_PAYLOADVALIDATION_H

#include <algorithm>
#include <cstdint>

namespace score
{
namespace utils
{

/* KW_SUPPRESS_START:MISRA.ONEDEFRULE.VAR:constexpr integers are safe */
constexpr std::uint8_t DEFAULT_VALUE = 0x00U;
constexpr std::uint8_t INVALID_VALUE = 0xFFU;
/* KW_SUPPRESS_END:MISRA.ONEDEFRULE.VAR */

/**
 * @brief This function checks the byte if exist
 * in the given payload
 *
 * @param payload service data to be validated
 * @param compare_value Value to compare.
 * @return TRUE if it finds the invalid/default data else FALSE
 */
template <typename PayloadType>
bool CheckByte(const PayloadType& payload, const uint8_t& compare_value)
{
    return (std::all_of(payload.cbegin(), payload.cend(), [compare_value](const std::uint8_t byte) {
        return compare_value == byte;
    }));
}

/**
 * @brief as per BN_KOM_2658 and DMA_PA_5530, service data
 * 0xFFFFFFFF and 0x00000000 are invalid and default values
 * respectively. This function checks for the same in the
 * given payload
 *
 *
 * @param payload service data to be validated
 * @return TRUE if it finds the invalid/default data else FALSE
 */
template <typename PayloadType>
bool IsPayloadInvalid(const PayloadType& payload)
{
    return (CheckByte(payload, DEFAULT_VALUE) || CheckByte(payload, INVALID_VALUE));
}

}  // namespace utils
}  // namespace score

#endif  // LIB_UTILS_SRC_PAYLOADVALIDATION_H
