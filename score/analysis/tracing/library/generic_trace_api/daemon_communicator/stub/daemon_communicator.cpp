#include "score/analysis/tracing/library/generic_trace_api/daemon_communicator/stub/daemon_communicator.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"

namespace score
{
namespace analysis
{
namespace tracing
{

ResultBlank DaemonCommunicator::Connect()
{
    return MakeUnexpected(ErrorCode::kServerConnectionNameOpenFailedFatal);
}

RegisterSharedMemoryObjectResult DaemonCommunicator::RegisterSharedMemoryObject(const std::int32_t) const
{
    return MakeUnexpected(ErrorCode::kServerConnectionNameOpenFailedFatal);
}

RegisterSharedMemoryObjectResult DaemonCommunicator::RegisterSharedMemoryObject(const std::string&) const
{
    return MakeUnexpected(ErrorCode::kServerConnectionNameOpenFailedFatal);
}

RegisterClientResult DaemonCommunicator::RegisterClient(const BindingType&, const AppIdType&) const
{
    return MakeUnexpected(ErrorCode::kServerConnectionNameOpenFailedFatal);
}

ResultBlank DaemonCommunicator::UnregisterSharedMemoryObject(const ShmObjectHandle) const
{
    return MakeUnexpected(ErrorCode::kServerConnectionNameOpenFailedFatal);
}

void DaemonCommunicator::SubscribeToDaemonTerminationNotification(DaemonTerminationCallback) {}
}  // namespace tracing
}  // namespace analysis
}  // namespace score
