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
#ifndef SCORE_LIB_OS_TIME_IMPL_H
#define SCORE_LIB_OS_TIME_IMPL_H

#include "score/os/time.h"

namespace score
{
namespace os
{

class TimeImpl final : public Time
{
  public:
    constexpr TimeImpl() = default;
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<std::int32_t, Error> clock_settime(const clockid_t clkid,
                                                     const struct timespec* const tp) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<std::int32_t, Error> clock_gettime(const clockid_t clkid,
                                                     struct timespec* const tp) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<std::int32_t, Error> clock_getres(const clockid_t clkid,
                                                    struct timespec* const res) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    struct tm* localtime_r(const time_t* timer, struct tm* tm_local_time) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_TIME_IMPL_H
