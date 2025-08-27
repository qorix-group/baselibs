///
/// @file daemon_communicator_test.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API daemon communicator test source file
///

#include "score/analysis/tracing/library/generic_trace_api/daemon_communicator/impl/daemon_communicator.h"
#include "score/analysis/tracing/common/testing_utils/notification/notification_helper.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"
#include "score/analysis/tracing/library/generic_trace_api/object_factory.h"
#include "score/analysis/tracing/plugin/ipc_trace_plugin/interface/daemon_communication_response.h"
#include "score/memory/shared/shared_memory_factory.h"
#include "score/memory/shared/shared_memory_factory_mock.h"
#include "score/memory/shared/shared_memory_resource_mock.h"
#include "score/os/mocklib/qnx/mock_channel.h"
#include "score/os/mocklib/qnx/mock_dispatch.h"
#include "score/os/mocklib/qnx/mock_mman.h"
#include "score/os/mocklib/qnx/neutrino_qnx_mock.h"
#include "score/os/mocklib/unistdmock.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <future>
using ::testing::_;
using ::testing::ByRef;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::InSequence;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;
using ::testing::StrEq;
using ::testing::StrictMock;

using namespace score::analysis::tracing;

ACTION_P(AssignResponse, param)
{
    *static_cast<Response*>(arg1) = param;
}

class DaemonCommunicatorFixture : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        dispatch_mock_ = std::make_unique<StrictMock<score::os::MockDispatch>>();
        channel_mock_ = std::make_unique<StrictMock<score::os::MockChannel>>();
        mman_mock_ = std::make_unique<StrictMock<score::os::qnx::MmanQnxMock>>();
        neutrino_mock_ = std::make_unique<StrictMock<score::os::qnx::NeutrinoMock>>();
        mock_unistd_ = std::make_unique<StrictMock<score::os::UnistdMock>>();

        dispatch_mock_raw_ptr_ = dispatch_mock_.get();
        channel_mock_raw_ptr_ = channel_mock_.get();
        mman_mock_raw_ptr_ = mman_mock_.get();
        neutrino_mock_raw_ptr_ = neutrino_mock_.get();
        unistd_mock_raw_ptr_ = mock_unistd_.get();

        received_shutdown_request_ = false;
    };

    void TearDown() override
    {
        daemon_communicator_.reset();
        ASSERT_TRUE(Mock::VerifyAndClearExpectations(dispatch_mock_raw_ptr_));
        ASSERT_TRUE(Mock::VerifyAndClearExpectations(channel_mock_raw_ptr_));
        ASSERT_TRUE(Mock::VerifyAndClearExpectations(mman_mock_raw_ptr_));
        ASSERT_TRUE(Mock::VerifyAndClearExpectations(neutrino_mock_raw_ptr_));
    };

    void CorrectInitialization()
    {
        HandleDaemonCrashDetectorThread();
        daemon_communicator_ = std::make_unique<DaemonCommunicator>(std::move(dispatch_mock_),
                                                                    std::move(channel_mock_),
                                                                    std::move(mman_mock_),
                                                                    std::move(neutrino_mock_),
                                                                    std::move(mock_unistd_),
                                                                    stop_source_.get_token());
        // wait for a little to give time for the daemon crash detector thread to be alive
        ASSERT_TRUE(deamon_thread_notification_.WaitForNotificationWithTimeout(std::chrono::milliseconds{100}));
    }

    void HandleDaemonCrashDetectorThread()
    {
        EXPECT_CALL(*neutrino_mock_raw_ptr_,
                    ChannelCreate(score::os::qnx::Neutrino::ChannelFlag::kConnectionIdDisconnect))
            .WillOnce(Return(channel_id_));
        EXPECT_CALL(*mock_unistd_, getpid()).WillRepeatedly(Return(valid_process_id_));
        EXPECT_CALL(*channel_mock_raw_ptr_, ConnectAttach).WillOnce(Invoke([this]() {
            deamon_thread_notification_.Notify();
            return connection_id_;
        }));
        EXPECT_CALL(*channel_mock_raw_ptr_, MsgReceivePulse)
            .WillRepeatedly(Invoke([this](const std::int32_t, void* const pulse, const std::size_t, _msg_info* const) {
                {
                    std::unique_lock<std::mutex> lock{mutex_};
                    std::cout << "Waiting for shutdown request or daemon crash notification ..." << std::endl;
                    condition_variable_.wait(lock, [this, &pulse]() {
                        std::cout << "Received shutdown request ..." << std::endl;
                        auto input_pulse = reinterpret_cast<struct _pulse*>(pulse);
                        input_pulse->code = kPulseCodeStopThread;
                        return received_shutdown_request_;
                    });
                }
                return 1;
            }));
        EXPECT_CALL(*channel_mock_raw_ptr_, MsgSendPulse)
            .WillRepeatedly(Invoke([this](const int32_t, const int32_t, const int32_t, const int32_t) {
                ShutdownDaemonCrashDetectorThread();
                return score::cpp::blank{};
            }));
        EXPECT_CALL(*channel_mock_raw_ptr_, ConnectDetach);

        EXPECT_CALL(*neutrino_mock_raw_ptr_, ChannelDestroy(_)).WillOnce(Invoke([this](int32_t) {
            deamon_thread_notification_.Notify();
            return EOK;
        }));
    }
    void ShutdownDaemonCrashDetectorThread()
    {
        if (!received_shutdown_request_)
        {
            // wait for a little to give time for the daemon crash detector thread to be alive
            std::cout << "Requesting stop..." << std::endl;
            stop_source_.request_stop();
            std::cout << "Waiting for the crash detector thread to shutdown..." << std::endl;
            std::unique_lock<std::mutex> lock{mutex_};
            received_shutdown_request_ = true;
            condition_variable_.notify_one();
            std::cout << "the crash detector thread finished !" << std::endl;
        }
    }

    void ExpectConnection()
    {
        Response response{};
        response.SetDaemonProcessId({valid_process_id_});

        InSequence sequence;
        EXPECT_CALL(*dispatch_mock_raw_ptr_, name_open(StrEq(GetServerAddressName()), Eq(name_open_flags)))
            .WillOnce(Return(channel_id_));
        ExpectIovSetting(&response);
        EXPECT_CALL(*neutrino_mock_raw_ptr_,
                    TimerTimeout(kClockType, _, testing::Matcher<std::unique_ptr<score::os::SigEvent>>(_), _, _))
            .WillOnce(Return(timer_timeout_result_));
        EXPECT_CALL(*channel_mock_raw_ptr_,
                    MsgSendv(Eq(channel_id_), _, Eq(message_parts_count_), _, Eq(message_parts_count_)))
            .WillOnce(Return(send_result_));
    }

    void ExpectConnectionClose()
    {
        EXPECT_CALL(*dispatch_mock_raw_ptr_, name_close(Eq(channel_id_))).WillOnce(Return(score::cpp::blank{}));
    }

    void ExpectIovSetting(const Response* const response = nullptr)
    {
        InSequence sequence;
        EXPECT_CALL(*channel_mock_raw_ptr_, SetIovConst(_, _, Eq(sizeof(Request)))).Times(1);
        if (response != nullptr)
        {
            EXPECT_CALL(*channel_mock_raw_ptr_, SetIov(_, _, Eq(sizeof(Response))))
                .Times(1)
                .WillOnce(AssignResponse(*response));
        }
        else
        {
            EXPECT_CALL(*channel_mock_raw_ptr_, SetIov(_, _, Eq(sizeof(Response)))).Times(1);
        }
    }

    void ExpectQnxSharedMemory(bool isPath = false)
    {
        const std::int32_t shm_create_handle_success{0};
        InSequence sequence;
        if (isPath)
        {
            EXPECT_CALL(*mman_mock_raw_ptr_, shm_open(StrEq(file_path_.c_str()), _, _))
                .WillOnce(Return(valid_file_descriptor_));
        }

        EXPECT_CALL(*mman_mock_raw_ptr_,
                    shm_create_handle(Eq(valid_file_descriptor_), _, Eq(O_RDONLY), _, Eq(shm_create_handle_options_)))
            .WillOnce(Return(shm_create_handle_success));
    }

    void CheckConnection()
    {
        const auto connect_result = daemon_communicator_->Connect();
        EXPECT_TRUE(connect_result.has_value());
    }

    std::unique_ptr<IDaemonCommunicator> daemon_communicator_;
    std::unique_ptr<StrictMock<score::os::MockDispatch>> dispatch_mock_;
    std::unique_ptr<StrictMock<score::os::MockChannel>> channel_mock_;
    std::unique_ptr<StrictMock<score::os::qnx::MmanQnxMock>> mman_mock_;
    std::unique_ptr<StrictMock<score::os::qnx::NeutrinoMock>> neutrino_mock_;
    std::unique_ptr<StrictMock<score::os::UnistdMock>> mock_unistd_;

    // not to be deleted, no leaks
    StrictMock<score::os::MockDispatch>* dispatch_mock_raw_ptr_;
    StrictMock<score::os::MockChannel>* channel_mock_raw_ptr_;
    StrictMock<score::os::qnx::MmanQnxMock>* mman_mock_raw_ptr_;
    StrictMock<score::os::qnx::NeutrinoMock>* neutrino_mock_raw_ptr_;
    StrictMock<score::os::UnistdMock>* unistd_mock_raw_ptr_;

    const std::string file_path_{"/test_file_path"};
    const ShmObjectHandle invalid_index_{-1};
    const ShmObjectHandle valid_index_{0};
    static constexpr shm_handle_t valid_handle{0};
    static constexpr std::int32_t valid_file_descriptor_{0};
    static constexpr std::int64_t send_result_{0};
    static constexpr std::int32_t timer_timeout_result_{0};
    static constexpr std::int32_t channel_id_{0};
    static constexpr std::int32_t connection_id_{150};
    static constexpr pid_t valid_process_id_{1000};
    static constexpr std::uint32_t message_parts_count_{1};
    static constexpr std::uint32_t shm_create_handle_options_{0};
    static constexpr std::int32_t name_open_flags{0};
    static constexpr auto kClockType{score::os::qnx::Neutrino::ClockType::kRealtime};
    static constexpr std::int32_t kPulseCodeStopThread = 100;

    score::cpp::stop_source stop_source_{};
    std::mutex mutex_;
    std::condition_variable condition_variable_;
    bool received_shutdown_request_;
    NotificationHelper deamon_thread_notification_;
};

