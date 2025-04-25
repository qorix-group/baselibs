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
#include "score/os/stdio_impl.h"

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
namespace score
{
namespace os
{
/* KW_SUPPRESS_START:MISRA.STDLIB.WRONGNAME:Function is wrapped */
score::cpp::expected<FILE*, Error> StdioImpl::fopen(const char* const filename, const char* const mode) const noexcept
/* KW_SUPPRESS_END:MISRA.STDLIB.WRONGNAME:Function is wrapped */
{ /* KW_SUPPRESS_START:MISRA.STDLIB.STDIO:Used fopen from stdio-wrapper */
    FILE* const ret = ::fopen(filename, mode);
    /* KW_SUPPRESS_END:MISRA.STDLIB.STDIO:Used fopen from stdio-wrapper */
    if (ret == nullptr)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}
/* KW_SUPPRESS_START:MISRA.STDLIB.WRONGNAME:Function is wrapped */
score::cpp::expected_blank<Error> StdioImpl::fclose(FILE* const stream) const noexcept
/* KW_SUPPRESS_END:MISRA.STDLIB.WRONGNAME:Function is wrapped */
{
    /* KW_SUPPRESS_START:MISRA.STDLIB.STDIO:Used fclose from stdio-wrapper */
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:OS library macro */
    /* KW_SUPPRESS_START:MISRA.EXPANSION.UNSAFE:macro from stdio library file used in stdio-wrapper */
    if (::fclose(stream) == EOF)
    /* KW_SUPPRESS_END:MISRA.EXPANSION.UNSAFE:macro from stdio library file used in stdio-wrapper */
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:OS library macro */
    /* KW_SUPPRESS_END:MISRA.STDLIB.STDIO:Used fclose from stdio-wrapper */
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}
/* KW_SUPPRESS_START:MISRA.STDLIB.WRONGNAME:Function is wrapped */
score::cpp::expected_blank<Error> StdioImpl::remove(const char* const pathname) const noexcept
/* KW_SUPPRESS_END:MISRA.STDLIB.WRONGNAME:Function is wrapped */
{
    /* KW_SUPPRESS_START:MISRA.STDLIB.STDIO:Used remove from stdio-wrapper */
    if (::remove(pathname) != 0)
    /* KW_SUPPRESS_END:MISRA.STDLIB.STDIO:Used remove from stdio-wrapper */
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}
/* KW_SUPPRESS_START:MISRA.STDLIB.WRONGNAME:Function is wrapped */
score::cpp::expected_blank<Error> StdioImpl::rename(const char* const oldname, const char* const newname) const noexcept
/* KW_SUPPRESS_END:MISRA.STDLIB.WRONGNAME:Function is wrapped */
{
    /* KW_SUPPRESS_START:MISRA.STDLIB.STDIO:Used rename from stdio-wrapper */
    if (::rename(oldname, newname) != 0)
    /* KW_SUPPRESS_END:MISRA.STDLIB.STDIO:Used rename from stdio-wrapper */
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected<FILE*, Error> StdioImpl::popen(const char* const filename, const char* const mode) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    FILE* const ret = ::popen(filename, mode);
    if (ret == nullptr)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected<std::int32_t, Error> StdioImpl::pclose(FILE* const stream) const noexcept
{
    // Manual code analysis:
    // ::pclose() only fails if the termination status of the child could not be retrieved or a signal interrupted
    // the call. Neither is easily simulated in a unit test. On failure, ::pclose will return -1 and set errno.
    // (POSIX 1003.1)
    const std::int32_t ret = ::pclose(stream);
    if (ret == -1)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());  // LCOV_EXCL_LINE
    }
    return ret;
}

score::cpp::expected<std::int32_t, Error> StdioImpl::fileno(FILE* const stream) const noexcept
{
    const std::int32_t ret = ::fileno(stream);
    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}

}  // namespace os
}  // namespace score

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
