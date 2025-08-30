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
#include "score/mw/log/runtime.h"

#include "score/mw/log/detail/empty_recorder.h"
#include "score/mw/log/detail/thread_local_guard.h"
#include "score/mw/log/irecorder_factory.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

Runtime& Runtime::Instance(Recorder* const recorder, score::cpp::pmr::memory_resource* memory_resource) noexcept
{
    /*
    Deviation from Rule A3-3-2:
    - Static and thread-local objects shall be constant-initialized.
    Justification:
    - This is intentional to apply singleton pattern,
      and 'runtime' is safely initialized on first use and is thread-safe due to C++11 static initialization guarantees.
    */
    // coverity[autosar_cpp14_a3_3_2_violation]
    static Runtime runtime{recorder, memory_resource};

    return runtime;
}

Runtime::Runtime(Recorder* const recorder, score::cpp::pmr::memory_resource* memory_resource) noexcept
{
    ThreadLocalGuard guard{};
    const auto recorder_factory = CreateRecorderFactory();
    if (recorder == nullptr)
    {
        default_recorder_ = recorder_factory->CreateFromConfiguration(memory_resource);
    }
    else
    {
        default_recorder_ = recorder_factory->CreateWithConsoleLoggingOnly(memory_resource);
        recorder_instance_ = recorder;
    }
}

score::mw::log::Recorder& Runtime::GetRecorder() noexcept
{
    const auto& instance = Instance(nullptr);
    if (instance.recorder_instance_ != nullptr)
    {
        return *instance.recorder_instance_;
    }
    if (instance.default_recorder_ != nullptr)
    {
        return *instance.default_recorder_;
    }
    //  Only as last resort, return static empty recorder:
    /*
    Deviation from Rule A3-3-2:
    - Static and thread-local objects shall be constant-initialized.
    Justification:
    - This is intentional to apply singleton pattern,
      and 'empty_recorder' is safely initialized on first use and is thread-safe due to C++11 static initialization
      guarantees.
    */
    // coverity[autosar_cpp14_a3_3_2_violation]
    static EmptyRecorder empty_recorder{};  // LCOV_EXCL_LINE : false positive
    return empty_recorder;
}

score::mw::log::Recorder& Runtime::GetFallbackRecorder() noexcept
{
    /*
    Deviation from Rule A3-3-2:
    - Static and thread-local objects shall be constant-initialized.
    Justification:
    - This is intentional to apply singleton pattern,
      and 'empty_recorder' is safely initialized on first use and is thread-safe due to C++11 static initialization
      guarantees.
    */
    // coverity[autosar_cpp14_a3_3_2_violation]
    static EmptyRecorder empty_recorder{};
    return empty_recorder;
}

score::mw::log::LoggerContainer& Runtime::GetLoggerContainer() noexcept
{
    return Instance(nullptr).logger_container_instance_;
}

void Runtime::SetRecorder(Recorder* const recorder, score::cpp::pmr::memory_resource* memory_resource) noexcept
{
    Instance(recorder, memory_resource).recorder_instance_ = recorder;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
