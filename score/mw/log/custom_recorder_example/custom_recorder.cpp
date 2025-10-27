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
#include "score/mw/log/custom_recorder_example/custom_recorder.h"
#include <iostream>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

score::cpp::optional<SlotHandle> CustomRecorder::StartRecord(const std::string_view, const LogLevel) noexcept
{
    SlotHandle slot_handle{};
    return slot_handle;
}

void CustomRecorder::StopRecord(const SlotHandle&) noexcept {}

void CustomRecorder::Log(const SlotHandle&, const bool) noexcept {}
void CustomRecorder::Log(const SlotHandle&, const std::uint8_t) noexcept {}
void CustomRecorder::Log(const SlotHandle&, const std::int8_t) noexcept {}
void CustomRecorder::Log(const SlotHandle&, const std::uint16_t) noexcept {}
void CustomRecorder::Log(const SlotHandle&, const std::int16_t) noexcept {}
void CustomRecorder::Log(const SlotHandle&, const std::uint32_t) noexcept {}
void CustomRecorder::Log(const SlotHandle&, const std::int32_t) noexcept {}
void CustomRecorder::Log(const SlotHandle&, const std::uint64_t) noexcept {}
void CustomRecorder::Log(const SlotHandle&, const std::int64_t) noexcept {}
void CustomRecorder::Log(const SlotHandle&, const float) noexcept {}
void CustomRecorder::Log(const SlotHandle&, const double) noexcept {}
void CustomRecorder::Log(const SlotHandle&, const std::string_view data) noexcept
{
    std::cout << __FUNCTION__ << " " << data << std::endl;
}

void CustomRecorder::Log(const SlotHandle&, const LogHex8) noexcept {}
void CustomRecorder::Log(const SlotHandle&, const LogHex16) noexcept {}
void CustomRecorder::Log(const SlotHandle&, const LogHex32) noexcept {}
void CustomRecorder::Log(const SlotHandle&, const LogHex64) noexcept {}

void CustomRecorder::Log(const SlotHandle&, const LogBin8) noexcept {}
void CustomRecorder::Log(const SlotHandle&, const LogBin16) noexcept {}
void CustomRecorder::Log(const SlotHandle&, const LogBin32) noexcept {}
void CustomRecorder::Log(const SlotHandle&, const LogBin64) noexcept {}

void CustomRecorder::Log(const SlotHandle&, const LogRawBuffer) noexcept {}

void CustomRecorder::Log(const SlotHandle&, const LogSlog2Message) noexcept {}

bool CustomRecorder::IsLogEnabled(const LogLevel&, const std::string_view) const noexcept
{
    return true;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
