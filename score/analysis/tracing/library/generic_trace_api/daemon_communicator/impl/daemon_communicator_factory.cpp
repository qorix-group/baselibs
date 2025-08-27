///
/// @file daemon_communicator.cpp
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Daemon communicator factory source file. Purpose of this class is to create the daemon communicator instance
/// and do the needed initialization
///

#include "score/analysis/tracing/library/generic_trace_api/daemon_communicator/daemon_communicator_factory.h"
#include "score/analysis/tracing/library/generic_trace_api/daemon_communicator/impl/daemon_communicator.h"

namespace score
{
namespace analysis
{
namespace tracing
{
// This is intended, we need to use the default argument to ensure testability
// NOLINTBEGIN(google-default-arguments) see comment above
/* No issue here as createDaemonCommunicator is defined multiple times here ans also in stub folder*/
// coverity[autosar_cpp14_m3_2_4_violation]
// coverity[autosar_cpp14_m3_2_2_violation]
std::unique_ptr<IDaemonCommunicator> DaemonCommunicatorFactory::CreateDaemonCommunicator(
    std::unique_ptr<IDaemonCommunicator> daemon_communicator) const
{
    // NOLINTEND(google-default-arguments) see comment above

    if (daemon_communicator == nullptr)
    {
        daemon_communicator = std::make_unique<DaemonCommunicator>();
    }

    return daemon_communicator;
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
