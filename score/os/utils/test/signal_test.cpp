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
#include "score/os/utils/signal_impl.h"

#include <gtest/gtest.h>
#include <unistd.h>
#include <string.h>
#include <csignal>
#include <future>
#include <thread>

namespace score
{
namespace os
{
namespace
{

using ::testing::Test;

class SignalTest : public ::testing::Test
{
  protected:
    void block_signal(int32_t signo)
    {
        sigset_t sigset{};
        auto val = unit_->sigaddset(&sigset, signo);
        EXPECT_EQ(val, 0) << "Error: " << strerror(errno);
        val = unit_->pthread_sigmask(SIG_BLOCK, &sigset, nullptr);
        EXPECT_EQ(val, 0) << "Error: " << strerror(errno);
    }

    void unblock_signal(int32_t signo)
    {
        sigset_t sigset{};
        auto val = unit_->sigaddset(&sigset, signo);
        EXPECT_EQ(val, 0) << "Error: " << strerror(errno);
        val = unit_->pthread_sigmask(SIG_UNBLOCK, &sigset, nullptr);
        EXPECT_EQ(val, 0) << "Error: " << strerror(errno);
    }

    void BlockSignal(int32_t signo)
    {
        sigset_t sigset{};
        auto val = unit_->SigAddSet(sigset, signo);
        EXPECT_TRUE(val.has_value());
        EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
        val = unit_->PthreadSigMask(SIG_BLOCK, sigset);
        EXPECT_TRUE(val.has_value());
        EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
    }

    void UnblockSignal(int32_t signo)
    {
        sigset_t sigset{};
        auto val = unit_->SigAddSet(sigset, signo);
        EXPECT_TRUE(val.has_value());
        EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
        val = unit_->PthreadSigMask(SIG_UNBLOCK, sigset);
        EXPECT_TRUE(val.has_value());
        EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
    }

    std::unique_ptr<Signal> unit_{std::make_unique<SignalImpl>()};
};

TEST_F(SignalTest, handler_should_be_called)
{
    static bool triggered = false;
    unit_->signal(SIGUSR1, [](int) {
        triggered = true;
    });

    raise(SIGUSR1);
    EXPECT_TRUE(triggered);
    unit_->signal(SIGUSR1, SIG_DFL);
}

TEST_F(SignalTest, is_not_a_member_works)
{
    sigset_t sigset{};
    const auto val = unit_->is_member(SIGUSR1, sigset);
    EXPECT_EQ(val, 0) << "Error: " << strerror(errno);
}

TEST_F(SignalTest, check_if_sig_set_is_empty_works)
{
    sigset_t sigset{};
    auto val = unit_->sigaddset(&sigset, SIGUSR1);
    EXPECT_EQ(val, 0) << "Error: " << strerror(errno);
    val = unit_->sigemptyset(&sigset);
    EXPECT_EQ(val, 0) << "Error: " << strerror(errno);
    val = unit_->is_member(SIGUSR1, sigset);
    EXPECT_EQ(val, 0) << "Error: " << strerror(errno);
}

TEST_F(SignalTest, get_current_blocked_signals)
{
    sigset_t sigset{};
    block_signal(SIGUSR1);
    auto val = unit_->get_current_blocked_signals(sigset);
    EXPECT_EQ(val, 0) << "Error: " << strerror(errno);
    EXPECT_EQ(::sigismember(&sigset, SIGUSR1), 1) << "Error: " << strerror(errno);
    unblock_signal(SIGUSR1);
}

TEST_F(SignalTest, is_signal_blocked)
{
    block_signal(SIGUSR1);
    auto val = unit_->is_signal_block(SIGUSR1);
    EXPECT_EQ(val, 1) << "Error: " << strerror(errno);
    unblock_signal(SIGUSR1);
}

TEST_F(SignalTest, pthread_sig_mask)
{
    static bool triggered = false;
    unit_->signal(SIGUSR1, [](int) {
        triggered = true;
    });
    sigset_t sigset{};
    auto val = unit_->sigaddset(&sigset, SIGUSR1);
    EXPECT_EQ(val, 0) << "Error: " << strerror(errno);
    val = unit_->pthread_sigmask(sigset);
    EXPECT_EQ(val, 0) << "Error: " << strerror(errno);
    raise(SIGUSR1);
    // As mask is set. triggered should not be updated
    EXPECT_FALSE(triggered);

    // cleanup
    val = unit_->sigemptyset(&sigset);
    EXPECT_EQ(val, 0) << "Error: " << strerror(errno);
    val = unit_->pthread_sigmask(sigset);
    EXPECT_EQ(val, 0) << "Error: " << strerror(errno);
}

TEST_F(SignalTest, send_self_sig_term)
{
    static bool triggered = false;
    unit_->signal(SIGTERM, [](int) {
        triggered = true;
    });

    unit_->send_self_sigterm();
    EXPECT_TRUE(triggered);
    unit_->signal(SIGTERM, SIG_DFL);
}

TEST_F(SignalTest, sig_action)
{
    static bool triggered = false;
    struct sigaction sig_handler;
    struct sigaction old_sig_handler;
    memset(static_cast<void*>(&sig_handler), 0, sizeof(sig_handler));
    memset(static_cast<void*>(&old_sig_handler), 0, sizeof(old_sig_handler));

    sig_handler.sa_flags = SA_SIGINFO;
    sig_handler.sa_handler = [](int) {
        triggered = true;
    };

    auto val = unit_->sigaction(SIGUSR1, &sig_handler, &old_sig_handler);
    EXPECT_EQ(val, 0) << "Error: " << strerror(errno);
    raise(SIGUSR1);
    EXPECT_TRUE(triggered);

    // cleanup
    val = unit_->sigaction(SIGUSR1, &old_sig_handler, nullptr);
    EXPECT_EQ(val, 0) << "Error: " << strerror(errno);
}

TEST_F(SignalTest, kill)
{
    static bool triggered = false;
    unit_->signal(SIGTERM, [](int) {
        triggered = true;
    });
    unit_->kill(getpid(), SIGTERM);
    EXPECT_TRUE(triggered);
    unit_->signal(SIGTERM, SIG_DFL);
}

TEST_F(SignalTest, sig_fill_set_works)
{
    sigset_t sigset{};
    auto val = unit_->sigfillset(&sigset);
    EXPECT_EQ(val, 0) << "Error: " << strerror(errno);
    val = unit_->is_member(SIGUSR1, sigset);
    EXPECT_EQ(val, 1) << "Error: " << strerror(errno);
}

TEST_F(SignalTest, add_termination_signal_works)
{
    sigset_t sigset{};
    auto val = unit_->add_termination_signal(sigset);
    val = unit_->is_member(SIGTERM, sigset);
    EXPECT_EQ(val, 1) << "Error: " << strerror(errno);
}

TEST_F(SignalTest, isNotAMemberWorks)
{
    sigset_t sigset{};
    const auto val = unit_->SigIsMember(sigset, SIGUSR1);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
}

TEST_F(SignalTest, CheckIfSigSetIsEmptyWorks)
{
    sigset_t sigset{};
    auto val = unit_->SigAddSet(sigset, SIGUSR1);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);

