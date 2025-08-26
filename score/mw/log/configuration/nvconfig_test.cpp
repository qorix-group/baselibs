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
#include "nvconfig.h"
#include <fstream>
#include <iostream>

#include "score/mw/log/log_level.h"

#include "gtest/gtest.h"

namespace
{

using NvConfig = score::mw::log::NvConfig;
using ReadResult = NvConfig::ReadResult;

class NonVerboseConfig : public ::testing::Test
{
  public:
    const std::string JSON_PATH()
    {
        return get_path("test-class-id.json");
    };
    const std::string JSON_PATH_2()
    {
        return get_path("second-test-class-id.json");
    };
    const std::string EMPTY_FILE()
    {
        return get_path("empty-class-id.json");
    };
    const std::string EMPTY_JSON_OBJECT()
    {
        return get_path("error-parse-empty-json-object.json");
    };
    const std::string WRONG_JSON_PATH()
    {
        return get_path("wrong-path-class-id.json");
    };
    const std::string EMPTY_JSON()
    {
        return get_path("empty-json-class-id.json");
    };
    const std::string ERROR_PARSE_1_PATH()
    {
        return get_path("error-parse-1-json-class-id.json");
    };
    const std::string ERROR_CONTENT_1_PATH()
    {
        return get_path("error-content-1-json-class-id.json");
    };
    const std::string ERROR_CONTENT_2_PATH()
    {
        return get_path("error-content-2-json-class-id.json");
    };
    const std::string ERROR_CONTENT_3_PATH()
    {
        return get_path("error-content-3-json-class-id.json");
    };
    const std::string ERROR_CONTENT_WRONG_ID_VALUE()
    {
        return get_path("error-content-wrong-id-value.json");
    };