TEST_F(DaemonCommunicatorFixture, RegisterSharedMemoryObjectDaemonNotConnected)
{
    CorrectInitialization();

    const auto register_result = daemon_communicator_->RegisterSharedMemoryObject(valid_file_descriptor_);
    EXPECT_FALSE(register_result.has_value());
    EXPECT_EQ(register_result.error(), ErrorCode::kDaemonNotConnectedFatal);
}

TEST_F(DaemonCommunicatorFixture, UnregisterSharedMemoryObjectDaemonNotConnected)
{
    CorrectInitialization();

    const auto unregister_result = daemon_communicator_->UnregisterSharedMemoryObject(valid_index_);
    EXPECT_FALSE(unregister_result.has_value());
    EXPECT_EQ(unregister_result.error(), ErrorCode::kDaemonNotConnectedFatal);
}

TEST_F(DaemonCommunicatorFixture, ConnectNameOpenFailed)
{
    CorrectInitialization();

    EXPECT_CALL(*dispatch_mock_raw_ptr_, name_open(StrEq(GetServerAddressName()), _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));
    const auto connect_result = daemon_communicator_->Connect();
    EXPECT_FALSE(connect_result.has_value());
    EXPECT_EQ(connect_result.error(), ErrorCode::kServerConnectionNameOpenFailedFatal);
}

TEST_F(DaemonCommunicatorFixture, ConnectFailedForInvalidDaemonConnectionID)
{
    CorrectInitialization();

    ShutdownDaemonCrashDetectorThread();
    // waiting deamon thread to finish and call CleanupCrashDetectorThread
    ASSERT_TRUE(deamon_thread_notification_.WaitForNotificationWithTimeout(std::chrono::milliseconds{30}));
    const auto connect_result = daemon_communicator_->Connect();
    EXPECT_FALSE(connect_result.has_value());
    EXPECT_EQ(connect_result.error(), ErrorCode::kDaemonTerminationDetectionFailedFatal);
}

TEST_F(DaemonCommunicatorFixture, ConnectDaemonProcessIdRequestFailed)
{
    CorrectInitialization();

    InSequence sequence;
    EXPECT_CALL(*dispatch_mock_raw_ptr_, name_open(StrEq(GetServerAddressName()), Eq(name_open_flags)))
        .WillOnce(Return(channel_id_));
    ExpectIovSetting();
    EXPECT_CALL(*neutrino_mock_raw_ptr_,
                TimerTimeout(kClockType, _, testing::Matcher<std::unique_ptr<score::os::SigEvent>>(_), _, _))
        .WillOnce(Return(timer_timeout_result_));
    EXPECT_CALL(*channel_mock_raw_ptr_, MsgSendv(_, _, Eq(message_parts_count_), _, Eq(message_parts_count_)))
        .WillOnce(Return(score::cpp::unexpected<score::os::Error>(score::os::Error::createUnspecifiedError())));
    ExpectConnectionClose();

    const auto connect_result = daemon_communicator_->Connect();
    EXPECT_FALSE(connect_result.has_value());
    EXPECT_EQ(connect_result.error(), ErrorCode::kMessageSendFailedRecoverable);
}

