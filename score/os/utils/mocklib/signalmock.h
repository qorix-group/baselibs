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
#ifndef SCORE_LIB_OS_UTILS_MOCKLIB_SIGNALMOCK_H
#define SCORE_LIB_OS_UTILS_MOCKLIB_SIGNALMOCK_H

#include "score/os/utils/signal.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class SignalMock : public Signal
{
  public:
    MOCK_METHOD(std::int32_t, add_termination_signal, (sigset_t&), (override));
    MOCK_METHOD(std::int32_t, pthread_sigmask, (sigset_t&), (const, override));
    MOCK_METHOD(std::int32_t,
                pthread_sigmask,
                (const std::int32_t, const sigset_t* set, sigset_t* oldset),
                (const, override));
    MOCK_METHOD(void, send_self_sigterm, (), (override));

    MOCK_METHOD(Sighandler*, signal, (const std::int32_t sig, Sighandler handler), (override));
    MOCK_METHOD(std::int32_t, kill, (const pid_t pid, const std::int32_t sig), (override));
    MOCK_METHOD(std::int32_t, is_signal_block, (const std::int32_t signal_id), (override));
    MOCK_METHOD(std::int32_t,
                sigaction,
                (const std::int32_t sig, const struct sigaction* act, struct sigaction* oact),
                (override));
    MOCK_METHOD(std::int32_t, get_current_blocked_signals, (sigset_t & signals), (const, override));

    MOCK_METHOD(std::int32_t, sigfillset, (sigset_t * set), (override));
    MOCK_METHOD(std::int32_t, sigemptyset, (sigset_t * set), (const, override));
    MOCK_METHOD(std::int32_t, sigwait, (const sigset_t* set, std::int32_t* sig), (override));
    MOCK_METHOD(std::int32_t, sigaddset, (sigset_t * set, std::int32_t signo), (override));
    MOCK_METHOD(std::int32_t, is_member, (const std::int32_t signal_id, sigset_t& signals), (override));

    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>), SigEmptySet, (sigset_t & set), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                SigAddSet,
                (sigset_t & set, const std::int32_t signo),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                PthreadSigMask,
                (const sigset_t& signals),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                PthreadSigMask,
                (const std::int32_t how, const sigset_t& set),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                PthreadSigMask,
                (const std::int32_t how, const sigset_t& set, sigset_t& oldset),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                AddTerminationSignal,
                (sigset_t & add_signal),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                GetCurrentBlockedSignals,
                (sigset_t & signals),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                IsSignalBlocked,
                (const std::int32_t signal_id),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                SigIsMember,
                (sigset_t & signals, const std::int32_t signal_id),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>), SigFillSet, (sigset_t & set), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                SigWait,
                (const sigset_t& set, std::int32_t& sig),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                SigAction,
                (const std::int32_t signum, const struct sigaction& action, struct sigaction& old_action),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                Kill,
                (const pid_t pid, const std::int32_t signal),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>), SendSelfSigterm, (), (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif
