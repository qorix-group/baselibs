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
#include "score/os/cpuid.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <thread>

namespace score::os
{
namespace test
{

using namespace ::testing;

TEST(CpuidTest, InstanceShouldReturnSoleObject)
{
    CpuId& subject = CpuId::instance();
    CpuId& another_subject = CpuId::instance();
    std::atomic<CpuId*> subject_from_another_thread{nullptr};
    std::thread t{[&subject_from_another_thread]() noexcept {
        CpuId* subject_ptr = &CpuId::instance();
        subject_from_another_thread.store(subject_ptr);
    }};
    t.join();

    EXPECT_THAT(&subject, NotNull());
    EXPECT_EQ(&subject, &another_subject);
    EXPECT_EQ(&subject, subject_from_another_thread.load());
}

}  // namespace test
}  // namespace score::os