  private:
    const std::string get_path(const std::string& file_name)
    {
        const std::string default_path = "score/mw/log/configuration/test/data/" + file_name;

        std::ifstream file(default_path);
        if (file.is_open())
        {
            file.close();
            return default_path;
        }
        else
        {
            return "external/safe_posix_platform/" + default_path;
        }
    }
};

TEST_F(NonVerboseConfig, NvConfigReturnsExpectedValues)
{
    RecordProperty("Requirement", "SCR-1633147");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Logging libraries use static configuration based on .json files.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    NvConfig nvc1(JSON_PATH());
    EXPECT_EQ(ReadResult::kOK, nvc1.parseFromJson());  // ok json results
    EXPECT_EQ(301,
              nvc1.getDltMsgDesc("score::logging::PersistentLogFileEvent")->GetIdMsgDescriptor());  // id value reading
    EXPECT_EQ(
        1,
        static_cast<int>(
            nvc1.getDltMsgDesc("score::logging::PersistentLogFileEvent")->GetLogLevel()));  // loglevel value reading
    EXPECT_EQ(4,
              static_cast<int>(nvc1.getDltMsgDesc("poseng::logging::ReprocessingCycle")
                                   ->GetLogLevel()));  // loglevel value reading when using default loglevel
    EXPECT_EQ(
        std::string_view{"Repr"},
        nvc1.getDltMsgDesc("aas::logging::ReprocessingCycle")->GetCtxId().GetStringView());  // ctxid value reading
    EXPECT_EQ(
        std::string_view{"PE"},
        nvc1.getDltMsgDesc("poseng::logging::ReprocessingCycle")->GetAppId().GetStringView());  // appid value reading
    EXPECT_EQ(std::string_view{"PERL"},
              nvc1.getDltMsgDesc("score::logging::PersistentLogFileEvent")
                  ->GetCtxId()
                  .GetStringView());  // ctxid value reading other example
}

TEST_F(NonVerboseConfig, NvConfigReturnsExpectedValuesWithOtherFile)
{
    RecordProperty("Requirement", "SCR-1633147");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Logging libraries use static configuration based on .json files.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc2(JSON_PATH_2());
    EXPECT_EQ(ReadResult::kOK, nvc2.parseFromJson());                                               // ok json results
    EXPECT_EQ(8650816, nvc2.getDltMsgDesc("score::plg::awa::DebugData")->GetIdMsgDescriptor());  // id value reading
    EXPECT_EQ(
        3,
        static_cast<int>(nvc2.getDltMsgDesc("adp::logging::DynamicInsight")->GetLogLevel()));  // loglevel value reading
    EXPECT_EQ(2,
              static_cast<int>(nvc2.getDltMsgDesc("score::logging::timesync::DltTimeSyncTimestamp")
                                   ->GetLogLevel()));  // loglevel value reading other example
    EXPECT_EQ(8650814,
              nvc2.getDltMsgDesc("score::pcp::CrocStateTraceable")
                  ->GetIdMsgDescriptor());  // id value reading other example
    EXPECT_EQ(
        std::string_view{"Repr"},
        nvc2.getDltMsgDesc("aas::logging::ReprocessingEvent")->GetCtxId().GetStringView());  // ctxid value reading
    EXPECT_EQ(std::string_view{"Fasi"},
              nvc2.getDltMsgDesc("score::sli::TsfBaseConfig")->GetAppId().GetStringView());  // appid value reading
    EXPECT_EQ(std::string_view{"DTNV"},
              nvc2.getDltMsgDesc("score::plg::driving_tube::DiagnosticLogsData")
                  ->GetCtxId()
                  .GetStringView());  // ctxid value reading other example
    EXPECT_EQ(std::string_view{"Plan"},
              nvc2.getDltMsgDesc("score::plg::driving_tube::DiagnosticLogsData")
                  ->GetAppId()
                  .GetStringView());  // appid value reading other example
}

TEST_F(NonVerboseConfig, NvConfigReturnsErrorOpenWhenGivenEmptyFile)
{
    RecordProperty("Requirement", "SCR-1633147, SCR-7263548");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the inability of parsing a general empty file.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc3(EMPTY_FILE());
    EXPECT_EQ(ReadResult::kERROR_PARSE, nvc3.parseFromJson());  // error parse because it is a general empty file
}

TEST_F(NonVerboseConfig, NvConfigReturnsErrorOpenWhenGivenPathToNonExistentFile)
{
    RecordProperty("Requirement", "SCR-1633147, SCR-7263537");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "ara::log shall discard configuration files that are not found.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc4(WRONG_JSON_PATH());
    EXPECT_EQ(ReadResult::kERROR_PARSE, nvc4.parseFromJson());  // error parse because the file doesn't exist
}

TEST_F(NonVerboseConfig, NvConfigReturnsOkWhenGivenEmptyJsonFile)
{
    RecordProperty("Requirement", "SCR-1633147, SCR-7263548");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of parsing an empty JSON file.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc5(EMPTY_JSON());
    EXPECT_EQ(ReadResult::kOK, nvc5.parseFromJson());  // ok because this json file doesn't have items
}

TEST_F(NonVerboseConfig, NvConfigReturnsErrorParseIfEmptyObject)
{
    RecordProperty("Requirement", "SCR-1633147, SCR-7263548");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies the inability of parsing JSON file that has array instead of JSON object as a root node.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc6(EMPTY_JSON_OBJECT());
    EXPECT_EQ(ReadResult::kERROR_PARSE,
              nvc6.parseFromJson());  // array instead of json object as one of the values
}

TEST_F(NonVerboseConfig, NvConfigReturnsErrorParseIfThereIsSomethingElseInstedOfObjectAsValue)
{
    RecordProperty("Requirement", "SCR-1633147, SCR-7263548");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the inability of parsing a JSON file that does not have object as value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc6(ERROR_PARSE_1_PATH());
    EXPECT_EQ(ReadResult::kERROR_PARSE,
              nvc6.parseFromJson());  // aray instead of json object as one of the values
}

TEST_F(NonVerboseConfig, NvConfigReturnsErrorContentIfCtxidValuePairDoesntExistsForOneObject)
{
    RecordProperty("Requirement", "SCR-1633147, SCR-7263548");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verifies the inability of parsing JSON file that misses ctxid key-value pair for one of the objects.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc7(ERROR_CONTENT_1_PATH());
    EXPECT_EQ(ReadResult::kERROR_CONTENT,
              nvc7.parseFromJson());  // ctxid key-value pair is missing in one of the objects
}

TEST_F(NonVerboseConfig, NvConfigReturnsErrorContentIfAppidValuePairDoesntExistsForOneObject)
{
    RecordProperty("Requirement", "SCR-1633147, SCR-7263548");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verifies the inability of parsing JSON file that misses appid key-value pair for one of the objects.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc8(ERROR_CONTENT_2_PATH());
    EXPECT_EQ(ReadResult::kERROR_CONTENT,
              nvc8.parseFromJson());  // appid key-value pair is missing in one of the objects
}

TEST_F(NonVerboseConfig, NvConfigReturnsErrorContentIfIdValuePairDoesntExistsForOneObject)
{
    RecordProperty("Requirement", "SCR-1633147, SCR-7263548");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies the inability of parsing JSON file that misses id key-value pair for one of the objects.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc9(ERROR_CONTENT_3_PATH());
    EXPECT_EQ(ReadResult::kERROR_CONTENT,
              nvc9.parseFromJson());  // id key-value pair is missing in one of the objects
}

TEST_F(NonVerboseConfig, NvConfigReturnsErrorIfIdDataTypeIsWrong)
{
    RecordProperty("Requirement", "SCR-1633147, SCR-7263548");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the inability of parsing JSON file that has wrong id value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc10(ERROR_CONTENT_WRONG_ID_VALUE());
    EXPECT_EQ(ReadResult::kERROR_CONTENT, nvc10.parseFromJson());  // wrong ID data type (string instead of int).
}

}  // namespace
