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
#ifndef SCORE_MW_LOG_DETAIL_CONFIG_FILE_DISCOVERER_MOCK_H
#define SCORE_MW_LOG_DETAIL_CONFIG_FILE_DISCOVERER_MOCK_H

#include "score/mw/log/configuration/iconfiguration_file_discoverer.h"

#include "gmock/gmock.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

class ConfigurationFileDiscovererMock final : public IConfigurationFileDiscoverer
{
  public:
    /* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL,AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: */
    /* Not actual for mock class; Internal GTest Framework code caused the violation; */
    /* KW_SUPPRESS_START:MISRA.FUNC.UNUSEDPAR.UNNAMED,MISRA.MEMB.NOT_PRIVATE,MISRA.USE.EXPANSION: */
    /* Not actual for mock class; Internal GTest Framework code caused the violation; */
    MOCK_METHOD((std::vector<std::string>), FindConfigurationFiles, (), (const, noexcept, override));
    /* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL,AUTOSAR.STYLE.SINGLE_STMT_PER_LINE */
    /* KW_SUPPRESS_END:MISRA.FUNC.UNUSEDPAR.UNNAMED,MISRA.MEMB.NOT_PRIVATE,MISRA.USE.EXPANSION */
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_ICONFIG_FILE_DISCOVERER_H
