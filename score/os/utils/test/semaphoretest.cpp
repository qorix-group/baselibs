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
#include "score/os/utils/semaphore.h"

#include "score/os/utils/test/lib/random_string.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <thread>

namespace score
{
namespace os
{
namespace test
{

void f(const std::string& name)
{
    utils::Semaphore sem(name);
    sem.wait();
}

struct PosixSemaphoreWrapperTest : ::testing::Test
{
    std::string m_name{random_string(31)};
};

TEST_F(PosixSemaphoreWrapperTest, Count0)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PosixSemaphoreWrapperTest Count0");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    utils::Semaphore sem(m_name, 0);

    std::thread t1(f, m_name);
    std::thread t2(f, m_name);

    sem.post();
    sem.post();

    t1.join();
    t2.join();

    sem.unlink();
}

TEST_F(PosixSemaphoreWrapperTest, Count1)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PosixSemaphoreWrapperTest Count1");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    utils::Semaphore sem(m_name, 1);

    std::thread t1(f, m_name);
    t1.join();

    sem.unlink();
}

TEST_F(PosixSemaphoreWrapperTest, Count2)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PosixSemaphoreWrapperTest Count2");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    utils::Semaphore sem(m_name, 2);

    std::thread t1(f, m_name);
    t1.join();

    std::thread t2(f, m_name);
    t2.join();

    sem.unlink();
}

}  // namespace test
}  // namespace os
}  // namespace score
