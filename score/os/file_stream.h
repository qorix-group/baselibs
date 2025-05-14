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
#ifndef SCORE_LIB_OS_LINUX_FILE_STREAM_H
#define SCORE_LIB_OS_LINUX_FILE_STREAM_H

#include "score/os/ObjectSeam.h"

/* KW_SUPPRESS_START:MISRA.INCL.UNSAFE:Header needed for wrapper function */
// Suppressed here because POSIX method accepts pointer to FILE
// NOLINTBEGIN(score-banned-include) see comment above
// Supress autosar_cpp14_m27_0_1_violation, rule finding "The stream input/output library <cstdio> shall not be used."
// Rationale: Header needed for wrapper function. Suppressed here because POSIX method accepts pointer to FILE
// coverity[autosar_cpp14_m27_0_1_violation]
#include <cstdio>
// NOLINTEND(score-banned-include)

namespace score
{
namespace os
{

class FileStream : public ObjectSeam<FileStream>
{
  public:
    static FileStream& instance() noexcept;

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual void setlinebuf(FILE* stream) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    virtual ~FileStream() = default;

  protected:
    FileStream() = default;
    FileStream(const FileStream&) = default;
    FileStream(FileStream&&) = default;
    FileStream& operator=(const FileStream&) = default;
    FileStream& operator=(FileStream&&) = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_LINUX_FILE_STREAM_H
