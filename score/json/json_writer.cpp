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

#include "score/json/json_writer.h"
#include "score/json/i_json_writer.h"
#include "score/json/internal/model/error.h"
#include "score/json/internal/writer/json_serialize/json_serialize.h"

#include <ios>
#include <sstream>

namespace
{

template <typename T>
score::ResultBlank ToFileInternal(const T& json_data,
                                const score::cpp::string_view& file_path,
                                score::filesystem::IFileFactory& file_factory)
{
    const std::string file_path_string{file_path.data()};
    const auto file = file_factory.Open(file_path_string, std::ios::out | std::ios::trunc);
    if (!file.has_value())
    {
        return score::MakeUnexpected(score::json::Error::kInvalidFilePath);
    }

    score::json::JsonSerialize serializer{**file};
    return serializer << json_data;
}

template <typename T>
score::ResultBlank ToFileInternalAtomic(const T& json_data,
                                      const std::string_view& file_path,
                                      score::filesystem::IFileFactory& file_factory,
                                      const score::filesystem::AtomicUpdateOwnershipFlags atomic_ownership)

{
    return file_factory.AtomicUpdate(std::string{file_path}, std::ios::out | std::ios::trunc, atomic_ownership)
        .transform_error([](auto err) noexcept {
            return MakeError(score::json::Error::kInvalidFilePath, err.UserMessage());
        })
        .and_then([&json_data](auto filestream) {
            score::json::JsonSerialize serializer{*filestream};
            auto serializer_result = serializer << json_data;
            return filestream->Close().and_then([serializer_result](auto) noexcept {
                return serializer_result;
            });
        });
}

template <typename T>
score::Result<std::string> ToBufferInternal(const T& json_data)
{
    // This line must be hit when the function is called. Since other parts of this function show line coverage,
    // this line must also be hit. Missing coverage is due to a bug in the coverage tool
    std::ostringstream string_stream{};  // LCOV_EXCL_LINE

    score::json::JsonSerialize serializer{string_stream};
    serializer << json_data;
    return string_stream.str();
}

}  // namespace

score::json::JsonWriter::JsonWriter(FileSyncMode file_sync_mode,
                                  score::filesystem::AtomicUpdateOwnershipFlags ownership) noexcept
    : file_sync_mode_{file_sync_mode}, atomic_ownership_{ownership}
{
}

score::ResultBlank score::json::JsonWriter::ToFile(const score::json::Object& json_data,
                                               const score::cpp::string_view& file_path,
                                               std::shared_ptr<score::filesystem::IFileFactory> file_factory)
{
    return (file_sync_mode_ == FileSyncMode::kSynced)
               ? ToFileInternalAtomic(
                     json_data, std::string_view{file_path.begin(), file_path.size()}, *file_factory, atomic_ownership_)
               : ToFileInternal(json_data, file_path, *file_factory);
}

score::ResultBlank score::json::JsonWriter::ToFile(const score::json::List& json_data,
                                               const score::cpp::string_view& file_path,
                                               std::shared_ptr<score::filesystem::IFileFactory> file_factory)
{
    return (file_sync_mode_ == FileSyncMode::kSynced)
               ? ToFileInternalAtomic(
                     json_data, std::string_view{file_path.begin(), file_path.size()}, *file_factory, atomic_ownership_)
               : ToFileInternal(json_data, file_path, *file_factory);
}

score::Result<std::string> score::json::JsonWriter::ToBuffer(const score::json::Object& json_data)
{
    return ToBufferInternal(json_data);
}

score::Result<std::string> score::json::JsonWriter::ToBuffer(const score::json::List& json_data)
{
    return ToBufferInternal(json_data);
}