    val = unit_->SigEmptySet(sigset);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);

    val = unit_->SigIsMember(sigset, SIGUSR1);
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
}

TEST_F(SignalTest, GetCurrentBlockedSignals)
{
    BlockSignal(SIGUSR1);

    sigset_t sigset{};
    auto val = unit_->GetCurrentBlockedSignals(sigset);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
    EXPECT_EQ(::sigismember(&sigset, SIGUSR1), 1) << "Error: " << strerror(errno);

    UnblockSignal(SIGUSR1);
}

TEST_F(SignalTest, IsSignalBlocked)
{
    BlockSignal(SIGUSR1);

    auto val = unit_->IsSignalBlocked(SIGUSR1);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 1) << "Error: " << strerror(errno);

    UnblockSignal(SIGUSR1);
}

TEST_F(SignalTest, PthreadSigMask)
{
    static bool triggered = false;
    unit_->signal(SIGUSR1, [](int) {
        triggered = true;
    });

    sigset_t sigset{};
    auto val = unit_->SigAddSet(sigset, SIGUSR1);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);

    val = unit_->PthreadSigMask(sigset);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
    raise(SIGUSR1);
    // As mask is set. triggered should not be updated
    EXPECT_FALSE(triggered);

    // cleanup
    val = unit_->SigEmptySet(sigset);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);

    val = unit_->PthreadSigMask(sigset);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
}

TEST_F(SignalTest, PthreadSigMaskReturnsOldSet)
{
    static bool triggered = false;
    unit_->signal(SIGUSR1, [](int) {
        triggered = true;
    });

    sigset_t sigset{};
    auto val = unit_->SigAddSet(sigset, SIGUSR1);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);

    val = unit_->PthreadSigMask(SIG_BLOCK, sigset);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
    raise(SIGUSR1);
    // As mask is set. triggered should not be updated
    EXPECT_FALSE(triggered);

    // Unblock signal
    sigset_t oldsigset{};
    val = unit_->PthreadSigMask(SIG_UNBLOCK, sigset, oldsigset);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);

    // Check that the oldsigset contains the previously blocked signals
    val = unit_->SigIsMember(oldsigset, SIGUSR1);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 1) << "Error: " << strerror(errno);
}

TEST_F(SignalTest, SendSelfSigTerm)
{
    static bool triggered = false;
    unit_->signal(SIGTERM, [](int) {
        triggered = true;
    });

    unit_->SendSelfSigterm();
    EXPECT_TRUE(triggered);
    unit_->signal(SIGTERM, SIG_DFL);
}

TEST_F(SignalTest, SigAction)
{
    static bool triggered = false;
    struct sigaction sig_handler;
    struct sigaction old_sig_handler;
    memset(static_cast<void*>(&sig_handler), 0, sizeof(sig_handler));
    memset(static_cast<void*>(&old_sig_handler), 0, sizeof(old_sig_handler));

    sig_handler.sa_flags = SA_SIGINFO;
    sig_handler.sa_handler = [](int) {
        triggered = true;
    };

    auto val = unit_->SigAction(SIGUSR1, sig_handler, old_sig_handler);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
    raise(SIGUSR1);
    EXPECT_TRUE(triggered);

    // cleanup
    val = unit_->SigAction(SIGUSR1, old_sig_handler, sig_handler);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
}

TEST_F(SignalTest, Kill)
{
    static bool triggered = false;
    unit_->signal(SIGTERM, [](int) {
        triggered = true;
    });
    unit_->Kill(getpid(), SIGTERM);
    EXPECT_TRUE(triggered);
    unit_->signal(SIGTERM, SIG_DFL);
}

TEST_F(SignalTest, SigFillSetWorks)
{
    sigset_t sigset{};
    auto val = unit_->SigFillSet(sigset);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
    val = unit_->SigIsMember(sigset, SIGUSR1);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 1) << "Error: " << strerror(errno);
}

TEST_F(SignalTest, AddTerminationSignalWorks)
{
    sigset_t sigset{};
    auto val = unit_->AddTerminationSignal(sigset);
    val = unit_->SigIsMember(sigset, SIGTERM);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 1) << "Error: " << strerror(errno);
}

}  // namespace
}  // namespace os
}  // namespace score
