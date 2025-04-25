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
#include "score/os/utils/mocklib/semaphoremock.h"

#include <string>

namespace score
{
namespace os
{
namespace utils
{

namespace
{
}

SemaphoreMock::SemaphoreMock() {}

class Semaphore::SemaphorePrivate
{
};

Semaphore::Semaphore(const std::string& name, std::uint32_t count, bool create_if_not_exists)
{
    (void)name;
    (void)count;
    (void)create_if_not_exists;
}

Semaphore::~Semaphore() {}

score::cpp::expected_blank<score::os::Error> Semaphore::post() const noexcept
{
    return {};
}

score::cpp::expected_blank<score::os::Error> Semaphore::wait() const noexcept
{
    return {};
}

score::cpp::expected_blank<score::os::Error> Semaphore::unlink() const noexcept
{
    return {};
}

score::cpp::expected_blank<score::os::Error> Semaphore::getvalue(std::int32_t& sval) const noexcept
{
    sval = 0;
    return {};
}

score::cpp::expected_blank<score::os::Error> Semaphore::close() noexcept
{
    return {};
}

}  // namespace utils
}  // namespace os
}  // namespace score
