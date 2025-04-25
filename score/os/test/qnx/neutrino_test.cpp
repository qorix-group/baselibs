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
#include "score/os/mocklib/qnx/neutrino_qnx_mock.h"
#include "score/os/qnx/channel_impl.h"
#include "score/os/qnx/dispatch_impl.h"
#include "score/os/qnx/neutrino_impl.h"
#include <score/jthread.hpp>
#include <sys/neutrino.h>
#include <condition_variable>
#include <future>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using Neutrino = score::os::qnx::Neutrino;

namespace
{
static constexpr auto kTimeout = std::chrono::milliseconds{100};
static constexpr auto kClockType{Neutrino::ClockType::kRealtime};
static constexpr auto kTimeoutFlags{Neutrino::TimerTimeoutFlag::kSend | Neutrino::TimerTimeoutFlag::kReply};
}  // namespace

using namespace testing;

struct NeutrinoMockTest : ::testing::Test
{
    void SetUp() override
    {
        unblock_event.sigev_notify = SIGEV_UNBLOCK;
        neutrino_mock_ptr = std::make_shared<score::os::qnx::NeutrinoMock>();
        neutrino_ptr = neutrino_mock_ptr;
    }

    void TearDown() override
    {
        EXPECT_TRUE(::testing::Mock::VerifyAndClearExpectations(neutrino_mock_ptr.get()));
        neutrino_ptr.reset();
    }

    std::shared_ptr<score::os::qnx::NeutrinoMock> neutrino_mock_ptr{};
    std::shared_ptr<score::os::qnx::Neutrino> neutrino_ptr{};
    sigevent unblock_event{};
};

TEST_F(NeutrinoMockTest, TimerTimeout)
{
    EXPECT_CALL(*neutrino_mock_ptr, TimerTimeout(kClockType, _, _, _, _)).Times(1);
    neutrino_ptr->TimerTimeout(kClockType, kTimeoutFlags, nullptr, kTimeout, std::nullopt);
}

TEST_F(NeutrinoMockTest, ClockAdjust)
{
    _clockadjust adjustment{0, 0};

    EXPECT_CALL(*neutrino_mock_ptr, ClockAdjust);
    neutrino_ptr->ClockAdjust(CLOCK_REALTIME, &adjustment, nullptr);
}

TEST_F(NeutrinoMockTest, ThreadCtl)
{
    const std::int32_t cmd = _NTO_TCTL_IO;

    EXPECT_CALL(*neutrino_mock_ptr, ThreadCtl);
    neutrino_ptr->ThreadCtl(cmd, nullptr);
}

TEST_F(NeutrinoMockTest, InterruptWait_r)
{
    const std::int32_t flags = 0;

    EXPECT_CALL(*neutrino_mock_ptr, InterruptWait_r);
    neutrino_ptr->InterruptWait_r(flags, nullptr);
}

TEST_F(NeutrinoMockTest, InterruptAttachEvent)
{
    const std::int32_t intr = _NTO_INTR_CLASS_EXTERNAL;
    const unsigned flags = _NTO_INTR_FLAGS_END;

    EXPECT_CALL(*neutrino_mock_ptr, InterruptAttachEvent);
    neutrino_ptr->InterruptAttachEvent(intr, &unblock_event, flags);
}

TEST_F(NeutrinoMockTest, InterruptDetach)
{
    const std::int32_t id = 0;

    EXPECT_CALL(*neutrino_mock_ptr, InterruptDetach);
    neutrino_ptr->InterruptDetach(id);
}

TEST_F(NeutrinoMockTest, InterruptUnmask)
{
    const std::int32_t intr = _NTO_INTR_CLASS_EXTERNAL;
    const std::int32_t id = 0;

    EXPECT_CALL(*neutrino_mock_ptr, InterruptUnmask);
    neutrino_ptr->InterruptUnmask(intr, id);
}

TEST_F(NeutrinoMockTest, ChannelCreate)
{
    const auto flags{Neutrino::ChannelFlag::kConnectionIdDisconnect};

    EXPECT_CALL(*neutrino_mock_ptr, ChannelCreate(flags));
    neutrino_ptr->ChannelCreate(flags);
}

