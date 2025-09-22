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
#include "nvconfigfactory.h"

#include <iostream>

namespace score
{
namespace mw
{
namespace log
{

namespace
{

// Error domain for NvConfig errors
class NvConfigErrorDomain final : public score::result::ErrorDomain
{
  public:
    constexpr NvConfigErrorDomain() noexcept = default;

    std::string_view MessageFor(const score::result::ErrorCode& code) const noexcept override
    {
        switch (static_cast<NvConfigErrorCode>(code))
        {
            case NvConfigErrorCode::kParseError:
                return "Failed to parse JSON configuration file";
            case NvConfigErrorCode::kContentError:
                return "Invalid JSON content - missing required fields";
            default:
                return "Unknown NvConfig error";
        }
    }
};

constexpr NvConfigErrorDomain kNvConfigErrorDomain{};

}  // anonymous namespace

// MakeError function for NvConfigErrorCode (for ADL)
constexpr score::result::Error MakeError(NvConfigErrorCode code, std::string_view user_message) noexcept
{
    return score::result::Error{static_cast<score::result::ErrorCode>(code), kNvConfigErrorDomain, user_message};
}

constexpr mw::log::LogLevel kDefaultLogLevel = mw::log::LogLevel::kInfo;
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// std::terminate() will not  implicitly be called from GetMsgDescriptor as it declared as noexcept.
// coverity[autosar_cpp14_a15_5_3_violation]
config::NvMsgDescriptor NvConfigFactory::GetMsgDescriptor(
    const std::uint32_t id,
    const score::json::Object& object_array_value,
    const score::json::Object::const_iterator& object_appid_iterator,
    const score::json::Object::const_iterator& object_ctxid_iterator) noexcept
{
    const auto& object_loglevel_iterator = object_array_value.find("loglevel");
    auto log_level = kDefaultLogLevel;
    if (object_loglevel_iterator != object_array_value.end())
    {
        log_level = mw::log::TryGetLogLevelFromU8(object_loglevel_iterator->second.As<std::uint8_t>().value())
                        .value_or(kDefaultLogLevel);
    }

    auto object_appid_result = object_appid_iterator->second.As<std::string_view>();
    const score::mw::log::detail::LoggingIdentifier appid(object_appid_result.value());
    auto object_ctxid_result = object_ctxid_iterator->second.As<std::string_view>();
    const score::mw::log::detail::LoggingIdentifier ctxid(object_ctxid_result.value());
    return config::NvMsgDescriptor{id, appid, ctxid, log_level};
}

score::Result<NvConfig> NvConfigFactory::CreateAndInit(const std::string& file_path)
{
    auto typemap_result = ParseFromJson(file_path);
    if (!typemap_result.has_value())
    {
        const auto& error = typemap_result.error();
        std::cerr << "NvConfig error: " << error.Message() << std::endl;
        return score::MakeUnexpected<NvConfig>(error);
    }
    return NvConfig{std::move(typemap_result.value())};
}

NvConfig NvConfigFactory::CreateEmpty() noexcept
{
    return NvConfig{TypeMap{}};
}

score::Result<NvConfigFactory::TypeMap> NvConfigFactory::ParseFromJson(const std::string& json_path) noexcept
{
    const score::json::JsonParser json_parser_obj;
    // FromFile() is safe, if the JSON file is stored in stored on qtsafefs (integrity protection).
    // See broken_link_c/item/13777082 and broken_link_c/issue/6576406
    // NOLINTNEXTLINE(score-banned-function) - Argumentation is above.
    auto root = json_parser_obj.FromFile(json_path);
    if (!root.has_value())
    {
        return score::MakeUnexpected<TypeMap>(
            MakeError(NvConfigErrorCode::kParseError, "Failed to parse JSON file: " + json_path));
    }

    auto parse_result = root.value().As<score::json::Object>();
    if (!parse_result.has_value())
    {
        return score::MakeUnexpected<TypeMap>(
            MakeError(NvConfigErrorCode::kParseError, "Invalid JSON structure in file: " + json_path));
    }

    auto typemap = NvConfigFactory::TypeMap{};
    auto result_code = HandleParseResult(parse_result.value(), typemap);
    if (result_code == INvConfig::ReadResult::kOK)
    {
        return typemap;
    }
    else if (result_code == INvConfig::ReadResult::kERROR_CONTENT)
    {
        return score::MakeUnexpected<TypeMap>(
            MakeError(NvConfigErrorCode::kContentError, "Invalid JSON content in file: " + json_path));
    }
    else
    {
        return score::MakeUnexpected<TypeMap>(
            MakeError(NvConfigErrorCode::kParseError, "Failed to parse JSON file: " + json_path));
    }
}

INvConfig::ReadResult NvConfigFactory::HandleParseResult(const score::json::Object& parse_result,
                                                         NvConfigFactory::TypeMap& typemap) noexcept
{
    for (auto& result_iterator : parse_result)
    {
        auto object_array_result = result_iterator.second.As<score::json::Object>();
        if (!object_array_result.has_value())
        {
            return INvConfig::ReadResult::kERROR_PARSE;
        }

        auto& object_array_value = object_array_result.value().get();
        const auto& object_ctxid_iterator = object_array_value.find("ctxid");
        if (object_ctxid_iterator == object_array_value.end())
        {
            return INvConfig::ReadResult::kERROR_CONTENT;
        }

        const auto& object_id_iterator = object_array_value.find("id");
        if (object_id_iterator == object_array_value.end())
        {
            return INvConfig::ReadResult::kERROR_CONTENT;
        }

        const auto& object_appid_iterator = object_array_value.find("appid");
        if (object_appid_iterator == object_array_value.end())
        {
            return INvConfig::ReadResult::kERROR_CONTENT;
        }

        auto id = object_id_iterator->second.As<std::uint32_t>();
        if (id.has_value() == false)
        {
            return INvConfig::ReadResult::kERROR_CONTENT;
        }

        auto object_name = result_iterator.first.GetAsStringView();
        typemap[{object_name.data(), object_name.size()}] =
            GetMsgDescriptor(id.value(), object_array_value, object_appid_iterator, object_ctxid_iterator);
    }
    return INvConfig::ReadResult::kOK;
}

}  // namespace log
}  // namespace mw
}  // namespace score
   //
