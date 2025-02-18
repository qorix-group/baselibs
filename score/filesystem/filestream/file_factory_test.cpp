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

#include <gtest/gtest.h>

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
    FileFactoryTest() : unit_{}, test_tmpdir_{}
    {
        auto* env_tmpdir = std::getenv("TEST_TMPDIR");
        if (env_tmpdir != nullptr)
        {
            test_tmpdir_ = env_tmpdir;
        }
        else
        {
            test_tmpdir_ = "/tmp";
        }
    }

  protected:
    FileFactory unit_;
    Path test_tmpdir_;
};

TEST_F(FileFactoryTest, OpensFile)
{
    auto file = unit_.Open("/tmp/some_file", std::ios_base::out);

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

}  // namespace
}  // namespace filesystem
}  // namespace score
