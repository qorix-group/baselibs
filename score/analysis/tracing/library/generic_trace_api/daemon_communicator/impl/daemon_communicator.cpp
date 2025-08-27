///
/// @file daemon_communicator.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Daemon communicator source file. Purpose of this class is to enable communication between the
/// Generic Trace API and the Daemon, in order to register and unregister shared-memory objects
///

#include "score/analysis/tracing/library/generic_trace_api/daemon_communicator/impl/daemon_communicator.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"
#include "score/os/errno.h"
#include "score/os/qnx/sigevent_qnx_impl.h"

#include <iostream>

namespace score
{
namespace analysis
{
namespace tracing
{

namespace
{
static constexpr pid_t kInvalidProcessId{-1};
static constexpr std::int32_t kInvalidChannelId{-1};
static constexpr std::int32_t kInvalidConnectionId{-1};

// "struct _pulse" contains "_Int8t code;" so, pulse code should not cause overloading of variable "code"
static constexpr std::int32_t kPulseCodeStopThread = 100;

// No possibility for data loss here
//  coverity[autosar_cpp14_a4_7_1_violation]
static constexpr auto kMessageSendTimeout(std::chrono::milliseconds{500});
// No harm from implicit integral conversion here
// coverity[autosar_cpp14_m5_0_4_violation]
static constexpr auto kUsedClock{os::qnx::Neutrino::ClockType::kRealtime};
// No harm to do bitwise operations on these values m5_0_21,a4_5_1
//  coverity[autosar_cpp14_m5_0_21_violation]
//  coverity[autosar_cpp14_a4_5_1_violation]
static constexpr auto kMessageSendTimeoutFlags{//  coverity[autosar_cpp14_m5_0_21_violation]
                                               //  coverity[autosar_cpp14_a4_5_1_violation]
                                               os::qnx::Neutrino::TimerTimeoutFlag::kSend |
                                               //  coverity[autosar_cpp14_m5_0_21_violation]
                                               //  coverity[autosar_cpp14_a4_5_1_violation]
                                               os::qnx::Neutrino::TimerTimeoutFlag::kReply};
}  // namespace

DaemonCommunicator::DaemonCommunicator(std::unique_ptr<const os::Dispatch> dispatch,
                                       std::unique_ptr<const os::Channel> channel,
                                       std::unique_ptr<const os::qnx::MmanQnx> mman,
                                       std::unique_ptr<const os::qnx::Neutrino> neutrino,
                                       std::unique_ptr<const score::os::Unistd> unistd_impl,
                                       const std::optional<score::cpp::stop_token>& token_test)
    : IDaemonCommunicator(),
      daemon_process_id_{kInvalidProcessId},
      channel_id_{kInvalidChannelId},
      crash_detector_channel_id_{kInvalidChannelId},
      crash_detector_connection_id_{kInvalidConnectionId},
      daemon_terminated_callback_{nullptr},
      dispatch_{std::move(dispatch)},
      channel_{std::move(channel)},
      mman_{std::move(mman)},
      neutrino_{std::move(neutrino)},
      unistd_impl_{std::move(unistd_impl)},
      daemon_crash_detector_thread_{nullptr}
{
    daemon_crash_detector_thread_ =
        std::make_unique<score::cpp::jthread>([this, token_test](const score::cpp::stop_token& token) -> void {
            score::cpp::stop_token effective_token = token_test.has_value() ? token_test.value() : token;
            DaemonCrashDetector(effective_token);
        });

    os::set_thread_name(*daemon_crash_detector_thread_, "DaemonCrashDetectorThread");
}

DaemonCommunicator::~DaemonCommunicator()
{
    if (channel_id_.load() != kInvalidChannelId)
    {
        // Tolerated unless no alternative
        //  NOLINTNEXTLINE(score-banned-function) see comment above
        const auto name_close_result = dispatch_->name_close(channel_id_.load());
        if (!name_close_result.has_value())
        {
            // clang-format off
        // No harm from using this format in std::cerr
        // coverity[autosar_cpp14_m8_4_4_violation]
            std::cerr << "DaemonCommunicator::" << std::string(static_cast<const char*>(__func__))<< ": "<< name_close_result.error().ToString() << std::endl;// NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) see comment above
            // clang-format on
        }
        daemon_process_id_ = kInvalidProcessId;
        channel_id_.store(kInvalidChannelId);
    }
    Disconnect();
}
// Suppress "AUTOSAR C++14 A15-5-3" rule finding. This rule states: "The std::terminate()
// function shall not be called implicitly"
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// By design, thread is joined() in destructor, and there is no termination API.
// This violation concerns throw exception by join(), and the need to have destructor as noexcept.
// coverity[autosar_cpp14_a15_5_3_violation]
void DaemonCommunicator::Disconnect() noexcept
{
    // clang-format off
    // No harm from using this format in std::cout
    // coverity[autosar_cpp14_m8_4_4_violation]
    std::cout << "DaemonCommunicator is Terminating!" << std::endl;
    // clang-format on

    if (kInvalidConnectionId != crash_detector_connection_id_)
    {
        // clang-format off
        // No harm from using this format in std::cout
        // coverity[autosar_cpp14_m8_4_4_violation]
        std::cout << "DaemonCommunicator::Disconnect(): request_stop()" << std::endl;
        // clang-format on
        score::cpp::ignore = daemon_crash_detector_thread_->request_stop();

        const pid_t process_pid_ = unistd_impl_->getpid();
        // Ignore the return for now
        // Tolerated as the pulse is delivered to an internal channel, not to an external one owned by different
        // process.
        // NOLINTNEXTLINE(score-banned-function) see comment above
        score::cpp::ignore = channel_->MsgSendPulse(
            crash_detector_connection_id_, SIGEV_PULSE_PRIO_INHERIT, kPulseCodeStopThread, process_pid_);
    }
    // Not testable by unit tests since daemon crash detector thread is already created in constructor and it's an
    // active thread so to cover negative case that thread is not joinable we shall deactivate the thread and this is
    // not feasible
    if (daemon_crash_detector_thread_->joinable())  // LCOV_EXCL_BR_LINE not testable see comment above.
    {
        // Suppress "AUTOSAR C++14 A15-4-2" rule findings. This rule states: "Throwing an exception in a "noexcept"
        // function." In this case it is ok, because the system anyways forces the process to terminate if an
        // exception is thrown.
        // coverity[autosar_cpp14_a15_4_2_violation] see above
        daemon_crash_detector_thread_->join();
        daemon_crash_detector_thread_ = nullptr;
    }
}
// No harm here as multiple definitions is needed for testing code(m3_2_4,m3_2_2_)
// coverity[autosar_cpp14_m3_2_4_violation]
// coverity[autosar_cpp14_m3_2_2_violation]
ResultBlank DaemonCommunicator::Connect()
{
    if ((crash_detector_channel_id_ == kInvalidChannelId) || (crash_detector_connection_id_ == kInvalidConnectionId))
    {
        // clang-format off
        // No harm from using this format in std::cerr
        // coverity[autosar_cpp14_m8_4_4_violation]
        std::cerr << "DaemonCommunicator::" << std::string(static_cast<const char*>(__func__))
                // coverity[autosar_cpp14_m8_4_4_violation]
                  << ": The DaemonCrashDetector couldn't be established" << std::endl;
        // clang-format on
        return MakeUnexpected(ErrorCode::kDaemonTerminationDetectionFailedFatal);
    }
    // Tolerated unless no alternative
    //  NOLINTNEXTLINE(score-banned-function) see comment above
    auto name_open_result = dispatch_->name_open(GetServerAddressName(), 0);
    if (!name_open_result.has_value())
    {
        return MakeUnexpected(ErrorCode::kServerConnectionNameOpenFailedFatal);
    }

    channel_id_.store(name_open_result.value());

    Request request{};
    request.SetMessageId(DaemonCommunicatorMessage::kDaemonProcessIdRequest);

    Response response{};
    response.SetDaemonProcessId({});

    const auto send_result = SendMessage(request, response);
    if (!send_result.has_value())
    {
        return send_result;
    }

    daemon_process_id_ = response.GetDaemonProcessId().daemon_process_id;
    return {};
}

void DaemonCommunicator::DaemonCrashDetector(score::cpp::stop_token stop_token)
{
    // clang-format off
    // No harm from calling the function in that format
    //   coverity[autosar_cpp14_m8_4_4_violation]
    std::cout << "DaemonCommunicator::DaemonCrashDetector" << std::endl;
    // clang-format on

    auto channel_create_result = neutrino_->ChannelCreate(os::qnx::Neutrino::ChannelFlag::kConnectionIdDisconnect);
    if (!channel_create_result.has_value())
    {
        // clang-format off
        // No harm from calling the function in that format
        //   coverity[autosar_cpp14_m8_4_4_violation]
        std::cerr << "ERROR: DaemonCommunicator::" << std::string(static_cast<const char*>(__func__)) // LCOV_EXCL_LINE False positive (tool issue).
        //   coverity[autosar_cpp14_m8_4_4_violation]
                  << ": ChannelCreate() failed: " << channel_create_result.error().ToString() << std::endl;
        // clang-format on
        return;
    }
    crash_detector_channel_id_ = channel_create_result.value();

    constexpr uint32_t kCrashDetectorNd = 0U;
    const pid_t crash_detector_pid = unistd_impl_->getpid();
    auto attach_result = channel_->ConnectAttach(kCrashDetectorNd,
                                                 crash_detector_pid,
                                                 crash_detector_channel_id_,
                                                 // coverity[autosar_cpp14_m2_13_3_violation] caused by QNX macros
                                                 // coverity[autosar_cpp14_m5_0_4_violation] caused by QNX macros
                                                 _NTO_SIDE_CHANNEL,
                                                 _NTO_COF_CLOEXEC);
    if (!attach_result.has_value())
    {
        // clang-format off
        // No harm from calling the function in that format
        //   coverity[autosar_cpp14_m8_4_4_violation]
        std::cerr << "ERROR: DaemonCommunicator::" << std::string(static_cast<const char*>(__func__)) // LCOV_EXCL_LINE False positive (tool issue).
        //   coverity[autosar_cpp14_m8_4_4_violation]
                  << ": ConnectAttach() failed: " << attach_result.error().ToString() << std::endl;
        // clang-format on
        crash_detector_channel_id_ = kInvalidChannelId;
        return;
    }
    crash_detector_connection_id_ = attach_result.value();

    while (!stop_token.stop_requested())
    {
        struct _pulse pulse_message{};
        auto receive_code =
            channel_->MsgReceivePulse(crash_detector_channel_id_, &pulse_message, sizeof(pulse_message), nullptr);

        if (!receive_code.has_value())
        {
            // clang-format off
            // No harm from calling the function in that format
            //   coverity[autosar_cpp14_m8_4_4_violation]
            std::cerr << "ERROR: DaemonCommunicator::" << std::string(static_cast<const char*>(__func__))
                    //   coverity[autosar_cpp14_m8_4_4_violation]
                      << ": MsgReceivePulse() failed: " << receive_code.error().ToString()
                    //   coverity[autosar_cpp14_m8_4_4_violation]
                      << "; pulse code: " << static_cast<int>(pulse_message.code) << std::endl;
            // clang-format on
        }
        if (_PULSE_CODE_COIDDEATH == pulse_message.code)
        {
            // LtpmDaemon get terminated
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access) Reading from QNX union here.
            if (pulse_message.value.sival_int == channel_id_.load())
            {
                if (daemon_terminated_callback_)
                {
                    daemon_terminated_callback_();
                }
                channel_id_.store(kInvalidChannelId);
            }
        }
        // The library instance is being terminated
        if (kPulseCodeStopThread == pulse_message.code)
        {
            break;
        }
    }

