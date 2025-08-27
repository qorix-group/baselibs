///
/// @file i_daemon_communicator.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Daemon communicator interface header file. Purpose of this class is to enable communication between the
/// Generic Trace API and the Daemon, in order to register and unregister shared-memory objects
///

#ifndef GENERIC_TRACE_API_I_DAEMON_COMMUNICATOR_H
#define GENERIC_TRACE_API_I_DAEMON_COMMUNICATOR_H

#include "score/analysis/tracing/common/interface_types/types.h"

namespace score
{
namespace analysis
{
namespace tracing
{

class IDaemonCommunicator
{
  public:
    /// @brief Callback to notify about Daemon terminate or crash
    using DaemonTerminationCallback = std::function<void()>;
    /// @brief Destructor.
    virtual ~IDaemonCommunicator() = default;
    IDaemonCommunicator() noexcept = default;
    IDaemonCommunicator(const IDaemonCommunicator&) noexcept = delete;
    IDaemonCommunicator(IDaemonCommunicator&&) noexcept = delete;
    IDaemonCommunicator& operator=(const IDaemonCommunicator&) = delete;
    IDaemonCommunicator& operator=(IDaemonCommunicator&&) = delete;
    /// @brief Connect to Daemon.
    ///
    /// @return True if connection was successful, False otherwise.
    virtual ResultBlank Connect() = 0;

    /// @brief Send a message to the Daemon requesting to register a shared-memory object using a file descriptor and a
    /// client id. This function waits until a response from the Daemon is received
    ///
    /// @param[in] file_descriptor payload to be sent to the Daemon
    ///
    /// @return a \p GenericTraceAPI::RegisterSharedMemoryObjectResult
    virtual RegisterSharedMemoryObjectResult RegisterSharedMemoryObject(const std::int32_t file_descriptor) const = 0;

    /// @brief Send a message to the Daemon requesting to register a shared-memory object using a path and a client id.
    /// This function waits until a response from the Daemon is received
    ///
    /// @param[in] path payload to be sent to the Daemon
    ///
    /// @return a \p GenericTraceAPI::RegisterSharedMemoryObjectResult
    virtual RegisterSharedMemoryObjectResult RegisterSharedMemoryObject(const std::string& path) const = 0;

    /// @brief Send a message to the Daemon requesting to register an IPC Trace client using the provided binding type
    /// and app instance identifier This function waits until a response from the Daemon is received
    ///
    /// @param[in] binding_type A client needs to call this function for every binding type that is using
    /// @param[in] app_instance_identifier Uniquely identifies the IPC tracing client
    ///
    /// @return a \p GenericTraceAPI::RegisterClientResult
    virtual RegisterClientResult RegisterClient(const BindingType& binding_type,
                                                const AppIdType& app_instance_identifier) const = 0;

    /// @brief Send a message to the Daemon requesting to unregister a shared-memory object using its valid \p
    /// ShmObjectHandle and a client id. This function waits until a response from the Daemon is received
    ///
    /// @param[in] handle payload to be sent to the Daemon
    ///
    /// @return a \p ResultBlank
    virtual ResultBlank UnregisterSharedMemoryObject(const ShmObjectHandle handle) const = 0;

    /// @brief Subscribe a callback which will be called when LTPM daemon is terminated
    virtual void SubscribeToDaemonTerminationNotification(DaemonTerminationCallback callback) = 0;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_I_DAEMON_COMMUNICATOR_H