TEST_F(DaemonCommunicatorFixture, ConnectionSuccess)
{
    CorrectInitialization();
    EXPECT_CALL(*channel_mock_raw_ptr_, MsgReceivePulse)
        .WillRepeatedly(Invoke([](int32_t, void* const pulse, size_t, _msg_info*) {
            auto* input_pulse = reinterpret_cast<struct _pulse*>(pulse);
            input_pulse->code = kPulseCodeStopThread;
            return 1;
        }));
    InSequence sequence;
    ExpectConnection();
    ExpectConnectionClose();

    CheckConnection();
}

TEST_F(DaemonCommunicatorFixture, ConnectionCloseFailed)
{
    CorrectInitialization();

    InSequence sequence;
    ExpectConnection();
    EXPECT_CALL(*dispatch_mock_raw_ptr_, name_close(Eq(channel_id_)))
        .WillOnce(Return(score::cpp::unexpected<score::os::Error>(score::os::Error::createUnspecifiedError())));

    CheckConnection();
}

TEST_F(DaemonCommunicatorFixture, RegisterSharedMemoryObjectInvalidChannel)
{
    CorrectInitialization();

    const auto register_result = daemon_communicator_->RegisterSharedMemoryObject(file_path_);
    EXPECT_FALSE(register_result.has_value());
    EXPECT_EQ(register_result.error(), ErrorCode::kDaemonNotConnectedFatal);
}

TEST_F(DaemonCommunicatorFixture, RegisterSharedMemoryObjectEmptyPath)
{
    CorrectInitialization();

    InSequence sequence;
    ExpectConnection();
    ExpectConnectionClose();

    CheckConnection();
    const auto register_result = daemon_communicator_->RegisterSharedMemoryObject(std::string{});
    EXPECT_FALSE(register_result.has_value());
    EXPECT_EQ(register_result.error(), ErrorCode::kInvalidArgumentFatal);
}

TEST_F(DaemonCommunicatorFixture, RegisterSharedMemoryObjectSharedMemoryOpenFailed)
{
    CorrectInitialization();
    InSequence sequence;
    ExpectConnection();
    EXPECT_CALL(*mman_mock_raw_ptr_, shm_open(StrEq(file_path_.c_str()), _, _))
        .WillOnce(Return(score::cpp::unexpected<score::os::Error>(score::os::Error::createUnspecifiedError())));
    ExpectConnectionClose();

    CheckConnection();
    const auto register_result = daemon_communicator_->RegisterSharedMemoryObject(file_path_.c_str());
    EXPECT_FALSE(register_result.has_value());
    EXPECT_EQ(register_result.error(), ErrorCode::kBadFileDescriptorFatal);
}

TEST_F(DaemonCommunicatorFixture, RegisterSharedMemoryObjectSharedMemoryCreateHandleFailed)
{
    CorrectInitialization();

    // Prepare shared memory resource mock
    auto shared_memory_resource_mock = std::make_shared<score::memory::shared::SharedMemoryResourceMock>();
    EXPECT_CALL(*shared_memory_resource_mock, IsShmInTypedMemory).WillRepeatedly(Return(true));

    // Prepare shared memory factory mock
    auto shared_memory_factory_mock = std::make_shared<score::memory::shared::SharedMemoryFactoryMock>();
    EXPECT_CALL(*shared_memory_factory_mock, Create).WillRepeatedly(Return(shared_memory_resource_mock));

    // inject shared memory factory mock
    score::memory::shared::SharedMemoryFactory::InjectMock(shared_memory_factory_mock.get());

    InSequence sequence;
    ExpectConnection();
    EXPECT_CALL(*mman_mock_raw_ptr_, shm_open(StrEq(file_path_.c_str()), _, _))
        .WillOnce(Return(valid_file_descriptor_));
    EXPECT_CALL(*mman_mock_raw_ptr_,
                shm_create_handle(Eq(valid_file_descriptor_), _, Eq(O_RDONLY), _, Eq(shm_create_handle_options_)))
        .WillOnce(Return(score::cpp::unexpected<score::os::Error>(score::os::Error::createUnspecifiedError())));
    ExpectConnectionClose();

    CheckConnection();
    const auto register_result = daemon_communicator_->RegisterSharedMemoryObject(file_path_.c_str());
    EXPECT_FALSE(register_result.has_value());
    EXPECT_EQ(register_result.error(), ErrorCode::kSharedMemoryObjectRegistrationFailedFatal);
}

TEST_F(DaemonCommunicatorFixture, RegisterSharedMemoryObjectMessageSetSendTimeoutFailed)
{
    CorrectInitialization();

    // Prepare shared memory resource mock
    auto shared_memory_resource_mock = std::make_shared<score::memory::shared::SharedMemoryResourceMock>();
    EXPECT_CALL(*shared_memory_resource_mock, IsShmInTypedMemory).WillRepeatedly(Return(true));

    // Prepare shared memory factory mock
    auto shared_memory_factory_mock = std::make_shared<score::memory::shared::SharedMemoryFactoryMock>();
    EXPECT_CALL(*shared_memory_factory_mock, Create).WillRepeatedly(Return(shared_memory_resource_mock));

    // inject shared memory factory mock
    score::memory::shared::SharedMemoryFactory::InjectMock(shared_memory_factory_mock.get());

    InSequence sequence;
    ExpectConnection();
    ExpectQnxSharedMemory();
    ExpectIovSetting();

    EXPECT_CALL(*neutrino_mock_raw_ptr_,
                TimerTimeout(kClockType, _, testing::Matcher<std::unique_ptr<score::os::SigEvent>>(_), _, _))
        .WillOnce(Return(score::cpp::unexpected<score::os::Error>(score::os::Error::createUnspecifiedError())));
    ExpectConnectionClose();

    CheckConnection();
    const auto register_result = daemon_communicator_->RegisterSharedMemoryObject(valid_file_descriptor_);
    EXPECT_FALSE(register_result.has_value());
    EXPECT_EQ(register_result.error(), ErrorCode::kMessageSendFailedRecoverable);
}

