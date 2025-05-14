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
#ifndef SCORE_LIB_OS_MAKEDEV_H
#define SCORE_LIB_OS_MAKEDEV_H

#include "score/os/ObjectSeam.h"

#include <sys/types.h>
#include <cstdint>

namespace score
{
namespace os
{

class MakeDev : public ObjectSeam<MakeDev>
{
  public:
    /// \brief thread-safe singleton accessor
    /// \return Either concrete OS-dependent instance or respective set mock instance
    static MakeDev& instance() noexcept;

    virtual dev_t make_dev(std::uint32_t major, std::uint32_t minor) const noexcept = 0;
    virtual std::uint32_t get_major(dev_t device) const noexcept = 0;
    virtual std::uint32_t get_minor(dev_t device) const noexcept = 0;

    virtual ~MakeDev() = default;

  protected:
    MakeDev() = default;
    MakeDev(const MakeDev&) = default;
    MakeDev(MakeDev&&) = default;
    MakeDev& operator=(const MakeDev&) = default;
    MakeDev& operator=(MakeDev&&) = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MAKEDEV_H
