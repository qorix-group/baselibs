///
/// @file daemon_communicator.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Daemon communicator header file. Purpose of this class is to enable communication between the
/// Generic Trace API and the Daemon, in order to register and unregister shared-memory objects
///

#ifndef GENERIC_TRACE_API_DAEMON_COMMUNICATOR_H
#define GENERIC_TRACE_API_DAEMON_COMMUNICATOR_H

#include "score/analysis/tracing/library/generic_trace_api/daemon_communicator/i_daemon_communicator.h"
#include "score/analysis/tracing/plugin/ipc_trace_plugin/interface/daemon_communication_messages.h"
#include "score/analysis/tracing/plugin/ipc_trace_plugin/interface/daemon_communication_request.h"
#include "score/analysis/tracing/plugin/ipc_trace_plugin/interface/daemon_communication_response.h"

#include "score/os/qnx/channel_impl.h"
#include "score/os/qnx/dispatch_impl.h"
#include "score/os/qnx/mman_impl.h"
#include "score/os/qnx/neutrino_impl.h"
#include "score/os/unistd.h"

#include "score/os/utils/thread.h"
#include <atomic>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief DaemonCommunicator class
///
/// Class used to communicate with the Daemon.
// No harm here as this api is defined also in stub folder
// coverity[autosar_cpp14_m3_2_2_violation]
// coverity[autosar_cpp14_m3_2_3_violation]
class DaemonCommunicator : public IDaemonCommunicator
{
  public:
    /// @brief Constructs a DaemonCommunicator object
    explicit DaemonCommunicator(
        std::unique_ptr<const os::Dispatch> dispatch = std::make_unique<const os::DispatchImpl>(),
        std::unique_ptr<const os::Channel> channel = std::make_unique<const os::ChannelImpl>(),
        std::unique_ptr<const os::qnx::MmanQnx> mman = std::make_unique<const os::qnx::MmanQnxImpl>(),
        std::unique_ptr<const os::qnx::Neutrino> neutrino = std::make_unique<const os::qnx::NeutrinoImpl>(),
        std::unique_ptr<const score::os::Unistd> unistd_impl = std::make_unique<const score::os::internal::UnistdImpl>(),
        const std::optional<score::cpp::stop_token>& token_test = std::nullopt);

    DaemonCommunicator(const DaemonCommunicator&) = delete;
    DaemonCommunicator(const DaemonCommunicator&&) = delete;
    DaemonCommunicator& operator=(const DaemonCommunicator&) = delete;
    DaemonCommunicator& operator=(const DaemonCommunicator&&) = delete;

    /// @brief Destructor. Destroys a DaemonCommunicator object.
    ~DaemonCommunicator() override;

    /// @brief Connect to Daemon.
    ///
    /// @return True if connection was successful, False otherwise.
    ResultBlank Connect() override;

    /// @copydoc IDaemonCommunicatorShm::RegisterSharedMemoryObject(const std::int)
    RegisterSharedMemoryObjectResult RegisterSharedMemoryObject(const std::int32_t file_descriptor) const override;

    /// @copydoc IDaemonCommunicatorShm::RegisterSharedMemoryObject(const std::string&)
    RegisterSharedMemoryObjectResult RegisterSharedMemoryObject(const std::string& path) const override;

    /// @copydoc IDaemonCommunicatorShm::RegisterClient(const BindingType&, const AppIdType&)
    RegisterClientResult RegisterClient(const BindingType& binding_type,
                                        const AppIdType& app_instance_identifier) const override;

    /// @copydoc IDaemonCommunicatorShm::UnregisterSharedMemoryObject(const ShmObjectHandle)
    ResultBlank UnregisterSharedMemoryObject(const ShmObjectHandle handle) const override;

    /// @brief Subscribe a callback which will be called when LTPM daemon is terminated
    void SubscribeToDaemonTerminationNotification(DaemonTerminationCallback callback) override;

  private:
    ResultBlank SendMessage(const Request& request, Response& response) const;

    void DaemonCrashDetector(score::cpp::stop_token stop_token);
    void CleanupCrashDetectorThread();
    void Disconnect() noexcept;

    pid_t daemon_process_id_;
    std::atomic<std::int32_t> channel_id_;
    std::atomic<std::int32_t> crash_detector_channel_id_;
    std::atomic<std::int32_t> crash_detector_connection_id_;
    DaemonTerminationCallback daemon_terminated_callback_;
    std::unique_ptr<const os::Dispatch> dispatch_;
    std::unique_ptr<const os::Channel> channel_;
    std::unique_ptr<const os::qnx::MmanQnx> mman_;
    std::unique_ptr<const os::qnx::Neutrino> neutrino_;
    std::unique_ptr<const score::os::Unistd> unistd_impl_;
    std::unique_ptr<score::cpp::jthread> daemon_crash_detector_thread_;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_DAEMON_COMMUNICATOR_H
