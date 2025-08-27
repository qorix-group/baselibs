///
/// @file daemon_communicator.cpp
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Daemon communicator factory source file. Purpose of this class is to create a stub instance of the daemon
/// communicator to enable the library build on linux
///

#include "score/analysis/tracing/library/generic_trace_api/daemon_communicator/daemon_communicator_factory.h"
#include "score/analysis/tracing/library/generic_trace_api/daemon_communicator/stub/daemon_communicator.h"

namespace score
{
namespace analysis
{
namespace tracing
{

std::unique_ptr<IDaemonCommunicator> DaemonCommunicatorFactory::CreateDaemonCommunicator(
    std::unique_ptr<IDaemonCommunicator> daemon_communicator) const
{
    if (daemon_communicator == nullptr)
    {
        daemon_communicator = std::make_unique<DaemonCommunicator>();
    }
    return daemon_communicator;
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