TEST_F(DaemonCommunicatorFixture, RegisterSharedMemoryObjectMessageSendFailed)
{
    CorrectInitialization();
    EXPECT_CALL(*channel_mock_raw_ptr_, MsgReceivePulse)
        .WillRepeatedly(Invoke([](int32_t, void* const pulse, size_t, _msg_info*) {
            auto* input_pulse = reinterpret_cast<struct _pulse*>(pulse);
            input_pulse->code = kPulseCodeStopThread;
            return 1;
        }));
    InSequence sequence;
    ExpectConnection();
    ExpectQnxSharedMemory();
    ExpectIovSetting();

    EXPECT_CALL(*neutrino_mock_raw_ptr_,
                TimerTimeout(kClockType, _, testing::Matcher<std::unique_ptr<score::os::SigEvent>>(_), _, _))
        .WillOnce(Return(timer_timeout_result_));
    EXPECT_CALL(*channel_mock_raw_ptr_, MsgSendv(_, _, Eq(message_parts_count_), _, Eq(message_parts_count_)))
        .WillOnce(Return(score::cpp::unexpected<score::os::Error>(score::os::Error::createUnspecifiedError())));
    ExpectConnectionClose();

    CheckConnection();
    const auto register_result = daemon_communicator_->RegisterSharedMemoryObject(valid_file_descriptor_);
    EXPECT_FALSE(register_result.has_value());
    EXPECT_EQ(register_result.error(), ErrorCode::kMessageSendFailedRecoverable);
}

TEST_F(DaemonCommunicatorFixture, RegisterSharedMemoryObjectInvalidSharedMemoryObjectIndex)
{
    CorrectInitialization();

    Response response{};
    response.SetRegisterSharedMemoryObject({invalid_index_});

    InSequence sequence;
    ExpectConnection();
    ExpectQnxSharedMemory();
    ExpectIovSetting(&response);
    EXPECT_CALL(*neutrino_mock_raw_ptr_,
                TimerTimeout(kClockType, _, testing::Matcher<std::unique_ptr<score::os::SigEvent>>(_), _, _))
        .WillOnce(Return(timer_timeout_result_));
    EXPECT_CALL(*channel_mock_raw_ptr_, MsgSendv(_, _, Eq(message_parts_count_), _, Eq(message_parts_count_)))
        .WillOnce(Return(send_result_));
    ExpectConnectionClose();

    CheckConnection();
    const auto register_result = daemon_communicator_->RegisterSharedMemoryObject(valid_file_descriptor_);
    EXPECT_FALSE(register_result.has_value());
    EXPECT_EQ(register_result.error(), ErrorCode::kSharedMemoryObjectHandleCreationFailedFatal);
}

TEST_F(DaemonCommunicatorFixture, RegisterSharedMemoryObjectSuccess)
{
    CorrectInitialization();

    Response response{};
    response.SetRegisterSharedMemoryObject({valid_index_});

    InSequence sequence;
    ExpectConnection();
    ExpectQnxSharedMemory();
    ExpectIovSetting(&response);
    EXPECT_CALL(*neutrino_mock_raw_ptr_,
                TimerTimeout(kClockType, _, testing::Matcher<std::unique_ptr<score::os::SigEvent>>(_), _, _))
        .WillOnce(Return(timer_timeout_result_));
    EXPECT_CALL(*channel_mock_raw_ptr_, MsgSendv(_, _, Eq(message_parts_count_), _, Eq(message_parts_count_)))
        .WillOnce(Return(send_result_));
    ExpectConnectionClose();

    CheckConnection();
    const auto register_result = daemon_communicator_->RegisterSharedMemoryObject(valid_file_descriptor_);
    EXPECT_EQ(register_result.value(), valid_index_);
}

TEST_F(DaemonCommunicatorFixture, UnregisterSharedMemoryObjectFailed)
{
    RecordProperty("Verifies", "SCR-39689123");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "Test verifies if Trace API returns recoverable error when SHM unregistration fails");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    CorrectInitialization();

    EXPECT_CALL(*channel_mock_raw_ptr_, MsgReceivePulse)
        .WillRepeatedly(Invoke([](int32_t, void* const pulse, size_t, _msg_info*) {
            auto* input_pulse = reinterpret_cast<struct _pulse*>(pulse);
            input_pulse->code = kPulseCodeStopThread;
            return 1;
        }));

    Response response{};
    response.SetUnregisterSharedMemoryObject({false});

    InSequence sequence;
    ExpectConnection();
    ExpectIovSetting(&response);
    EXPECT_CALL(*neutrino_mock_raw_ptr_,
                TimerTimeout(kClockType, _, testing::Matcher<std::unique_ptr<score::os::SigEvent>>(_), _, _))
        .WillOnce(Return(timer_timeout_result_));
    EXPECT_CALL(*channel_mock_raw_ptr_, MsgSendv(_, _, Eq(message_parts_count_), _, Eq(message_parts_count_)))
        .WillOnce(Return(send_result_));
    ExpectConnectionClose();

    CheckConnection();
    const auto register_result = daemon_communicator_->UnregisterSharedMemoryObject(valid_index_);
    EXPECT_FALSE(register_result.has_value());
    EXPECT_EQ(register_result.error(), ErrorCode::kSharedMemoryObjectUnregisterFailedFatal);
}

TEST_F(DaemonCommunicatorFixture, UnregisterSharedMemoryObjectSendMessageFails)
{
    CorrectInitialization();

    Response response{};
    response.SetUnregisterSharedMemoryObject({true});

    InSequence sequence;
    ExpectConnection();
    ExpectIovSetting(&response);
    EXPECT_CALL(*neutrino_mock_raw_ptr_,
                TimerTimeout(kClockType, _, testing::Matcher<std::unique_ptr<score::os::SigEvent>>(_), _, _))
        .WillOnce(Return(timer_timeout_result_));
    EXPECT_CALL(*channel_mock_raw_ptr_, MsgSendv(_, _, Eq(message_parts_count_), _, Eq(message_parts_count_)))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));

    ExpectConnectionClose();

    CheckConnection();
    const auto register_result = daemon_communicator_->UnregisterSharedMemoryObject(valid_index_);
    EXPECT_FALSE(register_result.has_value());
}

TEST_F(DaemonCommunicatorFixture, UnregisterSharedMemoryObjectSuccess)
{
    RecordProperty("Verifies", "SCR-39765455");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if Library closes connection to backend after unregistering SHM Object"
                   "what happens also the when process linking against the library is being terminated");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    CorrectInitialization();

    Response response{};
    response.SetUnregisterSharedMemoryObject({true});

    InSequence sequence;
    ExpectConnection();
    ExpectIovSetting(&response);
    EXPECT_CALL(*neutrino_mock_raw_ptr_,
                TimerTimeout(kClockType, _, testing::Matcher<std::unique_ptr<score::os::SigEvent>>(_), _, _))
        .WillOnce(Return(timer_timeout_result_));
    EXPECT_CALL(*channel_mock_raw_ptr_, MsgSendv(_, _, Eq(message_parts_count_), _, Eq(message_parts_count_)))
        .WillOnce(Return(send_result_));
    ExpectConnectionClose();

    CheckConnection();
    const auto register_result = daemon_communicator_->UnregisterSharedMemoryObject(valid_index_);
    EXPECT_TRUE(register_result.has_value());
}

TEST_F(DaemonCommunicatorFixture, RegisterClientInvalidBindingType)
{
    CorrectInitialization();

    const auto register_result = daemon_communicator_->RegisterClient(BindingType::kUndefined, AppIdType{"temp"});
    EXPECT_FALSE(register_result.has_value());
    EXPECT_EQ(register_result.error(), ErrorCode::kInvalidBindingTypeFatal);
}

TEST_F(DaemonCommunicatorFixture, RegisterClientSuccess)
{
    CorrectInitialization();

    InSequence sequence;
    EXPECT_CALL(*channel_mock_raw_ptr_, SetIovConst).Times(1);
    EXPECT_CALL(*channel_mock_raw_ptr_, SetIov).Times(1);

    EXPECT_CALL(*neutrino_mock_raw_ptr_,
                TimerTimeout(kClockType, _, testing::Matcher<std::unique_ptr<score::os::SigEvent>>(_), _, _))
        .WillOnce(Return(timer_timeout_result_));
    EXPECT_CALL(*channel_mock_raw_ptr_, MsgSendv).WillOnce(Return(send_result_));

    const auto register_result = daemon_communicator_->RegisterClient(BindingType::kFirst, AppIdType{"temp"});
    EXPECT_TRUE(register_result.has_value());
}

TEST_F(DaemonCommunicatorFixture, RegisterClientErrorCode)
{
    CorrectInitialization();

    Response response{};
    response.SetErrorCode({static_cast<score::result::ErrorCode>(ErrorCode::kWrongClientIdRecoverable)});

    InSequence sequence;
    ExpectConnection();
    ExpectIovSetting(&response);
    EXPECT_CALL(*neutrino_mock_raw_ptr_,
                TimerTimeout(kClockType, _, testing::Matcher<std::unique_ptr<score::os::SigEvent>>(_), _, _))
        .WillOnce(Return(timer_timeout_result_));
    EXPECT_CALL(*channel_mock_raw_ptr_, MsgSendv(_, _, Eq(message_parts_count_), _, Eq(message_parts_count_)))
        .WillOnce(Return(send_result_));
    ExpectConnectionClose();

    CheckConnection();
    const auto register_result = daemon_communicator_->RegisterClient(BindingType::kFirst, AppIdType{"temp"});
    EXPECT_FALSE(register_result.has_value());
    EXPECT_EQ(register_result.error(), ErrorCode::kWrongClientIdRecoverable);
}

TEST_F(DaemonCommunicatorFixture, RegisterClientInvalidReturn)
{
    CorrectInitialization();

    Response response{};
    response.SetDaemonProcessId({valid_process_id_});

    InSequence sequence;
    ExpectConnection();
    ExpectIovSetting(&response);
    EXPECT_CALL(*neutrino_mock_raw_ptr_,
                TimerTimeout(kClockType, _, testing::Matcher<std::unique_ptr<score::os::SigEvent>>(_), _, _))
        .WillOnce(Return(timer_timeout_result_));
    EXPECT_CALL(*channel_mock_raw_ptr_, MsgSendv(_, _, Eq(message_parts_count_), _, Eq(message_parts_count_)))
        .WillOnce(Return(send_result_));
    ExpectConnectionClose();

    CheckConnection();
    const auto register_result = daemon_communicator_->RegisterClient(BindingType::kFirst, AppIdType{"temp"});
    EXPECT_FALSE(register_result.has_value());
    EXPECT_EQ(register_result.error(), ErrorCode::kGenericErrorRecoverable);
}

TEST_F(DaemonCommunicatorFixture, RegisterClientFailedSendMessage)
{
    CorrectInitialization();

    InSequence sequence;
    EXPECT_CALL(*channel_mock_raw_ptr_, SetIovConst).Times(1);
    EXPECT_CALL(*channel_mock_raw_ptr_, SetIov).Times(1);

    EXPECT_CALL(*neutrino_mock_raw_ptr_,
                TimerTimeout(kClockType, _, testing::Matcher<std::unique_ptr<score::os::SigEvent>>(_), _, _))
        .WillOnce(Return(timer_timeout_result_));

    EXPECT_CALL(*channel_mock_raw_ptr_, MsgSendv)
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));

    const auto register_result = daemon_communicator_->RegisterClient(BindingType::kFirst, AppIdType{"temp"});
    EXPECT_FALSE(register_result.has_value());
    EXPECT_EQ(register_result.error(), ErrorCode::kMessageSendFailedRecoverable);
}

TEST_F(DaemonCommunicatorFixture, CreateChannelToDetectDaemonTerminationFailed)
{
    EXPECT_CALL(*neutrino_mock_raw_ptr_, ChannelCreate(score::os::qnx::Neutrino::ChannelFlag::kConnectionIdDisconnect))
        .WillOnce(Invoke([this]() {
            deamon_thread_notification_.Notify();
            return score::cpp::make_unexpected(score::os::Error::createFromErrno());
        }));
    daemon_communicator_ = std::make_unique<DaemonCommunicator>(std::move(dispatch_mock_),
                                                                std::move(channel_mock_),
                                                                std::move(mman_mock_),
                                                                std::move(neutrino_mock_),
                                                                std::move(mock_unistd_),
                                                                stop_source_.get_token());
    // wait for a little to give time for the daemon crash detector thread to be alive
    ASSERT_TRUE(deamon_thread_notification_.WaitForNotificationWithTimeout(std::chrono::milliseconds{100}));

    const auto connect_result = daemon_communicator_->Connect();
    EXPECT_FALSE(connect_result.has_value());
}

