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
#ifndef BASELIBS_SCORE_FILESYSTEM_FILESTREAM_FILE_FACTORY_FAKE_H
#define BASELIBS_SCORE_FILESYSTEM_FILESTREAM_FILE_FACTORY_FAKE_H

#include "score/filesystem/filestream/file_factory_mock.h"
#include "score/filesystem/filestream/i_string_stream_collection.h"

#include <functional>
#include <sstream>

namespace score
{
namespace filesystem
{

/// @brief Fake implementation of IFileFactory, to be used for Unit-Tests.
///
/// @details The methods Get() enable manipulation of underlying streams. This factory owns
/// the underlying string streams and thus the underlying character devices. Towards the user we provide another stream
/// that also points towards the same character device.
/// @note This class is already included in StandardFilesystemFake.
/// So there is no need to create this class separately if StandardFilesystemFake object is already created.
class FileFactoryFake : public FileFactoryMock
{
  public:
    explicit FileFactoryFake(IStringStreamCollection& collection);

    /// @brief Returns reference to string stream for specified path.
    ///
    /// @details The method is intended to be used in unit tests.
    /// The specified path should exist in IStringStreamCollection, otherwise the assert error occurs.
    std::stringstream& Get(const Path& path) const;

  private:
    score::Result<std::unique_ptr<std::iostream>> FakeOpen(const Path& path,
                                                         const std::ios_base::openmode mode = std::ios_base::in |
                                                                                              std::ios_base::out) const;

    std::reference_wrapper<IStringStreamCollection> collection_;
};

}  // namespace filesystem
}  // namespace score

#endif  // BASELIBS_SCORE_FILESYSTEM_FILESTREAM_FILE_FACTORY_FAKE_H
