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
#ifndef SCORE_LIB_OS_TIME_H
#define SCORE_LIB_OS_TIME_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

#include <cstdint>
#include <ctime>

namespace score
{
namespace os
{

class Time : public ObjectSeam<Time>
{
  public:
    /// \brief thread-safe singleton accessor
    /// \return Either concrete OS-dependent instance or respective set mock instance
    static Time& instance() noexcept;
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, Error> clock_settime(const clockid_t clkid,
                                                             const struct timespec* const tp) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, Error> clock_gettime(const clockid_t clkid,
                                                             struct timespec* const tp) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, Error> clock_getres(const clockid_t clkid,
                                                            struct timespec* const res) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual struct tm* localtime_r(const time_t* timer, struct tm* tm_local_time) const noexcept = 0;

    virtual ~Time() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Time(const Time&) = delete;
    Time& operator=(const Time&) = delete;
    Time(Time&& other) = delete;
    Time& operator=(Time&& other) = delete;

  protected:
    Time() = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_TIME_H
