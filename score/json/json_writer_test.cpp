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
#include "score/filesystem/filestream/file_factory_fake.h"
#include "score/filesystem/filestream/simple_string_stream_collection.h"

#include <gtest/gtest.h>

#include <memory>
#include <sstream>

using ::testing::_;

namespace score
{
namespace json
{
namespace
{

const std::string input_json_object{R"({
    "num": 1,
    "string": "foo"
})"};

const std::string input_json_list{R"([
    1234,
    "string",
    {
        "key": "value"
    }
])"};

TEST(JsonWriterTest, ToBufferObject)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "writing json object to string buffer");
    RecordProperty("TestType", "Verification of the control flow and data flow");
    RecordProperty("DerivationTechnique", "Error guessing based on knowledge or experience");

    score::json::Object json{};
    json["string"] = score::json::Any{std::string{"foo"}};
    json["num"] = score::json::Any{1};

    score::json::JsonWriter writer{};
    std::string buffer = *writer.ToBuffer(json);

    EXPECT_EQ(buffer, input_json_object);
}
TEST(JsonWriterTest, ToBufferList)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "writing json list to string buffer, cf. RFC-8259 section 4, 5 and 9");
    RecordProperty("TestType", "Verification of the control flow and data flow");
    RecordProperty("DerivationTechnique", "Error guessing based on knowledge or experience");

    score::json::List list{};
    list.push_back(json::Any{1234});
    list.push_back(json::Any{std::string{"string"}});
    score::json::Object obj{};
    obj["key"] = score::json::Any{std::string{"value"}};
    list.push_back(json::Any{std::move(obj)});

    score::json::JsonWriter writer{};
    std::string buffer = *writer.ToBuffer(list);

    EXPECT_EQ(buffer, input_json_list);
}

TEST(JsonWriterTest, ToFileInvalidFilePath)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Failure while writing json object to invalid file path");
    RecordProperty("TestType", "Verification of the control flow and data flow");
    RecordProperty("DerivationTechnique", "Error guessing based on knowledge or experience");

    score::json::Object json{};
    json["string"] = score::json::Any{std::string{"foo"}};
    json["num"] = score::json::Any{1};

    score::json::JsonWriter writer{};
    auto result = writer.ToFile(json, "/foo/bar.json", std::make_shared<score::filesystem::FileFactory>());

    EXPECT_EQ(result.error(), score::json::Error::kInvalidFilePath);
}

TEST(JsonWriterTest, ToFileObject)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "writing json object to valid file path");
    RecordProperty("TestType", "Verification of the control flow and data flow");
    RecordProperty("DerivationTechnique", "Error guessing based on knowledge or experience");

    score::json::Object json{};
    json["string"] = score::json::Any{std::string{"foo"}};
    json["num"] = score::json::Any{1};

    score::filesystem::SimpleStringStreamCollection stream{};
    auto file_factory_fake = std::make_shared<score::filesystem::FileFactoryFake>(stream);

    EXPECT_CALL(*file_factory_fake, Open(_, _)).Times(1);

    score::json::JsonWriter writer{};
    auto result = writer.ToFile(json, "/foo/foo.json", file_factory_fake);

    EXPECT_EQ(result.has_value(), true);

    auto& file = file_factory_fake->Get("/foo/foo.json");

    EXPECT_EQ(file.str(), input_json_object);
}

TEST(JsonWriterTest, ToFileList)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "writing json list to valid file path, cf. RFC-8259 section 4, 5 and 9");
    RecordProperty("TestType", "Verification of the control flow and data flow");
    RecordProperty("DerivationTechnique", "Error guessing based on knowledge or experience");

    score::json::List list{};
    list.push_back(json::Any{1234});
    list.push_back(json::Any{std::string{"string"}});
    score::json::Object obj{};
    obj["key"] = score::json::Any{std::string{"value"}};
    list.push_back(json::Any{std::move(obj)});

    score::filesystem::SimpleStringStreamCollection stream{};
    auto file_factory_fake = std::make_shared<score::filesystem::FileFactoryFake>(stream);

    EXPECT_CALL(*file_factory_fake, Open(_, _)).Times(1);

    score::json::JsonWriter writer{};
    auto result = writer.ToFile(list, "/foo/foo.json", file_factory_fake);

    EXPECT_EQ(result.has_value(), true);

    auto& file = file_factory_fake->Get("/foo/foo.json");

    EXPECT_EQ(file.str(), input_json_list);
}

}  // namespace
}  // namespace json
}  // namespace score
