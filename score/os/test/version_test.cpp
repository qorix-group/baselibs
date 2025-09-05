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
#include <gtest/gtest.h>

#include "score/os/version.h"

class VersionTest : public ::testing::Test
{
};

#ifndef __QNX__
TEST(VersionTest, _SPPOSQnxVersion_MacroIsNotLeakedInNonQnxOS)
{
#ifndef _SPP_QNX_VERSION_
    SUCCEED();
#else
    FAIL() << "_SPP_QNX_VERSION_ should not be defined in non-QNX environment";
#endif
}

TEST(VersionTest, SPPOSQnx8MacroIsNotLeakedInNonQnxOS)
{
#ifndef SPP_OS_QNX8
    SUCCEED();
#else
    FAIL() << "SPP_OS_QNX8 should not be defined in non-QNX environment";
#endif
}
#endif

#if defined(__QNX__)
TEST(VersionTest, SPPOSQnxVersionMacroIsDefinedForQnx)
{
    ASSERT_TRUE(SPP_OS_QNX_VERSION);
}

TEST(QnxVersionTest, SPPOSQnxVersionMatchesQnxMacro)
{
#if __QNX__ >= 800  // QNX 8 or later
    EXPECT_EQ(SPP_OS_QNX_VERSION, __QNX__);
#else
    EXPECT_EQ(SPP_OS_QNX_VERSION, _NTO_VERSION);
#endif
}

TEST(QnxVersionTest, SPPOSQnxVersionIsWithinValidRange)
{
    EXPECT_GE(SPP_OS_QNX_VERSION, 700u);   // Assuming QNX version is at least 7.0
    EXPECT_LT(SPP_OS_QNX_VERSION, 1000u);  // Assuming QNX version is less than 10.0
}

#if __QNX__ >= 800  // QNX 8 or later
TEST(QnxVersionTest, SPPOSQnxVersionIsQnx8OrHigher)
{
    EXPECT_GE(SPP_OS_QNX_VERSION, 800u);
}
#else
TEST(QnxVersionTest, SPPOSQnxVersionIsLowerThanQnx8)
{
    EXPECT_LT(SPP_OS_QNX_VERSION, 800u);
}
#endif

#if __QNX__ >= 800  // QNX 8 or later
TEST(QnxVersionTest, SPPOSQNX8IsDefinedForQnx8OrHigher)
{
    ASSERT_TRUE(SPP_OS_QNX8);
}
#endif

#endif