struct NeutrinoTest : ::testing::Test
{
    void SetUp() override
    {
        /// TODO: To be fixed by Ticket-180848
        GTEST_SKIP() << "This entire test suite is currently disabled.";
        neutrino_ = std::make_unique<score::os::qnx::NeutrinoImpl>();
        dispatch_ = std::make_unique<score::os::DispatchImpl>();
        channel_ = std::make_unique<score::os::ChannelImpl>();

        memset(sample_request, 0, kMinMessageSize);
        memset(sample_response, 0, kMinMessageSize);

        memcpy(sample_request, kSampleRequestMessage.c_str(), kSampleRequestMessage.size());
        memcpy(sample_response, kSampleReponseMessage.c_str(), kSampleReponseMessage.size());
    }

    void TearDown() override
    {
        memset(sample_request, 0, kMinMessageSize);
        memset(sample_response, 0, kMinMessageSize);
    }

    score::cpp::expected<std::tuple<std::int32_t, name_attach_t*, dispatch_t*>, score::os::Error> CreateNamedServer(
        const Neutrino::ChannelFlag flags = Neutrino::ChannelFlag::kPrivate,
        const std::string& server_name = kServerName);
    score::cpp::expected_blank<score::os::Error> DestroyNamedServer(name_attach_t* name_attach, dispatch_t* dispatch);
    score::cpp::expected<std::int32_t, score::os::Error> CreateNamedServerClient(const std::string& server_name = kServerName);
    score::cpp::expected_blank<score::os::Error> DestroyClient(const std::int32_t channel_id);
    score::cpp::expected<std::int32_t, score::os::Error> SetTimerTimeout(const Neutrino::ClockType clock_type,
                                                                const Neutrino::TimerTimeoutFlag flags,
                                                                const std::chrono::nanoseconds timeout);
    score::cpp::expected_blank<score::os::Error> SendMessage(
        const std::int32_t connection_id,
        const iov_t& request,
        iov_t& response,
        std::optional<
            std::tuple<const Neutrino::ClockType, const Neutrino::TimerTimeoutFlag, const std::chrono::nanoseconds>>
            timeout = std::nullopt);
    score::cpp::expected<std::int32_t, score::os::Error> ReceiveMessage(
        const std::int32_t channel_id,
        iov_t& request,
        std::optional<
            std::tuple<const Neutrino::ClockType, const Neutrino::TimerTimeoutFlag, const std::chrono::nanoseconds>>
            timeout = std::nullopt,
        std::optional<_msg_info> message_info = std::nullopt);
    score::cpp::expected_blank<score::os::Error> ReplyMessage(const std::int32_t receive_message_id, const iov_t& response);

    static constexpr auto kServerName = "test_server_name";
    static constexpr auto kMinMessageSize = sizeof(_pulse);

    char sample_request[kMinMessageSize];
    char sample_response[kMinMessageSize];
    const std::string kSampleRequestMessage{"SampleRequest"};
    const std::string kSampleReponseMessage{"SampleResponse"};

    std::unique_ptr<score::os::qnx::Neutrino> neutrino_;
    std::unique_ptr<score::os::Dispatch> dispatch_;
    std::unique_ptr<score::os::Channel> channel_;
    std::mutex name_open_mutex;
    std::mutex name_close_mutex;
};

score::cpp::expected_blank<score::os::Error> NeutrinoTest::DestroyNamedServer(name_attach_t* name_attach, dispatch_t* dispatch)
{
    const auto name_detach = dispatch_->name_detach(name_attach, NAME_FLAG_DETACH_SAVEDPP);
    if (!name_detach.has_value())
    {
        return score::cpp::make_unexpected(name_detach.error());
    }

    const auto dispatch_destroy = dispatch_->dispatch_destroy(dispatch);
    if (!dispatch_destroy.has_value())
    {
        return score::cpp::make_unexpected(dispatch_destroy.error());
    }
    return {};
}

score::cpp::expected<std::tuple<std::int32_t, name_attach_t*, dispatch_t*>, score::os::Error> NeutrinoTest::CreateNamedServer(
    const Neutrino::ChannelFlag flags,
    const std::string& server_name)
{
    const auto channel_id = neutrino_->ChannelCreate(flags);
    if (!channel_id.has_value())
    {
        return score::cpp::make_unexpected(channel_id.error());
    }

    const auto dispatch_channel = dispatch_->dispatch_create_channel(channel_id.value(), 0);
    if (!dispatch_channel.has_value())
    {
        return score::cpp::make_unexpected(dispatch_channel.error());
    }

    const auto name_attach = dispatch_->name_attach(dispatch_channel.value(), server_name.c_str(), 0);
    if (!name_attach.has_value())
    {
        return score::cpp::make_unexpected(name_attach.error());
    }
    return std::make_tuple(channel_id.value(), name_attach.value(), dispatch_channel.value());
}

