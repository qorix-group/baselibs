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
#include "score/filesystem/filestream/file_factory.h"
// #include "score/filesystem/factory/filesystem_factory.h"
#include "score/filesystem/filestream/file_buf.h"

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

class FileFactoryTest : public ::testing::Test
{
  public:
    FileFactoryTest() : unit_{}, test_tmpdir_{}, remove_temp_{false}
    {
        InitializeTempDir();
    }

    ~FileFactoryTest() override
    {
        RemoveTempContent();
    }

  protected:
    void InitializeTempDir()
    {
        // First, check whether we have TEST_TMPDIR (which is set by Bazel)
        // coverity[autosar_cpp14_m18_0_3_violation] Test code, not a problem to use getenv.
        auto* env_tmpdir = std::getenv("TEST_TMPDIR");
        if (env_tmpdir != nullptr)
        {
            test_tmpdir_ = env_tmpdir;
        }
        else
        {
            // Otherwise, check whether we might be on a target
            //
            // This is a poor-man's test whether we're being run on real hardware
            // If /persistent happens to exist anywhere else, we just assume that reusing this directory is also ok in
            // case we're allowed to create a directory here.
            //
            // We're using pure POSIX here since the stdc++ filesystem lib doesn't seem to exist on the target toolchain
            // and we're testing our filesystem library here which makes it brittle to use other parts of itself to test
            // it.
            struct ::stat statbuf;
            if (::stat("/persistent", &statbuf) == 0)  // If "/persistent" exists...
            {
                if (statbuf.st_mode & S_IFDIR)  // ...and is a directory
                {
                    if (::stat("/persistent/tmp", &statbuf) != 0)  // ...and if "/persistent/tmp" does not exist
                    {
                        // ...then create it and also remove it during destruction
                        remove_temp_ = true;
                        if (::mkdir("/persistent/tmp", 0777) != 0)
                        {
                            throw std::runtime_error("Couldn't create dir");
                        }
                    }
                    test_tmpdir_ = "/persistent/tmp";
                }
                else
                {
                    throw std::runtime_error("/persistent is not a directory. What gives?!");
                }
            }
        }

        // As a last resort, just use /tmp
        if (test_tmpdir_.Empty())
        {
            test_tmpdir_ = "/tmp";
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

TEST_F(FileFactoryTest, CreateFileStreamUnopenedFile)
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
