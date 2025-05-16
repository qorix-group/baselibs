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

#include <gtest/gtest.h>

#include <unistd.h>

namespace score
{
namespace filesystem
{
namespace
{

class FileFactoryTest : public ::testing::Test
{
  public:
    FileFactory unit_{};
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

}  // namespace
}  // namespace filesystem
}  // namespace score
