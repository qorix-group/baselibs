/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
/*!        \file
 *        \brief  Contains tests concerning the SingleArrayParser class.
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>

#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/composition_parser_impl.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/json_data.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/json_error_domain.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace unit_test
{
namespace
{

class DynamicSingleArrayParser : public v2::SingleArrayParser
{
  public:
    using v2::SingleArrayParser::SingleArrayParser;

    MOCK_METHOD(ParserResult, OnElement, (), (noexcept, override));
};

struct InputData
{
    std::string_view json;
    std::string name;
    std::size_t index;
};

auto operator<<(std::ostream& os, const InputData& in) noexcept -> std::ostream&
{
    return os << in.name;
}

const InputData ordinary_array{R"([1,2,3])", "OrdinaryArray", 3};
const InputData empty_elements{R"([,,,,,1,,,,2])", "EmptyElements", 2};
const InputData trailing_commas{R"([1,,,])", "TrailingCommas", 1};
const InputData empty_array{R"([])", "EmptyArray", 0};
const InputData empty_elements_only{R"([,,,])", "EmptyElementsOnly", 0};

class UT__Parser__SingleArrayParser__Dynamic : public ::testing::TestWithParam<InputData>
{
  public:
    void SetUp() noexcept override
    {
        auto result = JsonData::FromBuffer(GetParam().json);
        ASSERT_TRUE(result.has_value());
        doc_ = std::make_unique<JsonData>(std::move(result).value());
        parser_ = std::make_unique<testing::NiceMock<DynamicSingleArrayParser>>(*doc_);

        ON_CALL(*parser_, OnElement()).WillByDefault([this]() noexcept {
            return parser_->Number<std::uint8_t>([](std::uint8_t) noexcept {});
        });
    }

    std::unique_ptr<JsonData> doc_;
    std::unique_ptr<testing::NiceMock<DynamicSingleArrayParser>> parser_;
};

}  // namespace

/*!
 * Tests the dynamic interface version of the SingleArrayParser.
 * \trace           score::json::vajson::v2::SingleArrayParser::GetIndex
 */
TEST_P(UT__Parser__SingleArrayParser__Dynamic, Success)
{
    const auto result = parser_->Parse();
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(parser_->GetIndex(), GetParam().index);
}

INSTANTIATE_TEST_SUITE_P(
    SingleArrayParser,
    UT__Parser__SingleArrayParser__Dynamic,
    ::testing::Values(ordinary_array, empty_elements, trailing_commas, empty_array, empty_elements_only),
    [](const testing::TestParamInfo<InputData>& input) noexcept {
        return input.param.name;
    });

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
