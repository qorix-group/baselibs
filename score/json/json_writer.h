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

#ifndef BASELIBS_SCORE_JSON_JSON_WRITER_H
#define BASELIBS_SCORE_JSON_JSON_WRITER_H

#include "score/filesystem/filestream/file_factory.h"
#include "score/json/i_json_writer.h"

namespace score
{
namespace json
{

class JsonWriter final : public IJsonWriter
{
  public:
    JsonWriter() noexcept = default;
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
};

}  // namespace json
}  // namespace score

#endif  // BASELIBS_SCORE_JSON_JSON_PARSER_H
