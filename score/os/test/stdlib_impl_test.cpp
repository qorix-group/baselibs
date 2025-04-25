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
#include "score/os/stdlib_impl.h"

#include "score/os/errno.h"

#include <gtest/gtest.h>
#include <limits>

namespace score
{
namespace os
{
namespace
{

TEST(StdlibImpl, system_call)
{
    const auto result = score::os::Stdlib::instance().system_call("ls /tmp");
    EXPECT_EQ(result, score::cpp::expected_blank<Error>{});
}

TEST(StdlibImpl, system_callFail)
{
    const auto result1 = score::os::Stdlib::Default()->system_call("d");
    ASSERT_FALSE(result1.has_value());
}

TEST(StdlibImpl, getenv)
{
    EXPECT_EQ(setenv("TEST_ENV", "TEST_VALUE", 0), 0);
    const auto env = score::os::Stdlib::instance().getenv("TEST_ENV");
    EXPECT_STREQ(env, "TEST_VALUE");
    EXPECT_EQ(unsetenv("TEST_ENV"), 0);
}

TEST(StdlibImpl, realpath)
{
    char resolved_path[20];
    auto res = score::os::Stdlib::instance().realpath("/usr/bin/grep", resolved_path);
    EXPECT_STREQ(res.value(), "/usr/bin/grep");
    EXPECT_STREQ(resolved_path, "/usr/bin/grep");
    memset(resolved_path, 0, sizeof(resolved_path));
    auto res1 = score::os::Stdlib::instance().realpath("/tmt", resolved_path);
    EXPECT_EQ(res1.error(), score::os::Error::createFromErrno(ENOENT));
}

TEST(StdlibImpl, calloc_fail)
{
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("ParentRequirement", "SCR-109773");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "This test verifies that an attempt to allocate more than the maximum amount of memory using "
                   "calloc reports an error.");

    // This test verifies that calloc fails gracefully when attempting to allocate excessive memory,
    // returning a null pointer and setting errno to ENOMEM as expected.
    // ThreadSanitizer (TSAN) is expected to fail in this scenario because its allocator terminates
    // the program on such failures instead of returning null. Therefore, we only run the test outside of TSAN.
#if !defined(__SANITIZE_THREAD__)
    const auto result = score::os::Stdlib::instance().calloc(1U, std::numeric_limits<std::size_t>::max() / 2U);
    ASSERT_FALSE(result.has_value());
#else
    GTEST_SKIP() << "Not supported in TSAN due to allocator limitations causing termination on allocation failure.";
#endif
}

TEST(StdlibImpl, calloc)
{
    const auto size = 2;
    const auto result = score::os::Stdlib::instance().calloc(size, sizeof(uint16_t));
    ASSERT_TRUE(result.has_value());
    std::uint16_t* ptr = static_cast<std::uint16_t*>(result.value());

    for (auto itr = 0; itr < size; itr++)
    {
        ASSERT_EQ(ptr[itr], 0);
    }

    ::free(ptr);
}

TEST(StdlibImpl, free)
{
    const auto size = 1;
    std::uint16_t* ptr = static_cast<std::uint16_t*>(::calloc(size, sizeof(uint16_t)));
    ASSERT_NE(ptr, nullptr);

    score::os::Stdlib::instance().free(ptr);
}

TEST(StdlibImpl, mkstemp)
{
    char path[] = "/tmp/fileXXXXXX";
    auto fd = score::os::Stdlib::instance().mkstemp(path);
    ASSERT_FALSE(fd.value() == -1);
    EXPECT_EQ(close(fd.value()), 0);
}

TEST(StdlibImpl, mkstempFail)
{
    char path[] = "/tmp/fileXXXX";
    auto fd = score::os::Stdlib::instance().mkstemp(path);
    EXPECT_EQ(fd.error(), score::os::Error::createFromErrno(EINVAL));
}

TEST(StdlibImpl, mkstemps)
{
    char path[] = "/tmp/fileXXXXXXsuffix";
    auto fd = score::os::Stdlib::instance().mkstemps(path, 6);
    ASSERT_FALSE(fd.value() == -1);
    EXPECT_EQ(close(fd.value()), 0);
}

TEST(StdlibImpl, mkstempsFail)
{
    char path[] = "/tmp/fileXXXXXX";
    auto fd = score::os::Stdlib::instance().mkstemps(path, 6);
    EXPECT_EQ(fd.error(), score::os::Error::createFromErrno(EINVAL));
}

TEST(StdlibTest, PMRDefaultShallReturnImplInstance)
{
    score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource();
    const auto instance = score::os::Stdlib::Default(memory_resource);
    ASSERT_TRUE(instance != nullptr);
    EXPECT_NO_THROW(std::ignore = dynamic_cast<score::os::StdlibImpl*>(instance.get()));
}

}  // namespace
}  // namespace os
}  // namespace score
