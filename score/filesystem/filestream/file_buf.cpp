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
#include "score/filesystem/filestream/file_buf.h"

#include "score/filesystem/error.h"
#include "score/os/fcntl.h"
#include "score/os/stdio.h"
#include "score/os/unistd.h"
#include "score/scope_exit/scope_exit.h"

namespace score::filesystem::details
{

namespace
{

ResultBlank SyncDirectory(const Path& dir_path)
{
    // NOLINTNEXTLINE(score-banned-function): We need to use the POSIX open call to obtain a file descriptor.
    auto dir_fd = os::Fcntl::instance().open(dir_path.CStr(), os::Fcntl::Open::kReadOnly | os::Fcntl::Open::kDirectory);
    if (!dir_fd.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotSyncDirectory);
    }

    auto fsync_result = os::Unistd::instance().fsync(*dir_fd);
    score::cpp::ignore = os::Unistd::instance().close(*dir_fd);

    if (!fsync_result.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotSyncDirectory);
    }

    return {};
}

}  // namespace

ResultBlank StdioFileBuf::Close()
{
    if (!is_open())
    {
        return {};
    }

    if (close() == nullptr)
    {
        return MakeUnexpected(ErrorCode::kCloseFailed, "Unable to close file descriptor file buffer");
    }

    return {};
}

AtomicFileBuf::AtomicFileBuf(int fd, std::ios::openmode mode, Path from_path, Path to_path)
    : StdioFileBuf{fd, mode}, from_path_{std::move(from_path)}, to_path_{std::move(to_path)}
{
}

AtomicFileBuf::~AtomicFileBuf()
{
    if (is_open())
    {
        score::cpp::ignore = os::Unistd::instance().unlink(from_path_.CStr());
    }
}

ResultBlank AtomicFileBuf::Close()
{
    if (!is_open())
    {
        return {};
    }

    // If any of the operations fail, remove the temporary file to avoid littering the file system
    // with garbage files. If unlink fails there is not much we can do about it anyway.
    utils::ScopeExit cleanup{[this]() {
        score::cpp::ignore = os::Unistd::instance().unlink(from_path_.CStr());
    }};

    if (sync() != 0)
    {
        return MakeUnexpected(ErrorCode::kFsyncFailed);
    }

    if (!os::Unistd::instance().fsync(fd()).has_value())
    {
        return MakeUnexpected(ErrorCode::kFsyncFailed);
    }

    if (close() == nullptr)
    {
        // If closing fails, do not try to rename since we might replace a working
        // file with a corrupted one.
        constexpr std::string_view user_message{"Unable to close file descriptor file buffer during atomic update"};
        return MakeUnexpected(ErrorCode::kCloseFailed, user_message);
    }

    if (auto rename_result = os::Stdio::instance().rename(from_path_.CStr(), to_path_.CStr());
        !rename_result.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotRenameFile);
    }

    cleanup.Release();
    return SyncDirectory(to_path_.ParentPath());
}

}  // namespace score::filesystem::details