score::cpp::expected<std::int32_t, score::os::Error> NeutrinoTest::CreateNamedServerClient(const std::string& server_name)
{
    std::lock_guard<std::mutex> lock{name_open_mutex};
    return dispatch_->name_open(server_name.c_str(), 0);
}

score::cpp::expected_blank<score::os::Error> NeutrinoTest::DestroyClient(const std::int32_t connection_id)
{
    std::lock_guard<std::mutex> lock{name_close_mutex};
    return dispatch_->name_close(connection_id);
}

score::cpp::expected<std::int32_t, score::os::Error> NeutrinoTest::SetTimerTimeout(const Neutrino::ClockType clock_type,
                                                                          const Neutrino::TimerTimeoutFlag flags,
                                                                          const std::chrono::nanoseconds timeout)
{
    return neutrino_->TimerTimeout(clock_type, flags, nullptr, timeout);
}

score::cpp::expected_blank<score::os::Error> NeutrinoTest::SendMessage(
    const std::int32_t connection_id,
    const iov_t& request,
    iov_t& response,
    std::optional<
        std::tuple<const Neutrino::ClockType, const Neutrino::TimerTimeoutFlag, const std::chrono::nanoseconds>>
        timeout)
{
    if (timeout.has_value())
    {
        const auto clock_type = std::get<0>(timeout.value());
        const auto timeout_flags = std::get<1>(timeout.value());
        const auto timeout_nanoseconds = std::get<2>(timeout.value());
        const auto set_timer_timeout = SetTimerTimeout(clock_type, timeout_flags, timeout_nanoseconds);
        if (!set_timer_timeout.has_value())
        {
            return score::cpp::make_unexpected(set_timer_timeout.error());
        }
    }

    const auto send_message = channel_->MsgSendv(connection_id, &request, 1, &response, 1);
    if (!send_message.has_value())
    {
        return score::cpp::make_unexpected(send_message.error());
    }
    return {};
}

score::cpp::expected<std::int32_t, score::os::Error> NeutrinoTest::ReceiveMessage(
    const std::int32_t channel_id,
    iov_t& request,
    std::optional<
        std::tuple<const Neutrino::ClockType, const Neutrino::TimerTimeoutFlag, const std::chrono::nanoseconds>>
        timeout,
    std::optional<_msg_info> message_info)
{
    if (timeout.has_value())
    {
        const auto clock_type = std::get<0>(timeout.value());
        const auto timeout_flags = std::get<1>(timeout.value());
        const auto timeout_nanoseconds = std::get<2>(timeout.value());
        const auto set_timer_timeout = SetTimerTimeout(clock_type, timeout_flags, timeout_nanoseconds);
        if (!set_timer_timeout.has_value())
        {
            return score::cpp::make_unexpected(set_timer_timeout.error());
        }
    }

    const auto receive_message =
        channel_->MsgReceivev(channel_id, &request, 1, message_info.has_value() ? &message_info.value() : nullptr);
    if (!receive_message.has_value())
    {
        return score::cpp::make_unexpected(receive_message.error());
    }

    return receive_message.value();
}

score::cpp::expected_blank<score::os::Error> NeutrinoTest::ReplyMessage(const std::int32_t receive_message_id,
                                                               const iov_t& reply)
{
    return channel_->MsgReplyv(receive_message_id, 0, &reply, 1);
}

