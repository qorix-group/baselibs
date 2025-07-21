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

#ifndef SCORE_LIB_JSON_JSON_WRITER_H
#define SCORE_LIB_JSON_JSON_WRITER_H

#include "score/filesystem/filestream/file_factory.h"
#include "score/json/i_json_writer.h"

#include <string_view>

namespace score
{
namespace json
{

enum class FileSyncMode
{
    kSynced,
    kUnsynced,
};

class JsonWriter final : public IJsonWriter
{
  public:
    explicit JsonWriter(FileSyncMode file_sync_mode = FileSyncMode::kUnsynced) noexcept;
    JsonWriter(const JsonWriter&) = delete;
    JsonWriter(JsonWriter&&) noexcept = delete;
    JsonWriter& operator=(const JsonWriter&) = delete;
    JsonWriter& operator=(JsonWriter&&) noexcept = delete;
    ~JsonWriter() noexcept override = default;
    score::ResultBlank ToFile(const score::json::Object& json_data,
                            const score::cpp::string_view& file_path,
                            std::shared_ptr<score::filesystem::IFileFactory> file_factory) override;

    score::ResultBlank ToFile(const score::json::List& json_data,
                            const score::cpp::string_view& file_path,
                            std::shared_ptr<score::filesystem::IFileFactory> file_factory) override;

    score::Result<std::string> ToBuffer(const score::json::Object& json_data) override;
    score::Result<std::string> ToBuffer(const score::json::List& json_data) override;

  private:
    FileSyncMode file_sync_mode_;
};

}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_JSON_PARSER_H
