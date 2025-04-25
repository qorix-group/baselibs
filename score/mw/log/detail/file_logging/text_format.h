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
#ifndef SCORE_MW_LOG_DETAIL_ASCII_FORMAT_H_
#define SCORE_MW_LOG_DETAIL_ASCII_FORMAT_H_

#include "score/mw/log/detail/integer_representation.h"
#include "score/mw/log/detail/verbose_payload.h"
#include "score/mw/log/log_types.h"

#include <string_view>

#include <bitset>
#include <cstdint>
#include <type_traits>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

/// \brief TextFormat Class used to format different data types to form a text log
///
/// \detail Used by TextMessageBuilder to build header and with (Stdout)Recorder to build payload
class TextFormat
{
  public:
    static void Log(VerbosePayload&, const bool) noexcept;
    static void Log(VerbosePayload&,
                    const std::uint8_t,
                    const IntegerRepresentation = IntegerRepresentation::kDecimal) noexcept;
    static void Log(VerbosePayload&,
                    const std::uint16_t,
                    const IntegerRepresentation = IntegerRepresentation::kDecimal) noexcept;
    static void Log(VerbosePayload&,
                    const std::uint32_t,
                    const IntegerRepresentation = IntegerRepresentation::kDecimal) noexcept;
    static void Log(VerbosePayload&,
                    const std::uint64_t,
                    const IntegerRepresentation = IntegerRepresentation::kDecimal) noexcept;
    static void Log(VerbosePayload&,
                    const std::int8_t,
                    const IntegerRepresentation = IntegerRepresentation::kDecimal) noexcept;
    static void Log(VerbosePayload&,
                    const std::int16_t,
                    const IntegerRepresentation = IntegerRepresentation::kDecimal) noexcept;
    static void Log(VerbosePayload&,
                    const std::int32_t,
                    const IntegerRepresentation = IntegerRepresentation::kDecimal) noexcept;
    static void Log(VerbosePayload&,
                    const std::int64_t,
                    const IntegerRepresentation = IntegerRepresentation::kDecimal) noexcept;
    static void Log(VerbosePayload&, const LogHex8) noexcept;
    static void Log(VerbosePayload&, const LogHex16) noexcept;
    static void Log(VerbosePayload&, const LogHex32) noexcept;
    static void Log(VerbosePayload&, const LogHex64) noexcept;
    static void Log(VerbosePayload&, const LogBin8) noexcept;
    static void Log(VerbosePayload&, const LogBin16) noexcept;
    static void Log(VerbosePayload&, const LogBin32) noexcept;
    static void Log(VerbosePayload&, const LogBin64) noexcept;
    static void Log(VerbosePayload&, const float) noexcept;
    static void Log(VerbosePayload&, const double) noexcept;
    static void Log(VerbosePayload&, const std::string_view) noexcept;
    static void Log(VerbosePayload&, const LogRawBuffer) noexcept;
    /// \brief Puts formatted time e.g. "2021/03/17 15:19:20.4360057 551684554"
    static void PutFormattedTime(VerbosePayload& payload) noexcept;
    /// \brief Puts '\n' character at the end of log
    static void TerminateLog(VerbosePayload& payload) noexcept;
};

//  Visibility of this funcation is extended because of coverage requirement
std::size_t FormattingFunctionReturnCast(const std::int32_t i) noexcept;

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  //  SCORE_MW_LOG_DETAIL_ASCII_FORMAT_H_
