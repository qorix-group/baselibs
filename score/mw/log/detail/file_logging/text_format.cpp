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
#include "score/mw/log/detail/file_logging/text_format.h"

#include "score/memory/string_literal.h"

#include "score/assert.hpp"
#include "score/span.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <type_traits>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

namespace
{
constexpr size_t kNumberOfBitsInByte = 8U;
constexpr size_t kTwoNibblesPerByte = 2U;
constexpr size_t kReserveSpaceForSpace = 1U;

template <typename T>
std::size_t GetBufferSizeCasted(T buffer_size) noexcept
{
    //  We only intend to use conversion function with human readable messages
    //  plus final memory management method will be avoiding dynamic allocation
    //  which limits maximum buffer size
    static_assert(sizeof(T) <= sizeof(std::size_t), "Buffer size conversion error");
    return static_cast<std::size_t>(buffer_size);
}

std::size_t GetSpanSizeCasted(const score::cpp::v1::span<Byte> buffer) noexcept
{
    return GetBufferSizeCasted(buffer.size());
}

template <typename T, IntegerRepresentation I>
struct GetFormatSpecifier
{
};

//  to reuse the same mechanism and not to overcompilicate the code with additional template complexity,
//  IntegerRepresentation is still used for float and double types:
template <>
struct GetFormatSpecifier<const float, IntegerRepresentation::kDecimal>
{
    static constexpr score::StringLiteral value = "%f ";
};

template <>
struct GetFormatSpecifier<const double, IntegerRepresentation::kDecimal>
{
    static constexpr score::StringLiteral value = "%f ";
};

template <>
struct GetFormatSpecifier<const std::uint8_t, IntegerRepresentation::kHex>
{
    static constexpr score::StringLiteral value = "%hhx ";
};

template <>
struct GetFormatSpecifier<const std::uint8_t, IntegerRepresentation::kOctal>
{
    static constexpr score::StringLiteral value = "%hho ";
};

template <>
struct GetFormatSpecifier<const std::uint8_t, IntegerRepresentation::kDecimal>
{
    static constexpr score::StringLiteral value = "%hhu ";
};

template <>
struct GetFormatSpecifier<const std::uint16_t, IntegerRepresentation::kDecimal>
{
    static constexpr score::StringLiteral value = "%hu ";
};

template <>
struct GetFormatSpecifier<const std::uint16_t, IntegerRepresentation::kHex>
{
    static constexpr score::StringLiteral value = "%hx ";
};

template <>
struct GetFormatSpecifier<const std::uint16_t, IntegerRepresentation::kOctal>
{
    static constexpr score::StringLiteral value = "%ho ";
};

template <>
struct GetFormatSpecifier<const std::uint32_t, IntegerRepresentation::kDecimal>
{
    static constexpr score::StringLiteral value = "%u ";
};

template <>
struct GetFormatSpecifier<const std::uint32_t, IntegerRepresentation::kHex>
{
    static constexpr score::StringLiteral value = "%x ";
};

template <>
struct GetFormatSpecifier<const std::uint32_t, IntegerRepresentation::kOctal>
{
    static constexpr score::StringLiteral value = "%o ";
};

template <>
struct GetFormatSpecifier<const std::uint64_t, IntegerRepresentation::kDecimal>
{
    static constexpr score::StringLiteral value = "%lu ";
};

template <>
struct GetFormatSpecifier<const std::uint64_t, IntegerRepresentation::kHex>
{
    static constexpr score::StringLiteral value = "%lx ";
};

template <>
struct GetFormatSpecifier<const std::uint64_t, IntegerRepresentation::kOctal>
{
    static constexpr score::StringLiteral value = "%lo ";
};

template <>
struct GetFormatSpecifier<const std::int8_t, IntegerRepresentation::kDecimal>
{
    static constexpr score::StringLiteral value = "%hhi ";
};

template <>
struct GetFormatSpecifier<const std::int16_t, IntegerRepresentation::kDecimal>
{
    static constexpr score::StringLiteral value = "%hi ";
};

template <>
struct GetFormatSpecifier<const std::int32_t, IntegerRepresentation::kDecimal>
{
    static constexpr score::StringLiteral value = "%i ";
};

template <>
struct GetFormatSpecifier<const std::int64_t, IntegerRepresentation::kDecimal>
{
    static constexpr score::StringLiteral value = "%li ";
};

template <IntegerRepresentation I, typename T>
static void PutFormattedNumber(VerbosePayload& payload, const T data) noexcept
{
    std::ignore = payload.Put([data](const score::cpp::v1::span<Byte> buffer) noexcept {
        const auto buffer_space = GetSpanSizeCasted(buffer);
        if (buffer_space > 0U)  // LCOV_EXCL_BR_LINE: lcov complains about lots of uncovered branches here, it is not
                                // convenient/related to this condition.
        {
            constexpr score::StringLiteral format = GetFormatSpecifier<const T, I>::value;
            const auto written =
                // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg) safe to use std::snprintf
                FormattingFunctionReturnCast(std::snprintf(buffer.data(), GetSpanSizeCasted(buffer), format, data));

            const std::size_t last_index = std::min(written, GetSpanSizeCasted(buffer) - 1U);
            // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
            // False positive: Pointer arithmetic is used on span which is an array
            // coverity[autosar_cpp14_m5_0_15_violation]
            buffer.data()[last_index] = ' ';
            // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
            return written;
        }
        else
        {
            return 0UL;
        }
    });
}

template <typename T>
using is_formatting_supported = std::is_unsigned<T>;

template <typename T, typename std::enable_if_t<is_formatting_supported<T>::value == false, bool> = true>
// PutBinaryFormattedNumber is not static storage. function is used to log different types
// coverity[autosar_cpp14_a2_10_4_violation]
void PutBinaryFormattedNumber(VerbosePayload&, const T) noexcept
{
    // empty function to satisfy compiler for unsupported types
    // no actions are expected here
}

template <typename T, typename std::enable_if_t<is_formatting_supported<T>::value, bool> = true>
// PutBinaryFormattedNumber is not static storage. function is used to log different types
// coverity[autosar_cpp14_a2_10_4_violation]
void PutBinaryFormattedNumber(VerbosePayload& payload, const T data) noexcept
{
    constexpr auto characters_used = kNumberOfBitsInByte * sizeof(T) + kReserveSpaceForSpace;
    std::ignore = payload.Put(
        [data](const score::cpp::v1::span<Byte> buffer) noexcept {
            const auto buffer_space = GetSpanSizeCasted(buffer);
            if (buffer_space > 1U)  // LCOV_EXCL_BR_LINE: lcov complains about lots of uncovered branches here, it is
                                    // not convenient/related to this condition.
            {
                constexpr auto number_of_bits = kNumberOfBitsInByte * sizeof(T);
                const auto max_possible = std::min(number_of_bits, buffer_space);
                std::size_t buffer_index = 0U;
                // LCOV_EXCL_BR_START: The loop condition is always true because buffer_index starts at 0 and
                // max_possible is always greater than 1 under this condition if (buffer_space > 1U). And there is no
                // way to make it false. Therefore, we can safely suppress the coverage warning for this decision.
                while (buffer_index < max_possible)  //  only MSB bits will be filled in case of insufficient memory
                // LCOV_EXCL_BR_STOP
                {
                    const auto bits = std::bitset<number_of_bits>(data);
                    // bit [] access can only have 0 or 1 return value and even if offset by value of '0'
                    // is not able to overflow char
                    // static_cast<std::int32_t> is needed to perform addition '0' char value to bool value from
                    // std::bitset static_cast<std::string::value_type> because this is the type of the content of our
                    // buffer
                    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
                    // False positive: Pointer arithmetic is used on span which is an array
                    // bit [] access can only have 0 or 1 return value and even if offset by value of '0'
                    // is not able to overflow char
                    // static_cast<std::int32_t> is needed to perform addition '0' char value to bool value from
                    // std::bitset static_cast<std::string::value_type> because this is the type of the content of our
                    // buffer
                    // coverity[autosar_cpp14_m5_0_15_violation]
                    // coverity[autosar_cpp14_m4_5_1_violation]
                    // coverity[autosar_cpp14_m5_0_8_violation]
                    // coverity[autosar_cpp14_m4_5_3_violation]
                    buffer.data()[buffer_index] = static_cast<std::string::value_type>(
                        // static_cast<std::int32_t> is needed to perform addition '0' char value to bool
                        // buffer_index is limited to maximum amount of bits in data (checked in while condition)
                        // coverity[autosar_cpp14_a4_7_1_violation]
                        // coverity[autosar_cpp14_m4_5_1_violation]
                        // coverity[autosar_cpp14_m5_0_6_violation]
                        static_cast<std::int32_t>('0') + bits[bits.size() - 1U - buffer_index]);
                    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
                    buffer_index++;
                }

                const std::size_t last_index = buffer_space - 1U;
                // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
                // False positive: Pointer arithmetic is used on span which is an array
                // coverity[autosar_cpp14_m5_0_15_violation]
                buffer.data()[last_index] = ' ';
                // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
                return buffer_index + kReserveSpaceForSpace;
            }
            else
            {
                return 0UL;
            }
        },
        characters_used);
}

template <typename T, typename std::enable_if_t<is_formatting_supported<T>::value == false, bool> = true>
// PutHexFormattedNumber is not static storage. function is used to split supported/unsupported types
// coverity[autosar_cpp14_a2_10_4_violation]
void PutHexFormattedNumber(VerbosePayload&, const T) noexcept
{
    // empty function to satisfy compiler for unsupported types
    // no actions are expected here
}

template <typename T, typename std::enable_if_t<is_formatting_supported<T>::value, bool> = true>
// PutHexFormattedNumber is not static storage. function is used to split supported/unsupported types
// coverity[autosar_cpp14_a2_10_4_violation]
void PutHexFormattedNumber(VerbosePayload& payload, const T data) noexcept
{
    PutFormattedNumber<IntegerRepresentation::kHex>(payload, data);
}

template <typename T, typename std::enable_if_t<is_formatting_supported<T>::value == false, bool> = true>
// PutOctalFormattedNumber is not static storage. function is used to split supported/unsupported types
// coverity[autosar_cpp14_a2_10_4_violation]
void PutOctalFormattedNumber(VerbosePayload&, const T) noexcept
{
    // empty function to satisfy compiler for unsupported types
    // no actions are expected here
}

template <typename T, typename std::enable_if_t<is_formatting_supported<T>::value, bool> = true>
// PutOctalFormattedNumber is not static storage. function is used to split supported/unsupported types
// coverity[autosar_cpp14_a2_10_4_violation]
void PutOctalFormattedNumber(VerbosePayload& payload, const T data) noexcept
{
    PutFormattedNumber<IntegerRepresentation::kOctal>(payload, data);
}

}  // namespace