TEST_F(DaemonCommunicatorFixture, ConnectAttachToDetectDaemonTerminationFailed)
{
    EXPECT_CALL(*neutrino_mock_raw_ptr_, ChannelCreate(score::os::qnx::Neutrino::ChannelFlag::kConnectionIdDisconnect))
        .WillOnce(Return(channel_id_));
    EXPECT_CALL(*mock_unistd_, getpid()).WillRepeatedly(Return(valid_process_id_));
    EXPECT_CALL(*channel_mock_raw_ptr_, ConnectAttach).WillOnce(Invoke([this]() {
        deamon_thread_notification_.Notify();
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }));
    daemon_communicator_ = std::make_unique<DaemonCommunicator>(std::move(dispatch_mock_),
                                                                std::move(channel_mock_),
                                                                std::move(mman_mock_),
                                                                std::move(neutrino_mock_),
                                                                std::move(mock_unistd_),
                                                                stop_source_.get_token());
    // wait for a little to give time for the daemon crash detector thread to be alive
    ASSERT_TRUE(deamon_thread_notification_.WaitForNotificationWithTimeout(std::chrono::milliseconds{100}));

    const auto connect_result = daemon_communicator_->Connect();
    EXPECT_FALSE(connect_result.has_value());
}

TEST_F(DaemonCommunicatorFixture, ReceivePulseFailedAndStopTokenRequested)
{
    std::uint8_t receive_pulse_count = 0;
    EXPECT_CALL(*neutrino_mock_raw_ptr_, ChannelCreate(score::os::qnx::Neutrino::ChannelFlag::kConnectionIdDisconnect))
        .WillOnce(Return(channel_id_));
    EXPECT_CALL(*mock_unistd_, getpid()).WillRepeatedly(Return(valid_process_id_));
    EXPECT_CALL(*channel_mock_raw_ptr_, ConnectAttach).WillOnce(Return(connection_id_));

    EXPECT_CALL(*channel_mock_raw_ptr_, MsgReceivePulse)
        .WillRepeatedly(
            Invoke([this, &receive_pulse_count](const std::int32_t, void* const, const std::size_t, _msg_info* const) {
                receive_pulse_count++;
                // request stop after some time
                if (receive_pulse_count > 10)
                {
                    stop_source_.request_stop();
                }
                return score::cpp::make_unexpected(score::os::Error::createFromErrno());
                ;
            }));

    EXPECT_CALL(*channel_mock_raw_ptr_, ConnectDetach);

    EXPECT_CALL(*neutrino_mock_raw_ptr_, ChannelDestroy(_)).WillOnce(Invoke([this](int32_t) {
        deamon_thread_notification_.Notify();
        return EOK;
    }));

    daemon_communicator_ = std::make_unique<DaemonCommunicator>(std::move(dispatch_mock_),
                                                                std::move(channel_mock_),
                                                                std::move(mman_mock_),
                                                                std::move(neutrino_mock_),
                                                                std::move(mock_unistd_),
                                                                stop_source_.get_token());
    // wait for a little to give time for the daemon crash detector thread to be alive
    ASSERT_TRUE(deamon_thread_notification_.WaitForNotificationWithTimeout(std::chrono::milliseconds{300}));

    const auto connect_result = daemon_communicator_->Connect();
    EXPECT_FALSE(connect_result.has_value());
}

TEST_F(DaemonCommunicatorFixture, ReceiveUnexpectedPulseAndStopTokenRequested)
{
    std::uint8_t receive_pulse_count = 0;
    EXPECT_CALL(*neutrino_mock_raw_ptr_, ChannelCreate(score::os::qnx::Neutrino::ChannelFlag::kConnectionIdDisconnect))
        .WillOnce(Return(channel_id_));
    EXPECT_CALL(*mock_unistd_, getpid()).WillRepeatedly(Return(valid_process_id_));
    EXPECT_CALL(*channel_mock_raw_ptr_, ConnectAttach).WillOnce(Return(connection_id_));

    EXPECT_CALL(*channel_mock_raw_ptr_, MsgReceivePulse)
        .WillRepeatedly(Invoke(
            [this, &receive_pulse_count](const std::int32_t, void* const pulse, const std::size_t, _msg_info* const) {
                constexpr std::int8_t unexpected_pulse_code = 77;
                receive_pulse_count++;
                // request stop after some time
                if (receive_pulse_count > 10)
                {
                    stop_source_.request_stop();
                }
                auto input_pulse = reinterpret_cast<struct _pulse*>(pulse);
                input_pulse->code = unexpected_pulse_code;
                return 0;
            }));
    EXPECT_CALL(*channel_mock_raw_ptr_, ConnectDetach);

    EXPECT_CALL(*neutrino_mock_raw_ptr_, ChannelDestroy(_)).WillOnce(Invoke([this](int32_t) {
        deamon_thread_notification_.Notify();
        return EOK;
    }));
    daemon_communicator_ = std::make_unique<DaemonCommunicator>(std::move(dispatch_mock_),
                                                                std::move(channel_mock_),
                                                                std::move(mman_mock_),
                                                                std::move(neutrino_mock_),
                                                                std::move(mock_unistd_),
                                                                stop_source_.get_token());
    // wait for a little to give time for the daemon crash detector thread to be alive
    ASSERT_TRUE(deamon_thread_notification_.WaitForNotificationWithTimeout(std::chrono::milliseconds{300}));

    const auto connect_result = daemon_communicator_->Connect();
    EXPECT_FALSE(connect_result.has_value());
}

