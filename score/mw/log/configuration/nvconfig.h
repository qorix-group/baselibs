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
#ifndef SCORE_MW_LOG_NVCONFIG_H_
#define SCORE_MW_LOG_NVCONFIG_H_

#include "invconfig.h"

#include <unordered_map>

namespace score
{
namespace mw
{
namespace log
{

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
class NvConfig : public INvConfig
{
  public:
    using typemap_t = std::unordered_map<std::string, config::NvMsgDescriptor>;

    explicit NvConfig(const std::string& file_path = "/bmw/platform/opt/datarouter/etc/class-id.json");

    ~NvConfig() noexcept override = default;
    NvConfig(const NvConfig&) = default;

    NvConfig& operator=(const NvConfig& other) noexcept
    {
        if (this != &other)
        {
            // json_path_ is const, so we can't assign it
            // Only assign typemap_
            typemap_ = other.typemap_;
        }
        return *this;
    }

    NvConfig(NvConfig&&) noexcept = default;

    NvConfig& operator=(NvConfig&& other) noexcept
    {
        if (this != &other)
        {
            // json_path_ is const, so we can't move-assign it
            // Only move typemap_
            typemap_ = std::move(other.typemap_);
        }
        return *this;
    }

    INvConfig::ReadResult parseFromJson() noexcept override;
    const config::NvMsgDescriptor* getDltMsgDesc(const std::string& typeName) const noexcept override;

  private:
    const std::string json_path_;
    typemap_t typemap_;
};

}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_NVCONFIG_H_
