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
#include "score/os/file_stream.h"

#if defined(__QNX__)
#include <unix.h>
#endif

namespace score
{
namespace os
{

namespace
{

class FileStreamImpl final : public FileStream
{
    /* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    void setlinebuf(FILE* stream) const noexcept override
    {
        ::setlinebuf(stream);
    }
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
};

}  // namespace
}  // namespace os
}  // namespace score

score::os::FileStream& score::os::FileStream::instance() noexcept
{
    static score::os::FileStreamImpl instance;
    return select_instance(instance);
}
