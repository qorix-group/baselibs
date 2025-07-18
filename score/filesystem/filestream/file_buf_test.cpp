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
#include "score/filesystem/filestream/file_buf.h"
#include "score/filesystem/error.h"
#include "score/filesystem/filestream/file_stream.h"
#include "score/os/mocklib/stdiomock.h"
#include "score/os/mocklib/unistdmock.h"

#include <gtest/gtest.h>

using testing::_;
using testing::Return;

namespace score::filesystem::details
{
class FileBufTest : public ::testing::Test
{
  protected:
    StdioFileBuf filebuf{0, std::ios::in};
};

class AtomicFileBufTest : public ::testing::Test
{
  protected:
    AtomicFileBuf atomic_filebuf{0, std::ios::in, "from_path", "to_path"};
    os::MockGuard<os::UnistdMock> unistd_;
    os::MockGuard<os::StdioMock> stdio_;
};

TEST_F(FileBufTest, TestFailureOnClose)
{
    EXPECT_CALL(filebuf, is_open()).WillOnce(Return(true));
    EXPECT_CALL(filebuf, close()).WillOnce(Return(nullptr));

    auto result = filebuf.Close();
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kCloseFailed);
}

TEST_F(FileBufTest, TestClosingAlreadyClosedFile)
{
    EXPECT_CALL(filebuf, is_open()).WillOnce(Return(false));
    EXPECT_CALL(filebuf, close()).Times(0);

    auto result = filebuf.Close();
    EXPECT_TRUE(result.has_value());
}

TEST_F(AtomicFileBufTest, TestFailureOnSync)
{
    EXPECT_CALL(atomic_filebuf, is_open()).WillOnce(testing::Return(true));
    EXPECT_CALL(atomic_filebuf, sync()).WillOnce(testing::Return(-1));

    auto result = atomic_filebuf.Close();
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kFsyncFailed);
}

TEST_F(AtomicFileBufTest, TestFailureOnFsync)
{
    EXPECT_CALL(atomic_filebuf, is_open()).WillOnce(Return(true));
    EXPECT_CALL(atomic_filebuf, sync()).WillOnce(Return(0));
    EXPECT_CALL(*unistd_, fsync(_)).WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));
    EXPECT_CALL(atomic_filebuf, close()).Times(0);

    auto result = atomic_filebuf.Close();
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kFsyncFailed);
}

TEST_F(AtomicFileBufTest, TestFailureOnClose)
{
    EXPECT_CALL(atomic_filebuf, is_open()).WillOnce(Return(true));
    EXPECT_CALL(atomic_filebuf, sync()).WillOnce(Return(0));
    EXPECT_CALL(*unistd_, fsync(_)).WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));
    EXPECT_CALL(atomic_filebuf, close()).WillOnce(Return(nullptr));

    auto result = atomic_filebuf.Close();
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kCloseFailed);
}
TEST_F(AtomicFileBufTest, TestFailureOnRename)
{
    EXPECT_CALL(atomic_filebuf, is_open()).WillOnce(Return(true));
    EXPECT_CALL(atomic_filebuf, sync()).WillOnce(Return(0));
    EXPECT_CALL(*unistd_, fsync(_)).WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));
    EXPECT_CALL(atomic_filebuf, close()).WillOnce(Return(&atomic_filebuf));
    EXPECT_CALL(*stdio_, rename(_, _)).WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));

    auto result = atomic_filebuf.Close();
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kCouldNotRenameFile);
}

TEST(FileStreamTest, TestNullBuffer)
{
    FileStream unit{nullptr};
    EXPECT_TRUE(unit.Close().has_value());
}

}  // namespace score::filesystem::details
