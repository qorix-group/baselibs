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
#ifndef SCORE_LIB_OS_UTILS_SIGNAL_H
#define SCORE_LIB_OS_UTILS_SIGNAL_H

#include <csignal>
#include <cstdint>
#include <memory>

#include <score/expected.hpp>

#include "score/memory.hpp"
#include "score/os/errno.h"

namespace score
{
namespace os
{

class Signal
{
  public:
    virtual ~Signal() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Signal(const Signal&) = delete;
    Signal& operator=(const Signal&) = delete;
    Signal(Signal&& other) = delete;
    Signal& operator=(Signal&& other) = delete;

    using Sighandler = void(std::int32_t);

    [[deprecated(
        "SPP_DEPRECATION: Please use CamelCase API. Removed in SP25-07-A450. (Ticket-61833)")]] virtual std::int32_t
    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: add_termination_signal is not a member variable, it is a method */
    add_termination_signal(sigset_t& add_signal) = 0;
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: add_termination_signal is not a member variable, it is a method */

    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: AddTerminationSignal is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    virtual score::cpp::expected<std::int32_t, Error> AddTerminationSignal(sigset_t& add_signal) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: AddTerminationSignal is not a member variable, it is a method */

    [[deprecated("SPP_DEPRECATION: Please use CamelCase API. Removed in SP25-07-A450. (Ticket-61833)")]] virtual void
    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: send_self_sigterm is not a member variable, it is a method */
    send_self_sigterm() = 0;
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: send_self_sigterm is not a member variable, it is a method */

    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: SendSelfSigterm is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    virtual score::cpp::expected<std::int32_t, Error> SendSelfSigterm() const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: SendSelfSigterm is not a member variable, it is a method */

    [[deprecated(
        "SPP_DEPRECATION: Please use CamelCase API. Removed in SP25-07-A450. (Ticket-61833)")]] virtual std::int32_t
    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: get_current_blocked_signals is not a member variable, it is a method */
    get_current_blocked_signals(sigset_t& signals) const = 0;
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: get_current_blocked_signals is not a member variable, it is a method */

    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: GetCurrentBlockedSignals is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    virtual score::cpp::expected<std::int32_t, Error> GetCurrentBlockedSignals(sigset_t& signals) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: GetCurrentBlockedSignals is not a member variable, it is a method */

    [[deprecated(
        "SPP_DEPRECATION: Please use CamelCase API. Removed in SP25-07-A450. (Ticket-61833)")]] virtual std::int32_t
    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: is_signal_block is not a member variable, it is a method */
    // returns 1 for is blocked 0 for non blocking, -1 on error
    is_signal_block(const std::int32_t signal_id) = 0;
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: is_signal_block is not a member variable, it is a method */

    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: IsSignalBlocked is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    virtual score::cpp::expected<std::int32_t, Error> IsSignalBlocked(const std::int32_t signal_id) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: IsSignalBlocked is not a member variable, it is a method */

    [[deprecated(
        "SPP_DEPRECATION: Please use CamelCase API. Removed in SP25-07-A450. (Ticket-61833)")]] virtual std::int32_t
    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: is_member is not a member variable, it is a method */
    is_member(const std::int32_t signal_id, sigset_t& signals) = 0;
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: is_member is not a member variable, it is a method */

    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: SigIsMember is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    virtual score::cpp::expected<std::int32_t, Error> SigIsMember(sigset_t& signals,
                                                           const std::int32_t signal_id) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: SigIsMember is not a member variable, it is a method */

    [[deprecated("SPP_DEPRECATION: Please use CamelCase API. Removed in SP25-07-A450. (Ticket-61833)")]]
    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: sigfillset is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual std::int32_t sigfillset(sigset_t* set) = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: sigfillset is not a member variable, it is a method */

    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: SigFillSet is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    virtual score::cpp::expected<std::int32_t, Error> SigFillSet(sigset_t& set) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: SigFillSet is not a member variable, it is a method */

    [[deprecated("SPP_DEPRECATION: Please use CamelCase API. Removed in SP25-07-A450. (Ticket-61833)")]]
    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: sigemptyset is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual std::int32_t sigemptyset(sigset_t* set) const = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: sigemptyset is not a member variable, it is a method */

    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: SigEmptySet is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    virtual score::cpp::expected<std::int32_t, Error> SigEmptySet(sigset_t& set) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: SigEmptySet is not a member variable, it is a method */

