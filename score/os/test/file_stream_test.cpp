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
#include "score/os/file_stream.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <cstdio>
#include <thread>

namespace score::os
{
namespace test
{

using namespace ::testing;

TEST(FileStreamTest, InstanceShouldReturnSoleObject)
{
    FileStream& subject = FileStream::instance();
    FileStream& another_subject = FileStream::instance();
    std::atomic<FileStream*> subject_from_another_thread{nullptr};
    std::thread t{[&subject_from_another_thread]() noexcept {
        FileStream* subject_ptr = &FileStream::instance();
        subject_from_another_thread.store(subject_ptr);
    }};
    t.join();

    EXPECT_THAT(&subject, NotNull());
    EXPECT_EQ(&subject, &another_subject);
    EXPECT_EQ(&subject, subject_from_another_thread.load());
}

TEST(FileStreamTest, SetLineBufShouldSetLineBuf)
{
    FileStream& subject = FileStream::instance();

    ::setvbuf(stdin, NULL, _IONBF, 0);
    ::setvbuf(stderr, NULL, _IONBF, 0);

    ASSERT_NO_THROW(subject.setlinebuf(stdin));
    ASSERT_NO_THROW(subject.setlinebuf(stderr));
#if defined(__linux__)
    EXPECT_EQ(stdin->_flags & _IOLBF, _IOLBF);
    EXPECT_EQ(stderr->_flags & _IOLBF, _IOLBF);
#endif
}

}  // namespace test
}  // namespace score::os