TEST_F(DaemonCommunicatorFixture, ReceiveDaemonCrashPulse)
{
    std::uint8_t receive_pulse_count = 0;
    bool daemon_is_terminated = false;
    EXPECT_CALL(*neutrino_mock_raw_ptr_, ChannelCreate(score::os::qnx::Neutrino::ChannelFlag::kConnectionIdDisconnect))
        .WillOnce(Return(channel_id_));
    EXPECT_CALL(*mock_unistd_, getpid()).WillRepeatedly(Return(valid_process_id_));
    EXPECT_CALL(*channel_mock_raw_ptr_, ConnectAttach).WillOnce(Invoke([this]() {
        deamon_thread_notification_.Notify();
        return connection_id_;
    }));

    EXPECT_CALL(*channel_mock_raw_ptr_, MsgReceivePulse)
        .WillRepeatedly(Invoke([this, &receive_pulse_count, &daemon_is_terminated](
                                   const std::int32_t, void* const pulse, const std::size_t, _msg_info* const) {
            std::unique_lock<std::mutex> lock{mutex_};
            std::cout << "Waiting for shutdown request or daemon crash notification ..." << std::endl;
            condition_variable_.wait(lock, [this]() {
                return received_shutdown_request_;
            });
            auto input_pulse = reinterpret_cast<struct _pulse*>(pulse);
            if (daemon_is_terminated)
            {
                input_pulse->code = _PULSE_CODE_COIDDEATH;
                input_pulse->value.sival_int = channel_id_;
            }
            else
            {
                input_pulse->code = kPulseCodeStopThread;
            }
            return 1;
        }));
    EXPECT_CALL(*channel_mock_raw_ptr_, MsgSendPulse)
        .WillRepeatedly(
            Invoke([this, &daemon_is_terminated](const int32_t, const int32_t, const int32_t, const int32_t) {
                {
                    std::unique_lock<std::mutex> lock{mutex_};
                    received_shutdown_request_ = true;
                    daemon_is_terminated = false;
                    condition_variable_.notify_one();
                }
                return score::cpp::blank{};
            }));
    EXPECT_CALL(*channel_mock_raw_ptr_, ConnectDetach);

    EXPECT_CALL(*neutrino_mock_raw_ptr_, ChannelDestroy(_)).WillOnce(Invoke([this](int32_t) {
        deamon_thread_notification_.Notify();
        return EOK;
    }));
    ExpectConnection();

    daemon_communicator_ = std::make_unique<DaemonCommunicator>(std::move(dispatch_mock_),
                                                                std::move(channel_mock_),
                                                                std::move(mman_mock_),
                                                                std::move(neutrino_mock_),
                                                                std::move(mock_unistd_),
                                                                stop_source_.get_token());

    std::promise<void> callback_called;
    auto callback_future = callback_called.get_future();

    daemon_communicator_->SubscribeToDaemonTerminationNotification([&callback_called]() {
        callback_called.set_value();
    });

    // wait for a little to give time for the daemon crash detector thread to be alive
    ASSERT_TRUE(deamon_thread_notification_.WaitForNotificationWithTimeout(std::chrono::milliseconds{50}));
    // Should connect normally
    const auto connect_result = daemon_communicator_->Connect();
    EXPECT_TRUE(connect_result.has_value());

    // After while the daemon would crash
    {
        std::unique_lock<std::mutex> lock{mutex_};
        received_shutdown_request_ = true;
        daemon_is_terminated = true;
        condition_variable_.notify_one();
    }
    EXPECT_EQ(callback_future.wait_for(std::chrono::milliseconds(100)), std::future_status::ready);
}

TEST_F(DaemonCommunicatorFixture, ReceiveDaemonCrashPulseWithoutSubscribeCallback)
{
    bool daemon_is_terminated = false;
    PromiseNotifier deamon_crash_reaction_notification_;
    PromiseNotifier shutdown_notification_;
    EXPECT_CALL(*neutrino_mock_raw_ptr_, ChannelCreate(score::os::qnx::Neutrino::ChannelFlag::kConnectionIdDisconnect))
        .WillOnce(Return(channel_id_));
    EXPECT_CALL(*mock_unistd_, getpid()).WillRepeatedly(Return(valid_process_id_));
    EXPECT_CALL(*channel_mock_raw_ptr_, ConnectAttach).WillOnce(Invoke([this]() {
        deamon_thread_notification_.Notify();
        return connection_id_;
    }));

    EXPECT_CALL(*channel_mock_raw_ptr_, MsgReceivePulse)
        .WillRepeatedly(Invoke([&](const std::int32_t, void* const pulse, const std::size_t, _msg_info* const) {
            std::unique_lock<std::mutex> lock{mutex_};
            std::cout << "Waiting for shutdown request or daemon crash notification ..." << std::endl;
            condition_variable_.wait(lock, [&]() {
                auto input_pulse = reinterpret_cast<struct _pulse*>(pulse);
                if (daemon_is_terminated)
                {
                    input_pulse->code = _PULSE_CODE_COIDDEATH;
                    input_pulse->value.sival_int = channel_id_;
                    daemon_is_terminated = false;
                    deamon_crash_reaction_notification_.Notify();
                }
                else if (received_shutdown_request_)
                {
                    input_pulse->code = kPulseCodeStopThread;
                    daemon_is_terminated = false;
                    shutdown_notification_.Notify();
                }
                return true;
            });
            return 1;
        }));
    EXPECT_CALL(*channel_mock_raw_ptr_, MsgSendPulse)
        .WillRepeatedly(Invoke([&](const int32_t, const int32_t, const int32_t, const int32_t) {
            {
                std::unique_lock<std::mutex> lock{mutex_};
                received_shutdown_request_ = true;
                condition_variable_.notify_one();
            }
            return score::cpp::blank{};
        }));

    EXPECT_CALL(*channel_mock_raw_ptr_, ConnectDetach);

    EXPECT_CALL(*neutrino_mock_raw_ptr_, ChannelDestroy(_)).WillOnce(Invoke([&](int32_t) {
        return EOK;
    }));

    ExpectConnection();

    daemon_communicator_ = std::make_unique<DaemonCommunicator>(std::move(dispatch_mock_),
                                                                std::move(channel_mock_),
                                                                std::move(mman_mock_),
                                                                std::move(neutrino_mock_),
                                                                std::move(mock_unistd_),
                                                                stop_source_.get_token());
    ASSERT_TRUE(deamon_thread_notification_.WaitForNotificationWithTimeout(std::chrono::milliseconds{100}));
    // Should connect normally
    const auto connect_result = daemon_communicator_->Connect();
    EXPECT_TRUE(connect_result.has_value());

    // After while the daemon would crash
    {
        std::unique_lock<std::mutex> lock{mutex_};
        daemon_is_terminated = true;
        condition_variable_.notify_one();
    }
    ASSERT_TRUE(deamon_crash_reaction_notification_.WaitForNotificationWithTimeout(std::chrono::milliseconds{50}));
    // request shutdown
    daemon_communicator_.reset();
    ASSERT_TRUE(shutdown_notification_.WaitForNotificationWithTimeout(std::chrono::milliseconds{50}));
}