std::size_t FormattingFunctionReturnCast(const std::int32_t i) noexcept
{
    if (i > 0)
    {
        return GetBufferSizeCasted(i);
    }
    return 0U;
}

void TextFormat::PutFormattedTime(VerbosePayload& payload) noexcept
{
    const auto time_point = std::chrono::system_clock::now();
    std::ignore = payload.Put([time_point](const score::cpp::v1::span<Byte> buffer) noexcept {
        std::size_t total = 0U;
        const auto now = std::chrono::system_clock::to_time_t(time_point);
        struct tm time_structure_buffer
        {
        };
        const struct tm* const time_structure =
            localtime_r(&now, &time_structure_buffer);  // LCOV_EXCL_BR_LINE: there are no branches to be covered.

        if (nullptr != time_structure)  // LCOV_EXCL_BR_LINE: "nullptr" condition can't be controlled via test case.
        {
            const std::size_t buffer_space = GetSpanSizeCasted(buffer);

            std::stringstream ss_time_str;
            ss_time_str << std::put_time(&time_structure_buffer, "%Y/%m/%d %H:%M:%S.");
            const std::string& time_str = ss_time_str.str();

            if (buffer_space > time_str.length())
            {
                std::ignore = std::copy(time_str.begin(), time_str.end(), buffer.begin());
                total = time_str.length();
            }
        }

        return total;
    });

    const auto time_elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(time_point.time_since_epoch()).count() % 10'000'000;

    // time_elapsed values are within the scope from 0 to 9'999'999 (after % 10'000'000 operation)
    // coverity[autosar_cpp14_a4_7_1_violation]
    const auto time_structure_elapsed = static_cast<std::uint32_t>(time_elapsed);

    PutFormattedNumber<IntegerRepresentation::kDecimal>(payload, time_structure_elapsed);
}