    CleanupCrashDetectorThread();
}

void DaemonCommunicator::CleanupCrashDetectorThread()
{
    if (kInvalidConnectionId != crash_detector_connection_id_)
    {
        score::cpp::ignore = channel_->ConnectDetach(crash_detector_connection_id_);
        crash_detector_connection_id_ = kInvalidConnectionId;
    }

    if (crash_detector_channel_id_ != kInvalidChannelId)
    {
        // clang-format off
        // No harm from calling the function in that format
        //   coverity[autosar_cpp14_m8_4_4_violation]
        std::cout << "DaemonCommunicator::" << std::string(static_cast<const char*>(__func__))
                //   coverity[autosar_cpp14_m8_4_4_violation]
                  << ": close crash_detector_channel_id_ " << crash_detector_channel_id_ << std::endl;
        // clang-format on

        score::cpp::ignore = neutrino_->ChannelDestroy(crash_detector_channel_id_);
        crash_detector_channel_id_ = kInvalidChannelId;
    }
}

ResultBlank DaemonCommunicator::SendMessage(const Request& request, Response& response) const
{
    iov_t request_message;
    channel_->SetIovConst(&request_message, &request, sizeof(request));

    iov_t response_message;
    channel_->SetIov(&response_message, &response, sizeof(response));

    std::unique_ptr<os::SigEventQnx> signal_event = std::make_unique<os::SigEventQnxImpl>();
    signal_event->SetUnblock();
    const auto set_timeout_result =
        neutrino_->TimerTimeout(kUsedClock, kMessageSendTimeoutFlags, std::move(signal_event), kMessageSendTimeout);
    if (!set_timeout_result.has_value())
    {
        // clang-format off
        // No harm from using this format in std::cerr
        // coverity[autosar_cpp14_m8_4_4_violation]
        std::cerr << "DaemonCommunicator::" << std::string(static_cast<const char*>(__func__)) << ": "
                  << set_timeout_result.error().ToString()
                  << std::endl;  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) see comment above
        // clang-format on
        return MakeUnexpected(ErrorCode::kMessageSendFailedRecoverable);
    }

