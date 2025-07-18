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
#include "score/filesystem/file_utils/file_test_utils.h"
#include "score/filesystem/filestream/file_buf.h"
#include "score/filesystem/filestream/file_factory.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ftw.h>

#include <cstdio>
#include <iostream>

namespace score
{
namespace filesystem
{
namespace
{

using namespace ::testing;
using testing::Return;

class FileFactoryTestExtra : public ::testing::Test
{
  public:
    FileFactoryTestExtra() : unit_{}, test_tmpdir_{}, remove_temp_{false}
    {
        InitializeTempDir();
    }

    ~FileFactoryTestExtra() override
    {
        RemoveTempContent();
    }

  protected:
    void InitializeTempDir()
    {
        test_tmpdir_ = "/tmp";
        const auto temp_directory = FileTestUtils::GetTempDirectory();
        if (temp_directory.has_value())
        {
            test_tmpdir_ = temp_directory.value();
        }
    }

    static int RemoveDirentry(const char* fname, const struct stat*, int, struct FTW*)
    {
        std::remove(fname);
        return 0;
    }

    void RemoveTempContent() const
    {
        if (remove_temp_)
        {
            ::nftw(test_tmpdir_.Native().c_str(), RemoveDirentry, 64, FTW_DEPTH);
        }
    }

    FileFactory unit_;
    Path test_tmpdir_;
    bool remove_temp_;
};

TEST_F(FileFactoryTestExtra, CreateFileStreamUnopenedFile)
{
    class MockStdioFileBuf : public details::StdioFileBuf
    {
      public:
        MockStdioFileBuf(int fd, std::ios::openmode mode) : StdioFileBuf(fd, mode)
        {
            EXPECT_CALL(*this, is_open()).WillOnce(Return(false));
        }
    };

    auto result = details::CreateFileStream<MockStdioFileBuf>(0, std::ios::in);
    ASSERT_FALSE(result.has_value());
}

}  // namespace
}  // namespace filesystem
}  // namespace score
