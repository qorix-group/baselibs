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
#ifndef SCORE_LIB_OS_MOCKLIB_MOCK_SELECT_H
#define SCORE_LIB_OS_MOCKLIB_MOCK_SELECT_H

#include "score/os/select.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace os
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Shaddowing function name is intended. */

class MockSelect : public Select
{
  public:
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                select,
                (const std::int32_t, fd_set*, fd_set*, fd_set*, struct timeval*),
                (const, noexcept, override));
};

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Shaddowing function name is intended. */

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_MOCK_SELECT_H
