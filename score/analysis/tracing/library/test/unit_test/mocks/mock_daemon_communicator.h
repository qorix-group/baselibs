///
/// @file mock_daemon_communicator.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Daemon Communicator mock header file
///

#ifndef GENERIC_TRACE_API_MOCK_DAEMON_COMMUNICATOR_H
#define GENERIC_TRACE_API_MOCK_DAEMON_COMMUNICATOR_H

#include "score/analysis/tracing/library/generic_trace_api/daemon_communicator/i_daemon_communicator.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief MockDaemonCommunicator class
class MockDaemonCommunicator : public IDaemonCommunicator
{
  public:
    ~MockDaemonCommunicator() override = default;
    MOCK_METHOD(ResultBlank, Connect, (), (override));
    MOCK_METHOD(RegisterSharedMemoryObjectResult, RegisterSharedMemoryObject, (const std::int32_t), (const, override));
    MOCK_METHOD(RegisterSharedMemoryObjectResult, RegisterSharedMemoryObject, (const std::string&), (const, override));
    MOCK_METHOD(RegisterClientResult, RegisterClient, (const BindingType&, const AppIdType&), (const, override));
    MOCK_METHOD(ResultBlank, UnregisterSharedMemoryObject, (const ShmObjectHandle), (const, override));
    MOCK_METHOD(void, SubscribeToDaemonTerminationNotification, (DaemonTerminationCallback), (override));
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_MOCK_DAEMON_COMMUNICATOR_H