TEST_F(NeutrinoTest, TimerTimeoutCalledOnMessageSend)
{
    std::mutex client_server_sync_mutex;
    std::condition_variable client_server_sync;
    bool is_ready_to_connect = false;
    bool is_ready_for_destruction = false;

    auto server_thread = score::cpp::jthread(
        [this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex, &is_ready_for_destruction]() {
            const auto create_server = CreateNamedServer();
            EXPECT_TRUE(create_server.has_value());
            auto name_attach = std::get<1>(create_server.value());
            auto dispatch = std::get<2>(create_server.value());

            is_ready_to_connect = true;
            client_server_sync.notify_one();

            {
                std::unique_lock<std::mutex> lock(client_server_sync_mutex);
                client_server_sync.wait(lock, [&is_ready_for_destruction]() {
                    return is_ready_for_destruction;
                });
            }
            const auto destroy_named_server = DestroyNamedServer(name_attach, dispatch);
            EXPECT_TRUE(destroy_named_server.has_value());
        });

    auto client_thread = score::cpp::jthread(
        [this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex, &is_ready_for_destruction]() {
            {
                std::unique_lock<std::mutex> lock(client_server_sync_mutex);
                client_server_sync.wait(lock, [&is_ready_to_connect]() {
                    return is_ready_to_connect;
                });
            }

            const auto create_client = CreateNamedServerClient();
            EXPECT_TRUE(create_client.has_value());

            iov_t request;
            channel_->SetIovConst(&request, sample_request, kMinMessageSize);
            char response_message[kMinMessageSize];
            iov_t response;
            channel_->SetIov(&response, &response_message, kMinMessageSize);

            const auto clock_type{Neutrino::ClockType::kRealtime};
            const auto timeout_flag{Neutrino::TimerTimeoutFlag::kSend};
            const auto timeout = std::chrono::milliseconds{50};
            // Send a message which is never received

            const auto send_message = SendMessage(
                create_client.value(), request, response, std::make_tuple(clock_type, timeout_flag, timeout));
            EXPECT_FALSE(send_message.has_value());
            EXPECT_EQ(send_message.error(), score::os::Error::Code::kKernelTimeout);

            const auto destroy_client = DestroyClient(create_client.value());
            EXPECT_TRUE(destroy_client.has_value());
            is_ready_for_destruction = true;
            client_server_sync.notify_one();
        });
}

TEST_F(NeutrinoTest, TimerTimeoutCalledOnMessageReply)
{
    std::mutex client_server_sync_mutex;
    std::condition_variable client_server_sync;
    bool is_ready_to_connect = false;
    bool is_ready_for_destruction = false;

    auto server_thread = score::cpp::jthread(
        [this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex, &is_ready_for_destruction]() {
            const auto create_server = CreateNamedServer();
            EXPECT_TRUE(create_server.has_value());

            auto channel_id = std::get<0>(create_server.value());
            auto name_attach = std::get<1>(create_server.value());
            auto dispatch = std::get<2>(create_server.value());

            is_ready_to_connect = true;
            client_server_sync.notify_one();

            char request_message[kMinMessageSize];
            iov_t request;
            channel_->SetIov(&request, request_message, kMinMessageSize);
            const auto message_receive = ReceiveMessage(channel_id, request);
            EXPECT_TRUE(message_receive.has_value());
            EXPECT_STREQ(request_message, sample_request);

            {
                std::unique_lock<std::mutex> lock(client_server_sync_mutex);
                client_server_sync.wait(lock, [&is_ready_for_destruction]() {
                    return is_ready_for_destruction;
                });
            }
            const auto destroy_named_server = DestroyNamedServer(name_attach, dispatch);
            EXPECT_TRUE(destroy_named_server.has_value());
        });

    auto client_thread = score::cpp::jthread(
        [this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex, &is_ready_for_destruction]() {
            {
                std::unique_lock<std::mutex> lock(client_server_sync_mutex);
                client_server_sync.wait(lock, [&is_ready_to_connect]() {
                    return is_ready_to_connect;
                });
            }

            const auto create_client = CreateNamedServerClient();
            EXPECT_TRUE(create_client.has_value());

            iov_t request;
            channel_->SetIovConst(&request, sample_request, kMinMessageSize);
            char response_message[kMinMessageSize];
            iov_t response;
            channel_->SetIov(&response, &response_message, kMinMessageSize);

            const auto clock_type{Neutrino::ClockType::kRealtime};
            const auto timeout_flag{Neutrino::TimerTimeoutFlag::kReply};
            const auto timeout = std::chrono::milliseconds{50};
            // Send a message which is received but no one replies to it

            const auto send_message = SendMessage(
                create_client.value(), request, response, std::make_tuple(clock_type, timeout_flag, timeout));
            EXPECT_FALSE(send_message.has_value());
            EXPECT_EQ(send_message.error(), score::os::Error::Code::kKernelTimeout);

            const auto destroy_client = DestroyClient(create_client.value());
            EXPECT_TRUE(destroy_client.has_value());

            is_ready_for_destruction = true;
            client_server_sync.notify_one();
        });
}

