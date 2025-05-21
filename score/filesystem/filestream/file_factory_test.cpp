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
#include "score/filesystem/factory/filesystem_factory.h"
#include "score/os/mocklib/fcntl_mock.h"

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

class FileFactoryTestWithOsMock : public FileFactoryTest
{
  protected:
    os::MockGuard<os::FcntlMock> fcntl_;
};

TEST_F(FileFactoryTest, OpensFile)
{
    auto file = unit_.Open("/tmp/some_file", std::ios_base::out);

    ASSERT_TRUE(file.has_value());

    *file.value() << "Foo";

    ::unlink("/tmp/some_file");
}

TEST_F(FileFactoryTest, OpensFileForAppending)
{
    auto file = unit_.Open("/tmp/some_file", std::ios_base::out | std::ios_base::app);

    ASSERT_TRUE(file.has_value());

    *file.value() << "Foo";

    ::unlink("/tmp/some_file");
}

TEST_F(FileFactoryTest, OpensNonExistingFile)
{
    auto file = unit_.Open("/tmp/some_file_not_existing", std::ios_base::in);

    ASSERT_FALSE(file.has_value());
    EXPECT_EQ(file.error(), filesystem::ErrorCode::kCouldNotOpenFileStream);
}

TEST_F(FileFactoryTest, OpenForAtomicUpdate)
{
    static constexpr auto kTestFileName = "not_existing_yet";
    Path test_filename = test_tmpdir_ / kTestFileName;
    std::cout << "test_filename: " << test_filename << std::endl;

    // Given a file with some random content
    auto file_result = unit_.Open(test_filename, std::ios::out | std::ios::trunc);
    ASSERT_TRUE(file_result.has_value());
    std::unique_ptr<std::iostream> file = *std::move(file_result);
    (*file) << "Test";
    file.reset();

    // When updating the file with the atomic update
    file_result = unit_.AtomicUpdate(test_filename, std::ios::out | std::ios::trunc);
    ASSERT_TRUE(file_result.has_value());
    file = *std::move(file_result);
    (*file) << "Narf";
    file.reset();

    std::cout << "open" << std::endl;
    // Then the file contains the content that just had been written
    file_result = unit_.Open(test_filename, std::ios::in);
    ASSERT_TRUE(file_result.has_value());
    file = *std::move(file_result);
    std::string text;
    (*file) >> text;
    EXPECT_EQ(text, "Narf");
}

TEST_F(FileFactoryTest, AtomicUpdateInvalidMode)
{
    static constexpr auto kTestFileName = "not_existing_yet";
    Path test_filename = test_tmpdir_ / kTestFileName;
    auto file_result = unit_.AtomicUpdate(test_filename, std::ios::in);

    ASSERT_FALSE(file_result.has_value());
    EXPECT_EQ(file_result.error(), filesystem::ErrorCode::kNotImplemented);
}

TEST_F(FileFactoryTest, OldContentVisibleBeforeAtomicObjectGetsDeleted)
{
    static constexpr auto kTestFileName = "keep_content_before_drop";
    Path test_filename = test_tmpdir_ / kTestFileName;
    std::cout << "test_filename: " << test_filename << std::endl;

    // Given a file with some random content
    auto file_result = unit_.Open(test_filename, std::ios::out | std::ios::trunc);
    ASSERT_TRUE(file_result.has_value());
    std::unique_ptr<std::iostream> file = *std::move(file_result);
    (*file) << "Test";
    file.reset();

    // When updating the file without dropping the object
    auto update_result = unit_.AtomicUpdate(test_filename, std::ios::out | std::ios::trunc);
    ASSERT_TRUE(update_result.has_value());
    auto update_file = *std::move(update_result);
    (*update_file) << "Narf";

    // Then the file should still contain the previous content

    // Now read from the "old" file by regularly opening it
    file_result = unit_.Open(test_filename, std::ios::in);
    ASSERT_TRUE(file_result.has_value());
    file = *std::move(file_result);
    std::string text;
    (*file) >> text;
    EXPECT_EQ(text, "Test");
    text.clear();
    file.reset();

    // When finally dropping the atomic update file
    update_file.reset();

    // Then the file content shall change
    file_result = unit_.Open(test_filename, std::ios::in);
    ASSERT_TRUE(file_result.has_value());
    file = *std::move(file_result);
    (*file) >> text;
    EXPECT_EQ(text, "Narf");
}

TEST_F(FileFactoryTest, ErrorOnFailingAtomicUpdate)
{
    auto fs = FilesystemFactory{}.CreateInstance();

    static constexpr auto kFaultyTargetFile{"file_in_disguise"};
    Path test_dir = test_tmpdir_ / kFaultyTargetFile;
    ASSERT_TRUE(fs.standard->CreateDirectory(test_dir).has_value());

    auto stream = unit_.AtomicUpdate(test_dir, std::ios::out | std::ios::trunc);
    ASSERT_TRUE(stream.has_value());
    **stream << "Hallo";
    auto close_result = (*stream)->Close();
    EXPECT_FALSE(close_result.has_value());

    EXPECT_TRUE(fs.standard->RemoveAll(test_dir).has_value());
}

TEST_F(FileFactoryTest, AtomicUpdateWithEmptyPath)
{
    auto fs = FilesystemFactory{}.CreateInstance();

    static constexpr auto kFaultyTargetFile{""};
    Path test_dir = test_tmpdir_ / kFaultyTargetFile;
    ASSERT_TRUE(fs.standard->CreateDirectory(test_dir).has_value());

    auto stream = unit_.AtomicUpdate(test_dir, std::ios::out | std::ios::trunc);
    ASSERT_FALSE(stream.has_value());
    EXPECT_EQ(stream.error(), filesystem::ErrorCode::kCouldNotOpenFileStream);
}

TEST_F(FileFactoryTestWithOsMock, OpenFileHandleFailedOnOpen)
{
    constexpr os::Stat::Mode kDefaultMode = os::Stat::Mode::kReadUser | os::Stat::Mode::kWriteUser |
                                            os::Stat::Mode::kReadGroup | os::Stat::Mode::kWriteGroup |
                                            os::Stat::Mode::kReadOthers | os::Stat::Mode::kWriteOthers;

    EXPECT_CALL(*fcntl_, open(_, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));
    static constexpr auto kFaultyTargetFile{"path"};
    Path test_dir = test_tmpdir_ / kFaultyTargetFile;

    auto result = details::OpenFileHandle(test_tmpdir_, std::ios_base::out, kDefaultMode);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotOpenFileStream);
}

TEST_F(FileFactoryTestWithOsMock, AtomicUpdateFileHandleFailedOnOpen)
{
    EXPECT_CALL(*fcntl_, open(_, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));
    static constexpr auto kFaultyTargetFile{"path"};
    Path test_dir = test_tmpdir_ / kFaultyTargetFile;

    auto result = unit_.AtomicUpdate(test_tmpdir_, std::ios_base::out);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotOpenFileStream);
}

}  // namespace
}  // namespace filesystem
}  // namespace score
