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
#include "score/filesystem/filestream/file_factory.h"

#include "score/filesystem/filestream/file_buf.h"
#include "score/filesystem/filestream/file_stream.h"

#include "score/os/fcntl.h"
#include "score/os/stdio.h"
#include "score/os/unistd.h"

#include <charconv>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <random>
#include <thread>

namespace score::filesystem
{
namespace
{

constexpr os::Stat::Mode kDefaultMode = os::Stat::Mode::kReadUser | os::Stat::Mode::kWriteUser |
                                        os::Stat::Mode::kReadGroup | os::Stat::Mode::kWriteGroup |
                                        os::Stat::Mode::kReadOthers | os::Stat::Mode::kWriteOthers;

using OpenFlags = os::Fcntl::Open;

OpenFlags IosOpenModeToOpenFlags(const std::ios_base::openmode mode) noexcept
{
    // The value won't be used as such, but rather will be initialized properly by the bit-wise operations below.
    // Also, the Enum does not provide a 0-valued element, that must be the starting point.
    // Therefore, initializing it to 0U is the simplest solution. We could do something like
    // OpenFlags flags{OpenFlags::kReadOnly & OpenFlags::kWriteOnly} but that would be a bit
    // overkill for this case and would depend on the current values;
    // coverity[autosar_cpp14_a7_2_1_violation]
    OpenFlags flags{0U};
    if ((mode & std::ios_base::in) != 0U)
    {
        if ((mode & std::ios_base::out) != 0U)
        {
            flags = OpenFlags::kReadWrite;
        }
        else
        {
            flags = OpenFlags::kReadOnly;
        }
    }
    else if ((mode & std::ios_base::out) != 0U)
    {
        flags = OpenFlags::kWriteOnly | OpenFlags::kCreate;
    }

    if ((mode & std::ios_base::app) != 0U)
    {
        flags |= OpenFlags::kAppend | OpenFlags::kCreate;
    }
    if ((mode & std::ios_base::trunc) != 0U)
    {
        flags |= OpenFlags::kTruncate | OpenFlags::kCreate;
    }
    return flags;
}

template <typename Buf, typename... Args>
Result<std::unique_ptr<FileStream>> CreateFileStream(Args&&... args)
{
    Buf filebuf{std::forward<Args>(args)...};
    if (!filebuf.is_open())
    {
        return MakeUnexpected(filesystem::ErrorCode::kCouldNotOpenFileStream);
    }
    else
    {
        return std::make_unique<details::FileStreamImpl<Buf>>(std::move(filebuf));
    }
}

template <std::uint32_t NumDigits>
Result<std::string> AppendRandomDigits(std::string str) noexcept
{
    auto seed = std::hash<std::thread::id>{}(std::this_thread::get_id());
    // The usage of an unique seed per thread is paramount to avoid collisions,
    // meaning that threads could write to the same file.
    // Notice that the engine is not default initialized, but rather seeded with the thread id.
    // coverity[autosar_cpp14_a3_3_2_violation]
    // coverity[autosar_cpp14_a26_5_2_violation]
    thread_local std::default_random_engine random_engine{static_cast<std::default_random_engine::result_type>(seed)};

    static_assert(
        NumDigits < 8U,
        "max_num_digits must be less than 8");  // This prevents usage with NumDigits that would lead to overflow.
    constexpr std::uint32_t shift = std::min(31U, NumDigits * 4U - 1U);
    // The shift operation is guaranteed to be bound, as we limit it's range from 0 to 31.
    // The result is immediately cast to the underlying type. Therefore, these two violations are false positives.
    // coverity[autosar_cpp14_m5_8_1_violation]
    // coverity[autosar_cpp14_m5_0_10_violation]
    constexpr std::uint32_t upper_bound = static_cast<std::uint32_t>(1U << shift);
    std::uniform_int_distribution<std::uint32_t> random_distribution{0U, upper_bound};
    // False positive, random_number lives until the end of the function, so it overlives the call to std::to_chars.
    // coverity[autosar_cpp14_m7_5_2_violation]
    auto random_number = random_distribution(random_engine);
    std::array<char, NumDigits> random_number_buffer{};
    const auto to_chars_result =
        std::to_chars(random_number_buffer.begin(), random_number_buffer.end(), random_number, 16);
    if (to_chars_result.ec == std::errc{})
    {
        str = str.append(random_number_buffer.begin(), to_chars_result.ptr);
        return str;
    }
    else
    {
        return MakeUnexpected(filesystem::ErrorCode::kCouldNotOpenFileStream);
    }
}

Result<int> OpenFileHandle(const Path& path,
                           const std::ios_base::openmode mode,
                           const os::Stat::Mode create_mode) noexcept
{
    const auto flags = IosOpenModeToOpenFlags(mode);
    // NOLINTNEXTLINE(score-banned-function): We need to use the POSIX open call to obtain a file descriptor.
    const auto file_handle = os::Fcntl::instance().open(path.CStr(), flags, create_mode);
    if (file_handle.has_value())
    {
        return *file_handle;
    }
    else
    {
        return MakeUnexpected(ErrorCode::kCouldNotOpenFileStream);
    }
}

}  // namespace

Result<std::unique_ptr<std::iostream>> FileFactory::Open(const Path& path, const std::ios_base::openmode mode)
{
    return OpenFileHandle(path, mode, kDefaultMode).and_then([mode](int file_handle) {
        return CreateFileStream<details::StdioFileBuf>(file_handle, mode);
    });
}

Result<std::unique_ptr<FileStream>> FileFactory::AtomicUpdate(const Path& path,
                                                              const std::ios_base::openmode mode) noexcept
{
    if ((mode & ~(std::ios::out | std::ios::trunc)) != 0U)
    {
        return MakeUnexpected(ErrorCode::kNotImplemented);
    }

    auto filename = path.Filename();
    std::string_view filename_view{filename.Native()};
    if (filename_view.empty())
    {
        return MakeUnexpected(filesystem::ErrorCode::kCouldNotOpenFileStream);
    }

    static constexpr std::uint32_t kNumDigits = 6U;
    std::string temp_filename;
    temp_filename.reserve(filename_view.size() + kNumDigits + 1U);  // add 1 for the leading '.'
    temp_filename.push_back('.');
    temp_filename = temp_filename.append(filename_view.begin(), filename_view.end());
    auto rand_filename = AppendRandomDigits<kNumDigits>(std::move(temp_filename));
    if (!rand_filename.has_value())
    {
        return MakeUnexpected<std::unique_ptr<FileStream>>(std::move(rand_filename).error());
    }

    auto temp_path = path.ParentPath();
    temp_path /= *rand_filename;

    if (auto file_handle = OpenFileHandle(temp_path, mode, kDefaultMode); file_handle.has_value())
    {
        return CreateFileStream<details::AtomicFileBuf>(*file_handle, mode, std::move(temp_path), path);
    }
    else
    {
        return MakeUnexpected<std::unique_ptr<FileStream>>(file_handle.error());
    }
}

}  // namespace score::filesystem