    const auto message_send_result =
        channel_->MsgSendv(channel_id_.load(), &request_message, 1U, &response_message, 1U);
    if (!message_send_result.has_value())
    {
        // clang-format off
        // No harm from using this format in std::cerr
        // coverity[autosar_cpp14_m8_4_4_violation]
        std::cerr << "DaemonCommunicator::" << std::string(static_cast<const char*>(__func__))<< ": " << message_send_result.error().ToString()<< std::endl;// NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) see comment above
        // clang-format on
        return MakeUnexpected(ErrorCode::kMessageSendFailedRecoverable);
    }
    return {};
}
//  No harm here as multiple definitions is needed for testing code(m3_2_4,m3_2_2)
// coverity[autosar_cpp14_m3_2_4_violation]
// coverity[autosar_cpp14_m3_2_2_violation]
RegisterSharedMemoryObjectResult DaemonCommunicator::RegisterSharedMemoryObject(
    const std::int32_t file_descriptor) const
{
    if ((channel_id_.load() == kInvalidChannelId) || (daemon_process_id_ == kInvalidProcessId))
    {
        return MakeUnexpected(ErrorCode::kDaemonNotConnectedFatal);
    }

    shm_handle_t handle{};
    const auto create_result =
        // No harm from implicit integral conversion here
        // coverity[autosar_cpp14_m5_0_4_violation]
        mman_->shm_create_handle(file_descriptor, daemon_process_id_, kSharedMemoryObjectOpenFlags, &handle, 0U);

    if (!create_result.has_value())
    {
        // clang-format off
        // No harm from using this format in std::cerr
        // coverity[autosar_cpp14_m8_4_4_violation]
        std::cerr << "DaemonCommunicator::" << std::string(static_cast<const char*>(__func__))<< ": " << create_result.error().ToString()<< std::endl;// NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) see comment above
        // clang-format on
        return MakeUnexpected(ErrorCode::kSharedMemoryObjectRegistrationFailedFatal);
    }

    Request request{};
    request.SetMessageId(DaemonCommunicatorMessage::kSharedMemoryObjectRegistrationRequest);
    request.SetSharedMemoryObject({handle});

    Response response{};
    // No harm from using this c-style as input param
    // coverity[autosar_cpp14_a5_2_2_violation]
    response.SetRegisterSharedMemoryObject({ShmObjectHandle{kInvalidSharedObjectIndex}});

    const auto send_result = SendMessage(request, response);
    if (!send_result.has_value())
    {
        return Unexpected{send_result.error()};
    }

    const auto shared_memory_object_index = response.GetRegisterSharedMemoryObject().shared_memory_object_index;
    if (shared_memory_object_index == kInvalidSharedObjectIndex)
    {
        const auto error = MakeUnexpected(ErrorCode::kSharedMemoryObjectHandleCreationFailedFatal);
        // clang-format off
        // No harm from using this format in std::cerr
        // coverity[autosar_cpp14_m8_4_4_violation]
        std::cerr << "DaemonCommunicator::" << std::string(static_cast<const char*>(__func__))<< ": " << error.error().Message()<< std::endl;// NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) see comment above
        // clang-format on
        return error;
    }
    return shared_memory_object_index;
}
//  No harm here as multiple definitions is needed for testing code(m3_2_4,m3_2_2)
// coverity[autosar_cpp14_m3_2_4_violation]
// coverity[autosar_cpp14_m3_2_2_violation]
RegisterSharedMemoryObjectResult DaemonCommunicator::RegisterSharedMemoryObject(const std::string& path) const
{
    if (path.empty())
    {
        return MakeUnexpected(ErrorCode::kInvalidArgumentFatal);
    }

    if (channel_id_.load() == kInvalidChannelId)
    {
        return MakeUnexpected(ErrorCode::kDaemonNotConnectedFatal);
    }

    const auto shm_open_result =
        // No harm from implicit integral conversion here
        // coverity[autosar_cpp14_m5_0_4_violation]
        mman_->shm_open(path.c_str(), kSharedMemoryObjectOpenFlags, kSharedMemoryObjectOpenModes);
    if (!shm_open_result.has_value())
    {
        // clang-format off
        // No harm from using this format in std::cerr
        // coverity[autosar_cpp14_m8_4_4_violation]
        std::cerr << "DaemonCommunicator::" << std::string(static_cast<const char*>(__func__))<< ": " << shm_open_result.error().ToString()<< std::endl;// NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) see comment above
        // clang-format on
        return MakeUnexpected(ErrorCode::kBadFileDescriptorFatal);
    }
    return RegisterSharedMemoryObject(shm_open_result.value());
}
//  No harm here as multiple definitions is needed for testing code (m3-2-2),(m3-2-4)
// coverity[autosar_cpp14_m3_2_2_violation]
// coverity[autosar_cpp14_m3_2_4_violation]
RegisterClientResult DaemonCommunicator::RegisterClient(const BindingType& binding_type,
                                                        const AppIdType& app_instance_identifier) const
{
    if (binding_type >= BindingType::kUndefined)
    {
        return score::MakeUnexpected(ErrorCode::kInvalidBindingTypeFatal);
    }

    Request request{};
    request.SetMessageId(DaemonCommunicatorMessage::kRegisterClientRequest);
    request.SetRegisterClient({binding_type, app_instance_identifier});

    Response response{};
    response.SetTraceClientId({0U});

    const auto send_result = SendMessage(request, response);
    if (!send_result.has_value())
    {
        return MakeUnexpected<TraceClientId>(send_result.error());
    }
    if (response.GetVariantType() == VariantType::RegisterClient)
    {
        return response.GetTraceClientId().trace_client_id;
    }
    else if (response.GetVariantType() == VariantType::ErrorCode)
    {
        // Any other values do not correspond to
        // one of the enumerators of the enumeration will be handled with the default case.
        // coverity[autosar_cpp14_a7_2_1_violation]
        return score::MakeUnexpected(static_cast<ErrorCode>(response.GetErrorCode().error_code));
    }
    else
    {
        return score::MakeUnexpected(ErrorCode::kGenericErrorRecoverable);
    }
}
//  No harm here as multiple definitions is needed for testing code(m3_2_2,m3_2_4)
// coverity[autosar_cpp14_m3_2_2_violation]
// coverity[autosar_cpp14_m3_2_4_violation]
ResultBlank DaemonCommunicator::UnregisterSharedMemoryObject(const ShmObjectHandle handle) const
{
    if (channel_id_.load() == kInvalidChannelId)
    {
        return MakeUnexpected(ErrorCode::kDaemonNotConnectedFatal);
    }

    Request request{};
    request.SetMessageId(DaemonCommunicatorMessage::kSharedMemoryObjectUnregisterRequest);
    request.SetUnregisterSharedMemoryObject({handle});

    Response response{};
    response.SetUnregisterSharedMemoryObject({false});

    const auto send_result = SendMessage(request, response);
    if (!send_result.has_value())
    {
        return send_result;
    }

    if (response.GetUnregisterSharedMemoryObject().is_unregister_successful)
    {
        return {};
    }

    const auto error = MakeUnexpected(ErrorCode::kSharedMemoryObjectUnregisterFailedFatal);
    // clang-format off
    // No harm from using this format in std::cerr
    // coverity[autosar_cpp14_m8_4_4_violation]
    std::cerr << "DaemonCommunicator::" << std::string(static_cast<const char*>(__func__))<< ": " << error.error().Message()<< std::endl;// NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) see comment above
    // clang-format on
    return error;
}

void DaemonCommunicator::SubscribeToDaemonTerminationNotification(DaemonTerminationCallback callback)
{
    daemon_terminated_callback_ = callback;
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
