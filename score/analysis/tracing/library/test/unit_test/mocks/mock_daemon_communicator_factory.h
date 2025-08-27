///
/// @file mock_daemon_communicator.h
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Daemon Communicator mock header file
///

#ifndef GENERIC_TRACE_API_MOCK_DAEMON_COMMUNICATOR_FACTORY_H
#define GENERIC_TRACE_API_MOCK_DAEMON_COMMUNICATOR_FACTORY_H

#include "score/analysis/tracing/library/generic_trace_api/daemon_communicator/daemon_communicator_factory_interface.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief MockDaemonCommunicator class
class MockDaemonCommunicatorFactory : public IDaemonCommunicatorFactory
{
  public:
    ~MockDaemonCommunicatorFactory() override = default;
    MOCK_METHOD(std::unique_ptr<IDaemonCommunicator>,
                CreateDaemonCommunicator,
                (std::unique_ptr<IDaemonCommunicator> daemon_communicator),
                (const, override));
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_MOCK_DAEMON_COMMUNICATOR_FACTORY_H
