///
/// @file daemon_communicator.h
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Daemon communicator header file. Purpose of this class is to provide a non-functional stub class for the
/// linux build.
///

#ifndef SCORE_ANALYSIS_TRACING_LIB_GENERIC_TRACE_API_DAEMON_COMMUNICATOR_STUB_H
#define SCORE_ANALYSIS_TRACING_LIB_GENERIC_TRACE_API_DAEMON_COMMUNICATOR_STUB_H

#include "score/analysis/tracing/common/interface_types/types.h"

#include "score/analysis/tracing/library/generic_trace_api/daemon_communicator/i_daemon_communicator.h"

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief DaemonCommunicator class
///
/// Class used to communicate with the Daemon.
class DaemonCommunicator : public IDaemonCommunicator
{
  public:
    /// @brief Connect to Daemon.
    ///
    /// @return True if connection was successful, False otherwise.
    ResultBlank Connect() override;

    /// @copydoc IDaemonCommunicatorShm::RegisterSharedMemoryObject(GenericTraceAPI::TraceClientId, const int)
    RegisterSharedMemoryObjectResult RegisterSharedMemoryObject(const std::int32_t) const override;

    /// @copydoc IDaemonCommunicatorShm::RegisterSharedMemoryObject(TraceClientId, const char* const)
    RegisterSharedMemoryObjectResult RegisterSharedMemoryObject(const std::string&) const override;

    /// @copydoc IDaemonCommunicatorShm::RegisterClient(const BindingType&, const AppIdType&)
    RegisterClientResult RegisterClient(const BindingType& binding_type,
                                        const AppIdType& app_instance_identifier) const override;

    /// @copydoc IDaemonCommunicatorShm::UnregisterSharedMemoryObject(TraceClientId,
    ResultBlank UnregisterSharedMemoryObject(const ShmObjectHandle) const override;

    /// @brief Subscribe a callback which will be called when LTPM daemon is terminated
    void SubscribeToDaemonTerminationNotification(DaemonTerminationCallback callback) override;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_LIB_GENERIC_TRACE_API_DAEMON_COMMUNICATOR_STUB_H
