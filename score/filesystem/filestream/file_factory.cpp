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
    OpenFlags flags{0U};
    if ((mode & std::ios_base::in) != 0)
    {
        if ((mode & std::ios_base::out) != 0)
        {
            flags = OpenFlags::kReadWrite;
        }
        else
        {
            flags = OpenFlags::kReadOnly;
        }
    }
    else if ((mode & std::ios_base::out) != 0)
    {
        flags = OpenFlags::kWriteOnly | OpenFlags::kCreate;
    }
    else
    {
        flags = static_cast<OpenFlags>(0U);
    }

    if ((mode & std::ios_base::app) != 0)
    {
        flags |= OpenFlags::kAppend | OpenFlags::kCreate;
    }
    if ((mode & std::ios_base::trunc) != 0)
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
    thread_local std::default_random_engine random_engine{
        static_cast<std::default_random_engine::result_type>(std::hash<std::thread::id>{}(std::this_thread::get_id()))};

    static_assert(NumDigits < 8U,
                  "max_num_digits must be less than 8");  // This prevents an overflow in the next line
    std::uniform_int_distribution<std::uint32_t> random_distribution{0U, (1U << (NumDigits * 4U)) - 1U};
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
        std::cerr << "Error opening file " << file_handle.error();
        return MakeUnexpected(ErrorCode::kCouldNotOpenFileStream);
    }
}

}  // namespace

Result<std::unique_ptr<std::iostream>> FileFactory::Open(const Path& path, const std::ios_base::openmode mode) noexcept
{
    return OpenFileHandle(path, mode, kDefaultMode).and_then([mode](int file_handle) {
        return CreateFileStream<details::StdioFileBuf>(file_handle, mode);
    });
}

Result<std::unique_ptr<FileStream>> FileFactory::AtomicUpdate(const Path& path,
                                                              const std::ios_base::openmode mode) noexcept
{
    if ((mode & ~(std::ios::out | std::ios::trunc)) != 0)
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
