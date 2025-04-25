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
#ifndef GENERIC_TRACE_API_TRACE_POINT_TYPE_H
#define GENERIC_TRACE_API_TRACE_POINT_TYPE_H

#include <cstdint>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief TracePointType enumerator
enum class TracePointType : std::uint8_t
{
    SKEL_EVENT_SND = 0,
    SKEL_EVENT_SND_A = 1,
    SKEL_EVENT_SUB = 2,
    SKEL_FIELD_UPD = 3,
    SKEL_FIELD_SET_CALL = 4,
    SKEL_FIELD_SET_CALL_RESULT = 5,
    SKEL_FIELD_SUB = 6,
    SKEL_METHOD_CALL = 7,
    SKEL_METHOD_CALL_RESULT_OK = 8,
    SKEL_METHOD_CALL_RESULT_ERROR = 9,
    SKEL_EVENT_UNSUB = 10,
    SKEL_FIELD_UPD_A = 11,
    SKEL_FIELD_GET_CALL = 12,
    SKEL_FIELD_GET_CALL_RESULT = 13,
    SKEL_FIELD_UNSUB = 14,
    PROXY_EVENT_SUB = 128,
    PROXY_EVENT_UNSUB = 129,
    PROXY_EVENT_SET_RECHDL = 130,
    PROXY_EVENT_RECHDL = 131,
    PROXY_EVENT_UNSET_RECHDL = 132,
    PROXY_EVENT_SET_CHGHDL = 133,
    PROXY_EVENT_CHGHDL = 134,
    PROXY_EVENT_GET_SAMPLES = 135,
    PROXY_EVENT_SAMPLE_CB = 136,
    PROXY_FIELD_SUB = 137,
    PROXY_FIELD_UNSUB = 138,
    PROXY_FIELD_SET_RECHDL = 139,
    PROXY_FIELD_RECHDL = 140,
    PROXY_FIELD_UNSET_RECHDL = 141,
    PROXY_FIELD_SET_CHGHDL = 142,
    PROXY_FIELD_CHGHDL = 143,
    PROXY_FIELD_GET_SAMPLES = 144,
    PROXY_FIELD_SAMPLE_CB = 145,
    PROXY_FIELD_SET = 146,
    PROXY_FIELD_SET_RESULT = 147,
    PROXY_FIELD_GET = 148,
    PROXY_FIELD_GET_RESULT = 149,
    PROXY_METHOD_CALL = 150,
    PROXY_METHOD_CALL_RESULT_OK = 151,
    PROXY_METHOD_CALL_RESULT_ERROR = 152,
    PROXY_EVENT_SUBSTATE_CHANGE = 153,
    PROXY_EVENT_UNSET_CHGHDL = 154,
    PROXY_FIELD_SUBSTATE_CHANGE = 155,
    PROXY_FIELD_UNSET_CHGHDL = 156,
    TRACE_POINT_UNDEFINED = 255
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_TRACE_POINT_TYPE_H
