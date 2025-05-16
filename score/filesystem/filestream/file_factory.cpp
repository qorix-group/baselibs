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

// Current filesystem library as per design is wraper over OS filesystem so it should be the only place in the project
// where <fstream> usage alowed
// NOLINTNEXTLINE(score-banned-include): See above
#include <fstream>

namespace score
{
namespace filesystem
{

FileFactory::FileFactory() noexcept = default;

score::Result<std::unique_ptr<std::iostream>> FileFactory::Open(const Path& path,
                                                              const std::ios_base::openmode mode) noexcept
{
    //  NOLINTNEXTLINE(score-banned-type): this is wrapper over OS filesystem
    std::unique_ptr<std::fstream> file = std::make_unique<std::fstream>(path, mode);
    if (!file->is_open())
    {
        return MakeUnexpected(filesystem::ErrorCode::kCouldNotOpenFileStream);
    }
    return {std::move(file)};
}

}  // namespace filesystem
}  // namespace score
