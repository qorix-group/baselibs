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
#include "score/os/fcntl.h"
#include "score/os/errno.h"
#include "score/os/fcntl_impl.h"

#include "gtest/gtest.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <cerrno>

namespace score
{
namespace os
{
namespace test
{

TEST(CommandToInteger, kFileGetStatusFlags)
{
    const auto result = internal::fcntl_helper::CommandToInteger(Fcntl::Command::kFileGetStatusFlags);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), F_GETFL);
}

TEST(CommandToInteger, kFileSetStatusFlags)
{
    const auto result = internal::fcntl_helper::CommandToInteger(Fcntl::Command::kFileSetStatusFlags);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), F_SETFL);
}

TEST(CommandToInteger, kInvalid)
{
    const auto result = internal::fcntl_helper::CommandToInteger(Fcntl::Command::kInvalid);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::Code::kInvalidArgument);
}

TEST(IntegerToOpenFlag, Translate_O_RDONLY)
{
    const auto result = internal::fcntl_helper::IntegerToOpenFlag(O_RDONLY);
    EXPECT_EQ(result, Fcntl::Open::kReadOnly);
}

TEST(IntegerToOpenFlag, Translate_O_WRONLY)
{
    const auto result = internal::fcntl_helper::IntegerToOpenFlag(O_WRONLY);
    EXPECT_EQ(result, Fcntl::Open::kWriteOnly);
}

TEST(IntegerToOpenFlag, Translate_O_RDWR)
{
    const auto result = internal::fcntl_helper::IntegerToOpenFlag(O_RDWR);
    EXPECT_EQ(result, Fcntl::Open::kReadWrite);
}

TEST(IntegerToOpenFlag, Translate_O_CREAT)
{
    const auto result = internal::fcntl_helper::IntegerToOpenFlag(O_CREAT);
    // Open flags have always an access mode. If none is explicitly set it is readonly
    EXPECT_EQ(result, Fcntl::Open::kCreate | Fcntl::Open::kReadOnly);
}

TEST(IntegerToOpenFlag, Translate_O_CLOEXEC)
{
    const auto result = internal::fcntl_helper::IntegerToOpenFlag(O_CLOEXEC);
    // Open flags have always an access mode. If none is explicitly set it is readonly
    EXPECT_EQ(result, Fcntl::Open::kCloseOnExec | Fcntl::Open::kReadOnly);
}

TEST(IntegerToOpenFlag, Translate_O_NONBLOCK)
{
    const auto result = internal::fcntl_helper::IntegerToOpenFlag(O_NONBLOCK);
    // Open flags have always an access mode. If none is explicitly set it is readonly
    EXPECT_EQ(result, Fcntl::Open::kNonBlocking | Fcntl::Open::kReadOnly);
}

TEST(IntegerToOpenFlag, Translate_O_EXCL)
{
    const auto result = internal::fcntl_helper::IntegerToOpenFlag(O_EXCL);
    // Open flags have always an access mode. If none is explicitly set it is readonly
    EXPECT_EQ(result, Fcntl::Open::kExclusive | Fcntl::Open::kReadOnly);
}

TEST(IntegerToOpenFlag, Translate_O_TRUNC)
{
    const auto result = internal::fcntl_helper::IntegerToOpenFlag(O_TRUNC);
    // Open flags have always an access mode. If none is explicitly set it is readonly
    EXPECT_EQ(result, Fcntl::Open::kTruncate | Fcntl::Open::kReadOnly);
}

TEST(IntegerToOpenFlag, Translate_O_DIRECTORY)
{
    const auto result = internal::fcntl_helper::IntegerToOpenFlag(O_DIRECTORY);
    // Open flags have always an access mode. If none is explicitly set it is readonly
    EXPECT_EQ(result, Fcntl::Open::kDirectory | Fcntl::Open::kReadOnly);
}

TEST(IntegerToOpenFlag, Translate_O_APPEND)
{
    const auto result = internal::fcntl_helper::IntegerToOpenFlag(O_APPEND);
    // Open flags have always an access mode. If none is explicitly set it is readonly
    EXPECT_EQ(result, Fcntl::Open::kAppend | Fcntl::Open::kReadOnly);
}

