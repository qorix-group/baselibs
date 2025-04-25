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

#include "score/os/stdio.h"

#include "score/os/errno.h"

#include <gtest/gtest.h>

#include <fcntl.h>

namespace
{

TEST(FOpenTest, ReturnsValidFileDescriptor)
{
    const auto filename{"test"};
    const auto mode{"w"};
    const auto result = score::os::Stdio::instance().fopen(filename, mode);
    ASSERT_TRUE(result.has_value());
    EXPECT_NE(result.value(), nullptr);
}

TEST(FOpenTest, ReturnsErrorWithWrongMode)
{
    const auto filename{"test"};
    const auto mode{"bs"};
    const auto result = score::os::Stdio::instance().fopen(filename, mode);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kInvalidArgument);
}

TEST(FCloseTest, ReturnsBlankValueIfSuccessful)
{
    const auto filename{"test"};
    const auto mode{"w"};
    const auto open_result = score::os::Stdio::instance().fopen(filename, mode);
    ASSERT_TRUE(open_result.has_value());

    const auto close_result = score::os::Stdio::instance().fclose(open_result.value());
    ASSERT_TRUE(close_result.has_value());
}

TEST(FCloseTest, ReturnsErrorWithInvalidFileDescriptor)
{
#if defined(GTEST_OS_LINUX)
    GTEST_SKIP() << "::fclose() is not POSIX compliant with glibc on Linux - any error causes an abort";
#endif

    FILE invalid_file_descriptor;
    const auto result = score::os::Stdio::instance().fclose(&invalid_file_descriptor);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kInvalidArgument);
}

TEST(RemoveTest, ReturnsBlankValueIfSuccessful)
{
    const auto filename{"test"};
    const auto mode{"w"};
    const auto open_result = score::os::Stdio::instance().fopen(filename, mode);
    ASSERT_TRUE(open_result.has_value());

    const auto close_result = score::os::Stdio::instance().fclose(open_result.value());
    ASSERT_TRUE(close_result.has_value());

    const auto remove_result = score::os::Stdio::instance().remove(filename);
    ASSERT_TRUE(remove_result.has_value());
}

TEST(RemoveTest, ReturnsErrorIfFileDoesNotExist)
{
    const auto filename{"test-invalid-file"};
    const auto remove_result = score::os::Stdio::instance().remove(filename);
    ASSERT_FALSE(remove_result.has_value());
    EXPECT_EQ(remove_result.error(), score::os::Error::Code::kNoSuchFileOrDirectory);
}

TEST(RenameTest, ReturnsBlankValueIfSuccessful)
{
    const auto filename{"test"};
    const auto mode{"w"};
    const auto open_result = score::os::Stdio::instance().fopen(filename, mode);
    ASSERT_TRUE(open_result.has_value());

    const auto close_result = score::os::Stdio::instance().fclose(open_result.value());
    ASSERT_TRUE(close_result.has_value());

    const auto new_filename{"new_test"};
    const auto rename_result = score::os::Stdio::instance().rename(filename, new_filename);
    ASSERT_TRUE(rename_result.has_value());

    const auto remove_result = score::os::Stdio::instance().remove(new_filename);
    ASSERT_TRUE(remove_result.has_value());
}

TEST(RenameTest, ReturnsErrorIfNameInvalid)
{
    const auto filename{"invalid-test"};
    const auto new_filename{"invalid_test"};
    const auto rename_result = score::os::Stdio::instance().rename(filename, new_filename);
    ASSERT_FALSE(rename_result.has_value());
    EXPECT_EQ(rename_result.error(), score::os::Error::Code::kNoSuchFileOrDirectory);
}

TEST(POpenTest, ReturnsValidPipe)
{
    const auto cmd{"echo 1"};
    const auto mode{"r"};
    const auto result = score::os::Stdio::instance().popen(cmd, mode);
    ASSERT_TRUE(result.has_value());
    ASSERT_NE(result.value(), nullptr);
    constexpr std::int32_t buf_size{3};
    char buf[buf_size];
    memset(buf, '\0', buf_size);
    const auto read = ::fread(buf, sizeof(char), buf_size, result.value());
    EXPECT_EQ(read, 2);
    EXPECT_STREQ(buf, "1\n");
    ::pclose(result.value());
}

TEST(POpenTest, ReturnsErrorWithInvalidMode)
{
    const auto cmd{"echo 1"};
    const auto mode{"x"};
    const auto result = score::os::Stdio::instance().popen(cmd, mode);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kInvalidArgument);
}

TEST(PCloseTest, ProvidesReturnCodeOfPipeCommand)
{
    const auto cmd{"exit 1"};
    const auto mode{"r"};
    const auto open_result = score::os::Stdio::instance().popen(cmd, mode);
    ASSERT_TRUE(open_result.has_value());
    ASSERT_NE(open_result.value(), nullptr);

    const auto close_result = score::os::Stdio::instance().pclose(open_result.value());
    ASSERT_TRUE(close_result.has_value());
    EXPECT_TRUE(WIFEXITED(close_result.value()));
    EXPECT_EQ(WEXITSTATUS(close_result.value()), 1);
}

TEST(FileNoTest, CanTranslateFileDescriptorOfStream)
{
    const auto result = score::os::Stdio::instance().fileno(stdin);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), STDIN_FILENO);
}

TEST(FileNoTest, ReturnsErrorForInvalidFileStream)
{
#if defined(GTEST_OS_LINUX)
    GTEST_SKIP() << "::fileno() is not POSIX compliant with glibc on Linux - any error causes an abort";
#endif

    FILE* stream{nullptr};
    const auto result = score::os::Stdio::instance().fileno(stream);
    ASSERT_FALSE(result.has_value());
}

}  // namespace