template <typename T>
// LogData function uses Put*Number functions, some of them are also templates
// Name of Put*Number functions is reused to split implementation for supported/unsupported types
// coverity[autosar_cpp14_a2_10_4_violation]
static void LogData(VerbosePayload& payload,
                    const T data,
                    const IntegerRepresentation integral_representation = IntegerRepresentation::kDecimal) noexcept
{
    switch (integral_representation)
    {
        case IntegerRepresentation::kHex:
            PutHexFormattedNumber(payload, data);
            break;
        case IntegerRepresentation::kBinary:
            PutBinaryFormattedNumber(payload, data);
            break;
        case IntegerRepresentation::kOctal:
            PutOctalFormattedNumber(payload, data);
            break;
        case IntegerRepresentation::kDecimal:
        default:
            PutFormattedNumber<IntegerRepresentation::kDecimal>(payload, data);
            break;
    }
}

void TextFormat::Log(VerbosePayload& payload, const bool data) noexcept
{
    constexpr auto positive_value = std::string_view{"True"};
    constexpr auto negative_value = std::string_view{"False"};

    const auto data_value = data ? positive_value : negative_value;
    score::mw::log::detail::TextFormat::Log(payload, data_value);
}

void TextFormat::Log(VerbosePayload& payload,
                     const std::uint8_t data,
                     const IntegerRepresentation integral_representation) noexcept
{
    score::mw::log::detail::LogData(payload, data, integral_representation);
}

