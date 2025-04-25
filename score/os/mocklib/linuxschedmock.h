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
#ifndef BASELIBS_SCORE_OS_LINUX_MOCKLIB_LINUX_SCHED_MOCK_H_
#define BASELIBS_SCORE_OS_LINUX_MOCKLIB_LINUX_SCHED_MOCK_H_

#include "score/os/linux/linuxsched.h"
#include <gmock/gmock.h>

namespace score
{
namespace os
{

class LinuxSchedMock
{
  public:
    LinuxSchedMock();
    /* KW_SUPPRESS_START:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False positive */
    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE:Caused by MOCK_METHOD macros */
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:This implementation is intended */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Shadowing function name is intended */
    MOCK_METHOD(std::int32_t, sched_getcpu_os, ());
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN */
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE */
    /* KW_SUPPRESS_END:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE */
};

} /* namespace os */
} /* namespace score */

#endif  // BASELIBS_SCORE_OS_LINUX_MOCKLIB_LINUX_SCHED_MOCK_H_