#ifdef __linux__
TEST(IntegerToOpenFlag, Translate_O_SYNC)
{
    const auto result = internal::fcntl_helper::IntegerToOpenFlag(O_SYNC);
    // Open flags have always an access mode. If none is explicitly set it is readonly
    EXPECT_EQ(result, Fcntl::Open::kSynchronized | Fcntl::Open::kReadOnly);
}
#endif

TEST(IntegerToOpenFlag, TranslateMultiple)
{
    const auto result = internal::fcntl_helper::IntegerToOpenFlag(O_RDWR | O_CREAT);
    // Open flags have always an access mode. If none is explicitly set it is readonly
    EXPECT_EQ(result, Fcntl::Open::kReadWrite | Fcntl::Open::kCreate);
}

TEST(OpenFlagToInteger, TranslateKReadOnly)
{
    const auto result = internal::fcntl_helper::OpenFlagToInteger(Fcntl::Open::kReadOnly);
    EXPECT_EQ(result, O_RDONLY);
}

TEST(OpenFlagToInteger, TranslateKWriteOnly)
{
    const auto result = internal::fcntl_helper::OpenFlagToInteger(Fcntl::Open::kWriteOnly);
    EXPECT_EQ(result, O_WRONLY);
}

TEST(OpenFlagToInteger, TranslateKReadWrite)
{
    const auto result = internal::fcntl_helper::OpenFlagToInteger(Fcntl::Open::kReadWrite);
    EXPECT_EQ(result, O_RDWR);
}

TEST(OpenFlagToInteger, TranslateKCreate)
{
    const auto result = internal::fcntl_helper::OpenFlagToInteger(Fcntl::Open::kCreate);
    EXPECT_EQ(result, O_CREAT);
}

TEST(OpenFlagToInteger, TranslateKCloseOnExec)
{
    const auto result = internal::fcntl_helper::OpenFlagToInteger(Fcntl::Open::kCloseOnExec);
    EXPECT_EQ(result, O_CLOEXEC);
}

TEST(OpenFlagToInteger, TranslateKNonBlocking)
{
    const auto result = internal::fcntl_helper::OpenFlagToInteger(Fcntl::Open::kNonBlocking);
    EXPECT_EQ(result, O_NONBLOCK);
}

TEST(OpenFlagToInteger, TranslateKExclusive)
{
    const auto result = internal::fcntl_helper::OpenFlagToInteger(Fcntl::Open::kExclusive);
    EXPECT_EQ(result, O_EXCL);
}

TEST(OpenFlagToInteger, TranslateKTruncate)
{
    const auto result = internal::fcntl_helper::OpenFlagToInteger(Fcntl::Open::kTruncate);
    EXPECT_EQ(result, O_TRUNC);
}

TEST(OpenFlagToInteger, TranslateKDirectory)
{
    const auto result = internal::fcntl_helper::OpenFlagToInteger(Fcntl::Open::kDirectory);
    EXPECT_EQ(result, O_DIRECTORY);
}

TEST(OpenFlagToInteger, TranslatekAppend)
{
    const auto result = internal::fcntl_helper::OpenFlagToInteger(Fcntl::Open::kAppend);
    EXPECT_EQ(result, O_APPEND);
}

#ifdef __linux__
TEST(OpenFlagToInteger, TranslateKSynchronized)
{
    const auto result = internal::fcntl_helper::OpenFlagToInteger(Fcntl::Open::kSynchronized);
    EXPECT_EQ(result, O_SYNC);
}
#endif

TEST(fcntl, DefaultShallReturnImplInstance)
{
    const auto default_instance = score::os::Fcntl::Default();
    ASSERT_TRUE(default_instance != nullptr);
    EXPECT_NO_THROW(std::ignore = dynamic_cast<score::os::FcntlImpl*>(default_instance.get()));
}

TEST(fcntl, PMRDefaultShallReturnImplInstance)
{
    score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource();
    const auto instance = score::os::Fcntl::Default(memory_resource);
    ASSERT_TRUE(instance != nullptr);
    EXPECT_NO_THROW(std::ignore = dynamic_cast<score::os::FcntlImpl*>(instance.get()));
}

TEST(fcntl, CanGetInstance)
{
    EXPECT_NO_FATAL_FAILURE(Fcntl::instance());
}

}  // namespace test
}  // namespace os
}  // namespace score