TEST_F(NeutrinoTest, TimerTimeoutOnMessageReceive)
{
    const auto create_server = CreateNamedServer();
    EXPECT_TRUE(create_server.has_value());

    const auto clock_type{Neutrino::ClockType::kRealtime};
    const auto timeout_flag{Neutrino::TimerTimeoutFlag::kReceive};
    const auto timeout = std::chrono::milliseconds{50};
    char request_message[kMinMessageSize];
    iov_t request;
    channel_->SetIov(&request, request_message, kMinMessageSize);

    const auto message_receive =
        ReceiveMessage(std::get<0>(create_server.value()), request, std::make_tuple(clock_type, timeout_flag, timeout));
    EXPECT_FALSE(message_receive.has_value());
    EXPECT_EQ(message_receive.error(), score::os::Error::Code::kKernelTimeout);

    const auto destroy_named_server =
        DestroyNamedServer(std::get<1>(create_server.value()), std::get<2>(create_server.value()));
    EXPECT_TRUE(destroy_named_server.has_value());
}

TEST_F(NeutrinoTest, TimerTimeoutNeverCalledWrongFlag)
{
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    std::mutex client_server_sync_mutex;
    std::condition_variable client_server_sync;
    bool is_ready_to_connect = false;
    bool is_ready_for_destruction = false;

    auto server_thread = score::cpp::jthread(
        [this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex, &is_ready_for_destruction]() {
            const auto create_server = CreateNamedServer();
            EXPECT_TRUE(create_server.has_value());

            auto channel_id = std::get<0>(create_server.value());
            auto name_attach = std::get<1>(create_server.value());
            auto dispatch = std::get<2>(create_server.value());

            is_ready_to_connect = true;
            client_server_sync.notify_one();

            char request_message[kMinMessageSize];
            iov_t request;
            channel_->SetIov(&request, request_message, kMinMessageSize);
            const auto message_receive = ReceiveMessage(channel_id, request);
            EXPECT_TRUE(message_receive.has_value());
            EXPECT_STREQ(request_message, sample_request);

            {
                std::unique_lock<std::mutex> lock(client_server_sync_mutex);
                client_server_sync.wait(lock, [&is_ready_for_destruction]() {
                    return is_ready_for_destruction;
                });
            }
            const auto destroy_named_server = DestroyNamedServer(name_attach, dispatch);
            EXPECT_TRUE(destroy_named_server.has_value());
        });

    auto client_thread = score::cpp::jthread(
        [this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex, &is_ready_for_destruction]() {
            {
                std::unique_lock<std::mutex> lock(client_server_sync_mutex);
                client_server_sync.wait(lock, [&is_ready_to_connect]() {
                    return is_ready_to_connect;
                });
            }

            const auto create_client = CreateNamedServerClient();
            EXPECT_TRUE(create_client.has_value());

            iov_t request;
            channel_->SetIovConst(&request, sample_request, kMinMessageSize);
            char response_message[kMinMessageSize];
            iov_t response;
            channel_->SetIov(&response, &response_message, kMinMessageSize);

            const auto clock_type{Neutrino::ClockType::kRealtime};
            // here we should expect either kSend or kReply, as the client
            const auto timeout_flag{Neutrino::TimerTimeoutFlag::kReceive};
            const auto timeout = std::chrono::milliseconds{50};
            // Send a message which is received but no one replies to it and we never unblock due to wrong timeout flag
            const auto send_message = SendMessage(
                create_client.value(), request, response, std::make_tuple(clock_type, timeout_flag, timeout));
            EXPECT_EXIT(assert(false), ::testing::KilledBySignal(SIGABRT), "");
        });

    // Wait a bit to ensure that the client does not timeout by itself
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // force the client thread to shut down as it's never unblocked
    pthread_cancel(client_thread.native_handle());

    // shutdown the server explicitly
    is_ready_for_destruction = true;
    client_server_sync.notify_one();
}