void score::mw::log::detail::TextFormat::Log(VerbosePayload& payload,
                                           const std::uint16_t data,
                                           const IntegerRepresentation integral_representation) noexcept
{
    score::mw::log::detail::LogData(payload, data, integral_representation);
}

void score::mw::log::detail::TextFormat::Log(VerbosePayload& payload,
                                           const std::uint32_t data,
                                           const IntegerRepresentation integral_representation) noexcept
{
    score::mw::log::detail::LogData(payload, data, integral_representation);
}

void score::mw::log::detail::TextFormat::Log(VerbosePayload& payload,
                                           const std::uint64_t data,
                                           const IntegerRepresentation integral_representation) noexcept
{
    score::mw::log::detail::LogData(payload, data, integral_representation);
}

void score::mw::log::detail::TextFormat::Log(VerbosePayload& payload,
                                           const std::int8_t data,
                                           const IntegerRepresentation integral_representation) noexcept
{
    score::mw::log::detail::LogData(payload, data, integral_representation);
}

void score::mw::log::detail::TextFormat::Log(VerbosePayload& payload,
                                           const std::int16_t data,
                                           const IntegerRepresentation integral_representation) noexcept
{
    score::mw::log::detail::LogData(payload, data, integral_representation);
}

void score::mw::log::detail::TextFormat::Log(VerbosePayload& payload,
                                           const std::int32_t data,
                                           const IntegerRepresentation integral_representation) noexcept
{
    score::mw::log::detail::LogData(payload, data, integral_representation);
}

void score::mw::log::detail::TextFormat::Log(VerbosePayload& payload,
                                           const std::int64_t data,
                                           const IntegerRepresentation integral_representation) noexcept
{
    score::mw::log::detail::LogData(payload, data, integral_representation);
}

void score::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const LogHex8 data) noexcept
{
    score::mw::log::detail::LogData(payload, data.value, IntegerRepresentation::kHex);
}

void score::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const LogHex16 data) noexcept
{
    score::mw::log::detail::LogData(payload, data.value, IntegerRepresentation::kHex);
}

void score::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const LogHex32 data) noexcept
{
    score::mw::log::detail::LogData(payload, data.value, IntegerRepresentation::kHex);
}

