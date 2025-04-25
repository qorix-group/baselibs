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
#include "score/os/makedev.h"

#include <gtest/gtest.h>

TEST(MakedevTest, ReturnValidDev)
{
    constexpr std::uint32_t major = 1U;
    constexpr std::uint32_t minor = 0U;

    const auto device = score::os::MakeDev::instance().make_dev(major, minor);

    ASSERT_EQ(score::os::MakeDev::instance().get_major(device), major);
    ASSERT_EQ(score::os::MakeDev::instance().get_minor(device), minor);
}
