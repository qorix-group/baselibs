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
#include "score/os/stdio.h"
#include "score/os/unistd.h"

namespace score::filesystem::details
{

int StdioFileBuf::sync()
{
    const auto parent_sync = StdioFilebufBase::sync();

    if (!os::Unistd::instance().fsync(fd()).has_value())
    {
        return -1;
    }
    else
    {
        return parent_sync;
    }
}

ResultBlank StdioFileBuf::Close()
{
    close();
    return {};
}

AtomicFileBuf::AtomicFileBuf(int fd, std::ios::openmode mode, Path from_path, Path to_path)
    : StdioFileBuf{fd, mode}, from_path_{std::move(from_path)}, to_path_{std::move(to_path)}
{
}

ResultBlank AtomicFileBuf::Close()
{
    ResultBlank result{};

    if (is_open())
    {
        if (sync() != 0)
        {
            std::cerr << "Failed to issue fsync call before atomic update" << std::endl;
            result = MakeUnexpected(ErrorCode::kFsyncFailed);
        }
        StdioFileBuf::close();
        if (auto rename_result = os::Stdio::instance().rename(from_path_.CStr(), to_path_.CStr());
            !rename_result.has_value())
        {
            std::cerr << "Failed to rename temporary file to actual file name for atomic update: "
                      << rename_result.error().ToString() << std::endl;
            if (result.has_value())
            {
                result = MakeUnexpected(ErrorCode::kCouldNotRenameFile);
            }
        }
    }

    return result;
}

}  // namespace score::filesystem::details
