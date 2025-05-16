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
#include "score/filesystem/filestream/file_factory_fake.h"

#include <score/utility.hpp>
#include <sstream>

namespace score
{
namespace filesystem
{

FileFactoryFake::FileFactoryFake(IStringStreamCollection& collection) : FileFactoryMock{}, collection_{collection}
{
    using ::testing::_;
    ON_CALL(*this, Open(_, _)).WillByDefault(::testing::Invoke(this, &FileFactoryFake::FakeOpen));
    ON_CALL(*this, AtomicUpdate(_, _)).WillByDefault(::testing::Invoke(this, &FileFactoryFake::FakeAtomicUpdate));
}

score::Result<std::unique_ptr<std::iostream>> FileFactoryFake::FakeOpen(const Path& path,
                                                                      const std::ios_base::openmode mode) const
{
    score::cpp::ignore = mode;  // not supported, Ticket-36536
    auto stream_result = collection_.get().OpenStringStream(path);
    if (!stream_result.has_value())
    {
        return MakeUnexpected<std::unique_ptr<std::iostream>>(stream_result.error());
    }
    return std::make_unique<std::iostream>(stream_result.value().get().rdbuf());
}

std::stringstream& FileFactoryFake::Get(const Path& path) const
{
    auto stream_result = collection_.get().OpenStringStream(path);
    SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(stream_result.has_value(),
                       "The Get()-function should be exucuted for valid paths that exist in the collection.");
    return stream_result.value().get();
}

}  // namespace filesystem
}  // namespace score
