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
#ifndef BASELIBS_SCORE_FILESYSTEM_I_FILE_FACTORY_H
#define BASELIBS_SCORE_FILESYSTEM_I_FILE_FACTORY_H

#include "score/filesystem/error.h"
#include "score/filesystem/path.h"

#include "score/os/ObjectSeam.h"

#include <ios>
#include <istream>
#include <memory>

namespace score
{
namespace filesystem
{

/// @brief Abstracts the way of how to create input / out operations towards files
///
/// @details In general the idea is to use this factory to create respective /// @brief Then it is possible to inject
/// mocks (aka FileFactoryFake) to avoid file streams in testing environments.
class IFileFactory : public os::ObjectSeam<IFileFactory>
{
  public:
    /// @brief Access method for migration purpose to dependency injection
    static IFileFactory& instance() noexcept;

    /// @brief Opens a respective file stream under the provided path
    virtual score::Result<std::unique_ptr<std::iostream>> Open(const Path&,
                                                             const std::ios_base::openmode mode) noexcept = 0;

    /// @brief Destructor
    virtual ~IFileFactory() noexcept;

    /// @brief Default Constructor
    IFileFactory() noexcept;

  protected:
    /// @brief Copy Constructor
    IFileFactory(const IFileFactory&) noexcept;

    /// @brief Copy Assignment Operator
    IFileFactory& operator=(const IFileFactory&) & noexcept;

    /// @brief Move Constructor
    IFileFactory(IFileFactory&&) noexcept;

    /// @brief Move Assignment Operator
    IFileFactory& operator=(IFileFactory&&) & noexcept;
};

}  // namespace filesystem
}  // namespace score

#endif  // BASELIBS_SCORE_FILESYSTEM_I_FILE_FACTORY_H