TEST_F(NeutrinoTest, TimerTimeoutNeverCalledChannelUnblockFlag)
{
    std::mutex client_server_sync_mutex;
    std::condition_variable client_server_sync;
    bool is_ready_to_connect = false;
    bool is_ready_for_destruction = false;

    auto server_thread = score::cpp::jthread(
        [this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex, &is_ready_for_destruction]() {
            const auto create_server = CreateNamedServer(Neutrino::ChannelFlag::kUnblock);
            EXPECT_TRUE(create_server.has_value());

            auto channel_id = std::get<0>(create_server.value());
            auto name_attach = std::get<1>(create_server.value());
            auto dispatch = std::get<2>(create_server.value());

            is_ready_to_connect = true;
            client_server_sync.notify_one();

            char request_message[kMinMessageSize];
            iov_t request;
            channel_->SetIov(&request, request_message, kMinMessageSize);
            const auto message_receive = ReceiveMessage(channel_id, request);
            EXPECT_TRUE(message_receive.has_value());
            EXPECT_STREQ(request_message, sample_request);

            // sleep for 200ms just to demonstrate that we never unblock
            // regardless of the fact that the client sets a timeout
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            iov_t response;
            channel_->SetIovConst(&response, sample_response, kMinMessageSize);
            const auto message_reply = ReplyMessage(message_receive.value(), response);
            EXPECT_TRUE(message_reply.has_value());

            {
                std::unique_lock<std::mutex> lock(client_server_sync_mutex);
                client_server_sync.wait(lock, [&is_ready_for_destruction]() {
                    return is_ready_for_destruction;
                });
            }
            const auto destroy_named_server = DestroyNamedServer(name_attach, dispatch);
            EXPECT_TRUE(destroy_named_server.has_value());
        });

    auto client_thread = score::cpp::jthread(
        [this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex, &is_ready_for_destruction]() {
            {
                std::unique_lock<std::mutex> lock(client_server_sync_mutex);
                client_server_sync.wait(lock, [&is_ready_to_connect]() {
                    return is_ready_to_connect;
                });
            }

            const auto create_client = CreateNamedServerClient();
            EXPECT_TRUE(create_client.has_value());

            // We are blocked here until we receive a response, no matter the timeout we set
            // The main problem is that if the server sets kUnblock, clients won't even respond to SIGKILL or SIGTERM
            // We simulate that, in the end, server responds after some time...
            iov_t request;
            channel_->SetIovConst(&request, sample_request, kMinMessageSize);
            char response_message[kMinMessageSize];
            iov_t response;
            channel_->SetIov(&response, response_message, kMinMessageSize);

            const auto clock_type{Neutrino::ClockType::kRealtime};
            const auto timeout_flag{Neutrino::TimerTimeoutFlag::kReply | Neutrino::TimerTimeoutFlag::kSend};
            const auto timeout = std::chrono::milliseconds{50};

            const auto before_sending = std::chrono::high_resolution_clock::now();
            const auto send_message = SendMessage(
                create_client.value(), request, response, std::make_tuple(clock_type, timeout_flag, timeout));
            const auto after_receiving = std::chrono::high_resolution_clock::now();
            EXPECT_LT(timeout, after_receiving - before_sending);
            EXPECT_TRUE(send_message.has_value());
            const auto destroy_client = DestroyClient(create_client.value());
            EXPECT_TRUE(destroy_client.has_value());

            is_ready_for_destruction = true;
            client_server_sync.notify_one();
        });
}