    [[deprecated("SPP_DEPRECATION: Please use CamelCase API. Removed in SP25-07-A450. (Ticket-61833)")]]
    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: sigwait is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual std::int32_t sigwait(const sigset_t* set, std::int32_t* sig) = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: sigwait is not a member variable, it is a method */

    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: SigWait is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    virtual score::cpp::expected<std::int32_t, Error> SigWait(const sigset_t& set, std::int32_t& sig) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: SigWait is not a member variable, it is a method */

    [[deprecated("SPP_DEPRECATION: Please use CamelCase API. Removed in SP25-07-A450. (Ticket-61833)")]]
    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: sigaddset is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual std::int32_t sigaddset(sigset_t* set, const std::int32_t signo) = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: sigaddset is not a member variable, it is a method */

    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: SigAddSet is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    virtual score::cpp::expected<std::int32_t, Error> SigAddSet(sigset_t& set, const std::int32_t signo) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: SigAddSet is not a member variable, it is a method */

    [[deprecated("SPP_DEPRECATION: Please use CamelCase API. Removed in SP25-07-A450. (Ticket-61833)")]]
    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: pthread_sigmask is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual std::int32_t pthread_sigmask(sigset_t& signals) const = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: pthread_sigmask is not a member variable, it is a method */

    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: PthreadSigMask is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    virtual score::cpp::expected<std::int32_t, Error> PthreadSigMask(const sigset_t& signals) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: PthreadSigMask is not a member variable, it is a method */

    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: PthreadSigMask is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    virtual score::cpp::expected<std::int32_t, Error> PthreadSigMask(const std::int32_t how,
                                                              const sigset_t& set) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: PthreadSigMask is not a member variable, it is a method */

    [[deprecated("SPP_DEPRECATION: Please use CamelCase API. Removed in SP25-07-A450. (Ticket-61833)")]]
    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: pthread_sigmask is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual std::int32_t pthread_sigmask(const std::int32_t how, const sigset_t* set, sigset_t* oldset) const = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: pthread_sigmask is not a member variable, it is a method */

    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: PthreadSigMask is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    virtual score::cpp::expected<std::int32_t, Error> PthreadSigMask(const std::int32_t how,
                                                              const sigset_t& set,
                                                              sigset_t& oldset) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: PthreadSigMask is not a member variable, it is a method */

    [[deprecated("SPP_DEPRECATION: Please use CamelCase API. Removed in SP25-07-A450. (Ticket-61833)")]]
    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: sigaction is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual std::int32_t sigaction(const std::int32_t sig, const struct sigaction* act, struct sigaction* oact) = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: sigaction is not a member variable, it is a method */

    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: SigAction is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    virtual score::cpp::expected<std::int32_t, Error> SigAction(const std::int32_t signum,
                                                         const struct sigaction& action,
                                                         struct sigaction& old_action) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: SigAction is not a member variable, it is a method */

    [[deprecated("SPP_DEPRECATION: Please use CamelCase API. Removed in SP25-07-A450. (Ticket-61833)")]]
    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: kill is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual std::int32_t kill(const pid_t pid, const std::int32_t sig) = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: kill is not a member variable, it is a method */

    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: Kill is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    virtual score::cpp::expected<std::int32_t, Error> Kill(const pid_t pid, const std::int32_t sig) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: noexcept is not a variable, it cannot be hiding a different variable */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: Kill is not a member variable, it is a method */

    [[deprecated(
        "The behavior varies across UNIX versions and implementations. Please use SigAction instead. Removed in "
        "SP25-07-A450. (Ticket-61833)")]]
    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: signal is not a member variable, it is a method */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_START:MISRA.STDLIB.WRONGNAME:Signal is used to keep wrapper function signature */
    virtual Sighandler* signal(const std::int32_t sig, Sighandler handler) = 0;
    /* KW_SUPPRESS_END:MISRA.STDLIB.WRONGNAME:Signal is used to keep wrapper function signature */
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: signal is not a member variable, it is a method */

  protected:
    Signal() = default;
};

}  // namespace os
}  // namespace score

#endif