void score::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const LogHex64 data) noexcept
{
    score::mw::log::detail::LogData(payload, data.value, IntegerRepresentation::kHex);
}

void score::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const LogBin8 data) noexcept
{
    score::mw::log::detail::LogData(payload, data.value, IntegerRepresentation::kBinary);
}

void score::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const LogBin16 data) noexcept
{
    score::mw::log::detail::LogData(payload, data.value, IntegerRepresentation::kBinary);
}

void score::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const LogBin32 data) noexcept
{
    score::mw::log::detail::LogData(payload, data.value, IntegerRepresentation::kBinary);
}

void score::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const LogBin64 data) noexcept
{
    score::mw::log::detail::LogData(payload, data.value, IntegerRepresentation::kBinary);
}

void score::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const float data) noexcept
{
    score::mw::log::detail::LogData(payload, data);
}

void score::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const double data) noexcept
{
    score::mw::log::detail::LogData(payload, data);
}

void score::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const std::string_view data) noexcept
{
    if (data.size() > 0U)
    {
        const std::size_t data_length = data.size() + kReserveSpaceForSpace;

        std::ignore = payload.Put(
            [data](const score::cpp::v1::span<Byte> buffer) {
                const std::size_t length = std::min(data.size(), GetSpanSizeCasted(buffer));
                if (length > 0U)  // LCOV_EXCL_BR_LINE: Cannot be covered in unit tests as 'data.size() > 0' and buffer
                                  // size is uncontrollable.
                {
                    // NOLINTNEXTLINE(score-banned-function) memcpy is needed to copy string_view data
                    std::ignore = std::memcpy(buffer.data(), data.data(), length);
                    // buffer size is checked in if condition above "if (length > 0U)"
                    // so calculating last_index is safe
                    // coverity[autosar_cpp14_a4_7_1_violation]
                    const std::size_t last_index = GetSpanSizeCasted(buffer) - 1U;
                    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
                    // False positive: Pointer arithmetic is used on span which is an array
                    // coverity[autosar_cpp14_m5_0_15_violation]
                    buffer.data()[last_index] = ' ';
                    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
                    return GetSpanSizeCasted(buffer);
                }
                return 0LU;  // LCOV_EXCL_LINE: Cannot be covered in unit tests as if (length > 0) is uncontrollable.
            },
            data_length);
    }
}

void score::mw::log::detail::TextFormat::TerminateLog(VerbosePayload& payload) noexcept
{
    payload.Put("\n", kReserveSpaceForSpace);
}

void score::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const LogRawBuffer data) noexcept
{
    const auto max_string_len = kTwoNibblesPerByte * GetBufferSizeCasted(data.size());

    if (max_string_len > 0U)
    {
        std::ignore = payload.Put(
            [&data](const score::cpp::v1::span<Byte> buffer) noexcept {
                if (buffer.size() == 0)
                {
                    return 0L;
                }
                const auto destination_begin_reference = buffer.begin();
                const auto destination_end_reference = buffer.end();
                auto destination_iterator = destination_begin_reference;
                std::ignore =
                    std::for_each(data.begin(),
                                  data.end(),
                                  [&destination_iterator, destination_end_reference = buffer.end()](const auto input) {
                                      if (destination_iterator != destination_end_reference)
                                      {
                                          std::array<char, 3> temporary_formatting_buffer{};
                                          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg) safe to use std::snprintf
                                          std::ignore = std::snprintf(temporary_formatting_buffer.begin(),
                                                                      temporary_formatting_buffer.size(),
                                                                      "%02hhx",
                                                                      input);
                                          //  Take first two characters from temporary formatting buffer:
                                          destination_iterator =
                                              std::copy_n(temporary_formatting_buffer.begin(), 2, destination_iterator);
                                      }
                                  });
                if (destination_iterator != destination_end_reference)
                {
                    constexpr std::array<char, 1UL> kSpace({' '});
                    destination_iterator = std::copy(kSpace.begin(), kSpace.end(), destination_iterator);
                }
                return std::distance(destination_begin_reference, destination_iterator);
            },
            max_string_len + kReserveSpaceForSpace);
    }
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