TEST_F(NeutrinoTest, TestServerPulseOnClientShutdown)
{
    std::mutex client_server_sync_mutex;
    std::condition_variable client_server_sync;
    bool is_ready_to_connect = false;

    auto server_thread = score::cpp::jthread(
        [this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex](const score::cpp::stop_token& token) {
            // get notified for every client which disconnects
            const auto create_server =
                CreateNamedServer(Neutrino::ChannelFlag::kPrivate | Neutrino::ChannelFlag::kDisconnect);
            EXPECT_TRUE(create_server.has_value());

            auto channel_id = std::get<0>(create_server.value());
            auto name_attach = std::get<1>(create_server.value());
            auto dispatch = std::get<2>(create_server.value());

            is_ready_to_connect = true;
            client_server_sync.notify_all();

            iov_t response;
            channel_->SetIovConst(&response, sample_response, kMinMessageSize);

            char request_message[kMinMessageSize];
            iov_t request;
            channel_->SetIov(&request, request_message, kMinMessageSize);

            const auto clock_type{Neutrino::ClockType::kRealtime};
            const auto timeout_flag{Neutrino::TimerTimeoutFlag::kReceive};
            const auto timeout = std::chrono::milliseconds{50};

            while (!token.stop_requested())
            {
                const auto message_receive =
                    ReceiveMessage(channel_id, request, std::make_tuple(clock_type, timeout_flag, timeout));
                if (!message_receive.has_value())
                {
                    EXPECT_EQ(message_receive.error(), score::os::Error::Code::kKernelTimeout);
                    // you don't reply if you are timed out
                    continue;
                }
                else
                {
                    if (message_receive.value() != 0)
                    {
                        EXPECT_STREQ(request_message, sample_request);
                    }
                    else
                    {
                        const auto pulse = *reinterpret_cast<_pulse*>(request_message);
                        EXPECT_EQ(pulse.code, _PULSE_CODE_DISCONNECT);
                        // we need to destroy the internal server connection
                        // keep in mind that the client coid (from MsgReceive) != scoid from this pulse
                        const auto destroy_client = DestroyClient(pulse.scoid);
                        EXPECT_TRUE(destroy_client.has_value());
                        // you don't reply to a pulse
                        continue;
                    }
                }
                const auto message_reply = ReplyMessage(message_receive.value(), response);
                EXPECT_TRUE(message_reply.has_value());
            }

            const auto destroy_named_server = DestroyNamedServer(name_attach, dispatch);
            EXPECT_TRUE(destroy_named_server.has_value());
        });

    static constexpr auto client_count = 10;
    for (int client_index = 0; client_index < client_count; ++client_index)
    {
        auto client_thread =
            score::cpp::jthread([this, client_index, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex]() {
                {
                    std::unique_lock<std::mutex> lock(client_server_sync_mutex);
                    client_server_sync.wait(lock, [&is_ready_to_connect]() {
                        return is_ready_to_connect;
                    });
                }

                const auto create_client = CreateNamedServerClient();
                EXPECT_TRUE(create_client.has_value());

                iov_t request;
                channel_->SetIovConst(&request, sample_request, kMinMessageSize);
                char response_message[kMinMessageSize];
                iov_t response;
                channel_->SetIov(&response, response_message, kMinMessageSize);

                const auto clock_type{Neutrino::ClockType::kRealtime};
                const auto timeout_flag{Neutrino::TimerTimeoutFlag::kReply | Neutrino::TimerTimeoutFlag::kSend};
                const auto timeout = std::chrono::milliseconds{100};

                const auto send_message = SendMessage(
                    create_client.value(), request, response, std::make_tuple(clock_type, timeout_flag, timeout));
                EXPECT_TRUE(send_message.has_value());
                EXPECT_STREQ(response_message, sample_response);

                const auto destroy_client = DestroyClient(create_client.value());
                EXPECT_TRUE(destroy_client.has_value());
            });
    }
}

TEST_F(NeutrinoTest, TestSendErrorOnServerDeath)
{
    std::mutex client_server_sync_mutex;
    std::condition_variable client_server_sync;
    bool is_ready_to_connect = false;

    auto server_thread = score::cpp::jthread([this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex]() {
        const auto create_server = CreateNamedServer();
        EXPECT_TRUE(create_server.has_value());

        auto name_attach = std::get<1>(create_server.value());
        auto dispatch = std::get<2>(create_server.value());

        is_ready_to_connect = true;
        client_server_sync.notify_all();

        std::this_thread::sleep_for(std::chrono::milliseconds{50});

        const auto destroy_named_server = DestroyNamedServer(name_attach, dispatch);
        EXPECT_TRUE(destroy_named_server.has_value());
    });

    auto client_thread = score::cpp::jthread([this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex]() {
        {
            std::unique_lock<std::mutex> lock(client_server_sync_mutex);
            client_server_sync.wait(lock, [&is_ready_to_connect]() {
                return is_ready_to_connect;
            });
        }

        const auto create_client = CreateNamedServerClient();
        EXPECT_TRUE(create_client.has_value());

        iov_t request;
        channel_->SetIovConst(&request, sample_request, kMinMessageSize);
        char response_message[kMinMessageSize];
        iov_t response;
        channel_->SetIov(&response, response_message, kMinMessageSize);

        const auto clock_type{Neutrino::ClockType::kRealtime};
        const auto timeout_flag{Neutrino::TimerTimeoutFlag::kReply | Neutrino::TimerTimeoutFlag::kSend};
        const auto timeout = std::chrono::seconds{1};

        const auto before_sending = std::chrono::high_resolution_clock::now();
        const auto send_message =
            SendMessage(create_client.value(), request, response, std::make_tuple(clock_type, timeout_flag, timeout));
        const auto after_receiving = std::chrono::high_resolution_clock::now();
        EXPECT_GT(timeout, after_receiving - before_sending);
        EXPECT_FALSE(send_message.has_value());
        EXPECT_EQ(send_message.error(), score::os::Error::Code::kBadFileDescriptor);

        const auto destroy_client = DestroyClient(create_client.value());
        EXPECT_TRUE(destroy_client.has_value());
    });
}

