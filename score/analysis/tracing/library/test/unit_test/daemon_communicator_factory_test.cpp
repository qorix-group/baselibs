///
/// @file daemon_communicator_factory_test.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief daemon communicator factory test source file
///

#include "score/analysis/tracing/library/generic_trace_api/daemon_communicator/daemon_communicator_factory.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"
#include "score/analysis/tracing/library/generic_trace_api/object_factory.h"
#include "score/analysis/tracing/library/test/unit_test/mocks/mock_daemon_communicator.h"
#include "score/analysis/tracing/plugin/ipc_trace_plugin/interface/daemon_communication_response.h"
#include "score/memory/shared/shared_memory_factory.h"
#include "score/memory/shared/shared_memory_factory_mock.h"
#include "score/memory/shared/shared_memory_resource_mock.h"
#include "score/os/mocklib/qnx/mock_channel.h"
#include "score/os/mocklib/qnx/mock_dispatch.h"
#include "score/os/mocklib/qnx/mock_mman.h"
#include "score/os/mocklib/qnx/neutrino_qnx_mock.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Eq;
using ::testing::Mock;
using ::testing::Return;

using namespace score::analysis::tracing;

TEST(DaemonCommunicatorFactory, CreateDaemonCommunicatorSuccess)
{
    std::unique_ptr<MockDaemonCommunicator> daemon_communicator_mock = std::make_unique<MockDaemonCommunicator>();
    DaemonCommunicatorFactory daemonCommunicatorFactory;

    auto result = daemonCommunicatorFactory.CreateDaemonCommunicator(std::move(daemon_communicator_mock));
    ASSERT_TRUE(result);
}