TEST_F(DaemonCommunicatorFixture, ReceiveNotificationOfAnotherConnectedProcessCrashButNotTheDaemon)
{
    std::uint8_t receive_pulse_count = 0;
    constexpr int32_t kDummyChannelId = 0xABCD;
    EXPECT_CALL(*neutrino_mock_raw_ptr_, ChannelCreate(score::os::qnx::Neutrino::ChannelFlag::kConnectionIdDisconnect))
        .WillOnce(Return(channel_id_));
    EXPECT_CALL(*mock_unistd_, getpid()).WillRepeatedly(Return(valid_process_id_));
    EXPECT_CALL(*channel_mock_raw_ptr_, ConnectAttach).WillOnce(Invoke([this]() {
        deamon_thread_notification_.Notify();
        return connection_id_;
    }));

    EXPECT_CALL(*channel_mock_raw_ptr_, MsgReceivePulse)
        .WillRepeatedly(Invoke(
            [this, &receive_pulse_count](const std::int32_t, void* const pulse, const std::size_t, _msg_info* const) {
                {
                    std::unique_lock<std::mutex> lock{mutex_};
                    std::cout << "Waiting for shutdown request or daemon crash notification ..." << std::endl;
                    condition_variable_.wait(lock, [this, &pulse]() {
                        std::cout << "Received shutdown request ..." << std::endl;
                        auto input_pulse = reinterpret_cast<struct _pulse*>(pulse);
                        if (!received_shutdown_request_)
                        {
                            input_pulse->code = _PULSE_CODE_COIDDEATH;
                            input_pulse->value.sival_int = kDummyChannelId;
                        }
                        else
                        {
                            input_pulse->code = kPulseCodeStopThread;
                        }
                        return true;
                    });
                }
                return 1;
            }));
    EXPECT_CALL(*channel_mock_raw_ptr_, MsgSendPulse)
        .WillRepeatedly(Invoke([this](const int32_t, const int32_t, const int32_t, const int32_t) {
            ShutdownDaemonCrashDetectorThread();
            return score::cpp::blank{};
        }));

    EXPECT_CALL(*channel_mock_raw_ptr_, ConnectDetach);

    EXPECT_CALL(*neutrino_mock_raw_ptr_, ChannelDestroy(_)).WillOnce(Invoke([this](int32_t) {
        deamon_thread_notification_.Notify();
        return EOK;
    }));
    ExpectConnection();
    daemon_communicator_ = std::make_unique<DaemonCommunicator>(std::move(dispatch_mock_),
                                                                std::move(channel_mock_),
                                                                std::move(mman_mock_),
                                                                std::move(neutrino_mock_),
                                                                std::move(mock_unistd_),
                                                                stop_source_.get_token());
    // wait for a little to give time for the daemon crash detector thread to be alive
    ASSERT_TRUE(deamon_thread_notification_.WaitForNotificationWithTimeout(std::chrono::milliseconds{300}));

    // After while another process (not the daemon) would crash
    {
        std::unique_lock<std::mutex> lock{mutex_};
        received_shutdown_request_ = false;
        condition_variable_.notify_one();
    }

    ExpectConnectionClose();
    CheckConnection();

    ShutdownDaemonCrashDetectorThread();
    ASSERT_TRUE(deamon_thread_notification_.WaitForNotificationWithTimeout(std::chrono::milliseconds{100}));
}

TEST_F(DaemonCommunicatorFixture, CleanUpCrashDetectorThreadInCaseOfInvalidChannelID)
{
    std::uint8_t receive_pulse_count = 0;
    constexpr int32_t kInvalidID = -1;
    EXPECT_CALL(*neutrino_mock_raw_ptr_, ChannelCreate(score::os::qnx::Neutrino::ChannelFlag::kConnectionIdDisconnect))
        .WillOnce(Return(kInvalidID));
    EXPECT_CALL(*mock_unistd_, getpid()).WillRepeatedly(Return(valid_process_id_));
    EXPECT_CALL(*channel_mock_raw_ptr_, ConnectAttach).WillOnce(Return(connection_id_));

    EXPECT_CALL(*channel_mock_raw_ptr_, MsgReceivePulse)
        .WillRepeatedly(Invoke(
            [this, &receive_pulse_count](const std::int32_t, void* const pulse, const std::size_t, _msg_info* const) {
                constexpr std::int8_t unexpected_pulse_code = 77;
                receive_pulse_count++;
                // request stop after some time
                if (receive_pulse_count > 10)
                {
                    stop_source_.request_stop();
                }
                auto input_pulse = reinterpret_cast<struct _pulse*>(pulse);
                input_pulse->code = unexpected_pulse_code;
                std::this_thread::sleep_for(std::chrono::milliseconds{10});
                return 0;
            }));
    EXPECT_CALL(*channel_mock_raw_ptr_, ConnectDetach).WillOnce(Invoke([this]() {
        deamon_thread_notification_.Notify();
        return score::cpp::blank{};
    }));
    daemon_communicator_ = std::make_unique<DaemonCommunicator>(std::move(dispatch_mock_),
                                                                std::move(channel_mock_),
                                                                std::move(mman_mock_),
                                                                std::move(neutrino_mock_),
                                                                std::move(mock_unistd_),
                                                                stop_source_.get_token());
    // wait for a little to give time for the daemon crash detector thread to be alive
    ASSERT_TRUE(deamon_thread_notification_.WaitForNotificationWithTimeout(std::chrono::milliseconds{2 * 300}));

    const auto connect_result = daemon_communicator_->Connect();
    EXPECT_FALSE(connect_result.has_value());
}

TEST_F(DaemonCommunicatorFixture, CleanUpCrashDetectorThreadInCaseOfInvalidConnectionID)
{
    std::uint8_t receive_pulse_count = 0;
    constexpr int32_t kInvalidID = -1;
    EXPECT_CALL(*neutrino_mock_raw_ptr_, ChannelCreate(score::os::qnx::Neutrino::ChannelFlag::kConnectionIdDisconnect))
        .WillOnce(Return(channel_id_));
    EXPECT_CALL(*mock_unistd_, getpid()).WillRepeatedly(Return(valid_process_id_));
    EXPECT_CALL(*channel_mock_raw_ptr_, ConnectAttach).WillOnce(Return(kInvalidID));

    EXPECT_CALL(*channel_mock_raw_ptr_, MsgReceivePulse)
        .WillRepeatedly(Invoke(
            [this, &receive_pulse_count](const std::int32_t, void* const pulse, const std::size_t, _msg_info* const) {
                constexpr std::int8_t unexpected_pulse_code = 77;
                receive_pulse_count++;
                // request stop after some time
                if (receive_pulse_count > 10)
                {
                    stop_source_.request_stop();
                    deamon_thread_notification_.Notify();
                }
                auto input_pulse = reinterpret_cast<struct _pulse*>(pulse);
                input_pulse->code = unexpected_pulse_code;
                std::this_thread::sleep_for(std::chrono::milliseconds{10});
                return 0;
            }));
    EXPECT_CALL(*neutrino_mock_raw_ptr_, ChannelDestroy);

    daemon_communicator_ = std::make_unique<DaemonCommunicator>(std::move(dispatch_mock_),
                                                                std::move(channel_mock_),
                                                                std::move(mman_mock_),
                                                                std::move(neutrino_mock_),
                                                                std::move(mock_unistd_),
                                                                stop_source_.get_token());
    // wait for a little to give time for the daemon crash detector thread to be alive
    ASSERT_TRUE(deamon_thread_notification_.WaitForNotificationWithTimeout(std::chrono::milliseconds{300}));

    const auto connect_result = daemon_communicator_->Connect();
    EXPECT_FALSE(connect_result.has_value());
}
