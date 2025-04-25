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
#ifndef BASELIBS_SCORE_FILESYSTEM_FILE_FACTORY_H
#define BASELIBS_SCORE_FILESYSTEM_FILE_FACTORY_H

#include "score/filesystem/filestream/i_file_factory.h"

namespace score
{
namespace filesystem
{

/// @brief Production implementation of IFileFactory. Will create actual file streams.
class FileFactory final : public IFileFactory
{
  public:
    FileFactory() noexcept;

    score::Result<std::unique_ptr<std::iostream>> Open(const Path&, const std::ios_base::openmode mode) noexcept override;
};

}  // namespace filesystem
}  // namespace score

#endif  // BASELIBS_SCORE_FILESYSTEM_FILE_FACTORY_H
