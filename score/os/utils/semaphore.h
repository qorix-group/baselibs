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
#ifndef SCORE_LIB_OS_UTILS_SEMAPHORE_H
#define SCORE_LIB_OS_UTILS_SEMAPHORE_H

#include "score/os/errno.h"
#include "score/utils/pimpl_ptr.h"

#include <score/expected.hpp>

#include <string>

namespace score
{
namespace os
{
namespace utils
{

class Semaphore
{
    class SemaphorePrivate;
    score::utils::pimpl_ptr_64<SemaphorePrivate> d_ptr;

  public:
    explicit Semaphore(const std::string& name, const std::uint32_t count = 0, const bool create_if_not_exists = true);
    ~Semaphore();

    Semaphore(Semaphore&&) noexcept = delete;
    Semaphore& operator=(Semaphore&&) noexcept = delete;
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

    score::cpp::expected_blank<score::os::Error> post() const noexcept;
    score::cpp::expected_blank<score::os::Error> wait() const noexcept;
    score::cpp::expected_blank<score::os::Error> getvalue(std::int32_t& sval) const noexcept;
    score::cpp::expected_blank<score::os::Error> unlink() const noexcept;
    score::cpp::expected_blank<score::os::Error> close() noexcept;
};

}  // namespace utils
}  // namespace os
}  // namespace score

#endif
