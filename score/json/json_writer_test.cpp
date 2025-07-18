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

namespace score
{
namespace json
{
namespace
{

using ::testing::_;
using ::testing::ByMove;
using ::testing::Return;
using ::testing::StrEq;

class TestJsonList : public json::List
{
  public:
    TestJsonList()
    {
        emplace_back(1234);
        emplace_back(std::string{"string"});
        score::json::Object obj{};
        obj["key"] = score::json::Any{std::string{"value"}};
        emplace_back(std::move(obj));
    }

    static constexpr auto expected = R"([
    1234,
    "string",
    {
        "key": "value"
    }
])";
};

class TestJsonObject : public json::Object
{
  public:
    TestJsonObject()
    {
        emplace("string", score::json::Any{std::string{"foo"}});
        emplace("num", score::json::Any{1});
    }

    static constexpr auto expected = R"({
    "num": 1,
    "string": "foo"
})";
};

using JsonSampleTypes = ::testing::Types<TestJsonList, TestJsonObject>;

template <typename T>
class JsonWriterWriteToFileTest : public ::testing::Test
{
  protected:
    using SampleJson = T;

    score::filesystem::SimpleStringStreamCollection stream{};
    std::shared_ptr<score::filesystem::FileFactoryFake> file_factory_fake{
        std::make_shared<score::filesystem::FileFactoryFake>(stream)};

    template <typename Json, typename... OpenArgs>
    std::string WriteToFile(const Json& json, std::string_view path, FileSyncMode type, OpenArgs&&... open_args)
    {
        score::json::JsonWriter writer{type};
        score::cpp::string_view path_view{path.data(), path.size()};
        auto result = writer.ToFile(json, path_view, file_factory_fake, std::forward<OpenArgs>(open_args)...);

        EXPECT_EQ(result.has_value(), true);

        auto& file = file_factory_fake->Get("/foo/foo.json");
        return file.str();
    }
};

TYPED_TEST_SUITE(JsonWriterWriteToFileTest, JsonSampleTypes, );

TYPED_TEST(JsonWriterWriteToFileTest, ToBuffer)
{
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "writing json to string buffer, cf. RFC-8259 section 4, 5 and 9");
    this->RecordProperty("TestType", "Verification of the control flow and data flow");
    this->RecordProperty("DerivationTechnique", "Error guessing based on knowledge or experience");

    typename TestFixture::SampleJson json;
    score::json::JsonWriter writer{};
    std::string buffer = *writer.ToBuffer(json);

    EXPECT_EQ(buffer, TypeParam::expected);
}

TYPED_TEST(JsonWriterWriteToFileTest, ToFile)
{
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "writing json to valid file path");
    this->RecordProperty("TestType", "Verification of the control flow and data flow");
    this->RecordProperty("DerivationTechnique", "Error guessing based on knowledge or experience");

    typename TestFixture::SampleJson json;

    EXPECT_CALL(*this->file_factory_fake, AtomicUpdate(StrEq("/foo/foo.json"), std::ios::out | std::ios::trunc, _))
        .Times(1);

    auto file_content = this->WriteToFile(json, "/foo/foo.json", FileSyncMode::kSynced);

    EXPECT_EQ(file_content, TypeParam::expected);
}

TYPED_TEST(JsonWriterWriteToFileTest, ToUnsyncedFile)
{
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "writing json to valid file path");
    this->RecordProperty("TestType", "Verification of the control flow and data flow");
    this->RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");

    typename TestFixture::SampleJson json;

    EXPECT_CALL(*this->file_factory_fake, Open(StrEq("/foo/foo.json"), std::ios::out | std::ios::trunc)).Times(1);

    auto file_content = this->WriteToFile(json, "/foo/foo.json", FileSyncMode::kUnsynced);

    EXPECT_EQ(file_content, TypeParam::expected);
}

TYPED_TEST(JsonWriterWriteToFileTest, ToSyncedFile)
{
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "writing json to valid file path");
    this->RecordProperty("TestType", "Verification of the control flow and data flow");
    this->RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");

    typename TestFixture::SampleJson json;

    EXPECT_CALL(*this->file_factory_fake, AtomicUpdate(StrEq("/foo/foo.json"), std::ios::out | std::ios::trunc, _))
        .Times(1);

    auto file_content = this->WriteToFile(json, "/foo/foo.json", FileSyncMode::kSynced);

    EXPECT_EQ(file_content, TypeParam::expected);
}

TYPED_TEST(JsonWriterWriteToFileTest, ToUnsyncedFileResultsInError)
{
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "writing json to valid file path");
    this->RecordProperty("TestType", "Verification of the control flow and data flow");
    this->RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");

    typename TestFixture::SampleJson json;

    EXPECT_CALL(*this->file_factory_fake, Open(StrEq("/foo/foo.json"), std::ios::out | std::ios::trunc))
        .WillOnce(Return(ByMove(score::MakeUnexpected(score::json::Error::kInvalidFilePath))));

    score::json::JsonWriter writer{FileSyncMode::kUnsynced};
    score::cpp::string_view path_view{"/foo/foo.json"};
    auto result = writer.ToFile(json, path_view, this->file_factory_fake);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::json::Error::kInvalidFilePath);
}

TYPED_TEST(JsonWriterWriteToFileTest, ToSyncedFileResultsInError)
{
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "writing json to valid file path");
    this->RecordProperty("TestType", "Verification of the control flow and data flow");
    this->RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");

    typename TestFixture::SampleJson json;

    EXPECT_CALL(*this->file_factory_fake, AtomicUpdate(StrEq("/foo/foo.json"), std::ios::out | std::ios::trunc, _))
        .WillOnce(Return(ByMove(score::MakeUnexpected(score::json::Error::kInvalidFilePath))));

    score::json::JsonWriter writer{FileSyncMode::kSynced};
    score::cpp::string_view path_view{"/foo/foo.json"};
    auto result = writer.ToFile(json, path_view, this->file_factory_fake);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::json::Error::kInvalidFilePath);
}

}  // namespace
}  // namespace json
}  // namespace score