TEST_F(NeutrinoTest, TestClientPulseOnServerDeath)
{
    std::mutex client_server_sync_mutex;
    std::condition_variable client_server_sync;
    bool is_ready_to_connect = false;

    auto server_thread = score::cpp::jthread([this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex]() {
        const auto create_server = CreateNamedServer(Neutrino::ChannelFlag::kPrivate);
        EXPECT_TRUE(create_server.has_value());

        auto name_attach = std::get<1>(create_server.value());
        auto dispatch = std::get<2>(create_server.value());

        is_ready_to_connect = true;
        client_server_sync.notify_all();

        std::this_thread::sleep_for(std::chrono::milliseconds{200});
        const auto destroy_named_server = DestroyNamedServer(name_attach, dispatch);
        EXPECT_TRUE(destroy_named_server.has_value());
    });

    auto client_thread = score::cpp::jthread([this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex](
                                          const score::cpp::stop_token& stop_token) {
        {
            std::unique_lock<std::mutex> lock(client_server_sync_mutex);
            client_server_sync.wait(lock, [&is_ready_to_connect]() {
                return is_ready_to_connect;
            });
        }

        const auto create_client = CreateNamedServerClient();
        EXPECT_TRUE(create_client.has_value());

        while (!stop_token.stop_requested())
        {
            // just doing our job as a good client here
        }
    });

    auto observer_thread =
        score::cpp::jthread([this, &client_thread, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex]() {
            const auto observe_server =
                CreateNamedServer(Neutrino::ChannelFlag::kConnectionIdDisconnect, "observe_server");

            const auto channel_id = std::get<0>(observe_server.value());
            auto name_attach = std::get<1>(observe_server.value());
            auto dispatch = std::get<2>(observe_server.value());

            char request_message[kMinMessageSize];
            iov_t request;
            channel_->SetIov(&request, request_message, kMinMessageSize);

            const auto clock_type{Neutrino::ClockType::kRealtime};
            const auto timeout_flag{Neutrino::TimerTimeoutFlag::kReceive};
            const auto timeout = std::chrono::milliseconds{20};

            while (true)
            {
                // TODO: Use a pulse here
                const auto message_receive =
                    ReceiveMessage(channel_id, request, std::make_tuple(clock_type, timeout_flag, timeout));
                if (!message_receive.has_value())
                {
                    EXPECT_EQ(message_receive.error(), score::os::Error::Code::kKernelTimeout);
                    continue;
                }
                else
                {
                    // TODO: Use a pulse here, remove this
                    if (message_receive.value() != 0)
                    {
                        EXPECT_STREQ(request_message, sample_request);
                        // ignore anything which is not a pulse
                        continue;
                    }
                    else
                    {
                        const auto pulse = *reinterpret_cast<_pulse*>(request_message);
                        EXPECT_EQ(pulse.code, _PULSE_CODE_COIDDEATH);
                        const auto destroy_client = DestroyClient(pulse.value.sival_int);
                        EXPECT_TRUE(destroy_client.has_value());
                        break;
                    }
                }
            }

            client_thread.request_stop();
            const auto destroy_named_server = DestroyNamedServer(name_attach, dispatch);
            EXPECT_TRUE(destroy_named_server.has_value());
        });
}
