///
/// @file generic_trace_api_test.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API generic_trace_api test source
///

#include "score/analysis/tracing/library/interface/generic_trace_api.h"
#include "score/analysis/tracing/library/generic_trace_api/mocks/trace_library_mock.h"
#include "score/analysis/tracing/library/test/unit_test/mocks/mock_daemon_communicator.h"
#include "score/analysis/tracing/library/test/unit_test/mocks/mock_object_factory.h"
#include "score/language/safecpp/scoped_function/move_only_scoped_function.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <thread>
using namespace score::analysis::tracing;
using ::testing::Return;

class GenericTraceApiFixture : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        trace_library_mock_instance_ = std::make_unique<TraceLibraryMock>();
        trace_library_mock_ = trace_library_mock_instance_.get();
    }
    void TearDown() override
    {
        trace_library_mock_instance_.reset();
    }

    std::unique_ptr<TraceLibraryMock> trace_library_mock_instance_;
    TraceLibraryMock* trace_library_mock_;
};

TEST_F(GenericTraceApiFixture, TestRegisterCLient)
{
    RecordProperty("Verifies", "SCR-39683004");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "Test verifies if RegisterClient API exists, takes 2 args and returns unique ID");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const BindingType type = BindingType::kVector;
    const std::string app_instance_identifier = "TestApp";

    EXPECT_CALL(*trace_library_mock_, RegisterClient(type, app_instance_identifier)).Times(1);
    auto client_id = GenericTraceAPI::RegisterClient(type, app_instance_identifier);
    EXPECT_TRUE(client_id.has_value());
}

TEST_F(GenericTraceApiFixture, TestRegisterShmObjectWithPath)
{
    const TraceClientId client = 1;
    const std::string shm_object_path = "/dev/test/path";

    EXPECT_CALL(*trace_library_mock_, RegisterShmObject(client, shm_object_path)).Times(1);
    GenericTraceAPI::RegisterShmObject(client, shm_object_path);
}

TEST_F(GenericTraceApiFixture, TestRegisterShmObjectWithFileDescriptor)
{
    const TraceClientId client = 1;
    const std::int32_t shm_object_fd = 0x00BBCCDD;

    EXPECT_CALL(*trace_library_mock_, RegisterShmObject(client, shm_object_fd)).Times(1);
    GenericTraceAPI::RegisterShmObject(client, shm_object_fd);
}

TEST_F(GenericTraceApiFixture, TestUnregisterShmObject)
{
    RecordProperty("Verifies", "SCR-39688973");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if GTL provides an API to unregister a previously registered shared memory area");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const TraceClientId client = 1;
    const ShmObjectHandle handle = 1;

    EXPECT_CALL(*trace_library_mock_, UnregisterShmObject(client, handle)).Times(1);
    GenericTraceAPI::UnregisterShmObject(client, handle);
}

TEST_F(GenericTraceApiFixture, TestRegisterTraceDoneCallBack)
{
    RecordProperty("Verifies", "SCR-39687939");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "Test verifies if GTL provides an API to set the release callback per client");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const TraceClientId client = 1;
    TraceDoneCallBackType trace_done_callback = score::safecpp::MoveOnlyScopedFunction<void(TraceContextId)>{};

    EXPECT_CALL(*trace_library_mock_, RegisterTraceDoneCB(client, ::testing::_)).Times(1);
    GenericTraceAPI::RegisterTraceDoneCB(client, std::move(trace_done_callback));
}

TEST_F(GenericTraceApiFixture, TestTraceCallWithContextID)
{
    RecordProperty("Verifies", "SCR-39688348");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "Test verifies if Trace API exists and takes ShmDataChunkList and metadata as parameters");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const TraceClientId client = 3;
    const DltMetaInfo meta_info{DltProperties{}};
    ShmDataChunkList shm_data_chunk_list{{{0, 0}, 0}};
    const TraceContextId context_id = 23;

    EXPECT_CALL(*trace_library_mock_, Trace(client, ::testing::_, ::testing::_, context_id)).Times(1);
    GenericTraceAPI::Trace(client, meta_info, shm_data_chunk_list, context_id);
}

TEST_F(GenericTraceApiFixture, TestTraceCallWithoutContextID)
{
    RecordProperty("Verifies", "SCR-32743442");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "Test verifies if Trace API exists and takes LocalDataChunkList parameter");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const TraceClientId client = 3;
    const DltMetaInfo meta_info{DltProperties{}};
    LocalDataChunkList lcl_data_chunk_list{{nullptr, 0}};

    EXPECT_CALL(*trace_library_mock_, Trace(client, ::testing::_, ::testing::_)).Times(1);
    GenericTraceAPI::Trace(client, meta_info, lcl_data_chunk_list);
}

TEST_F(GenericTraceApiFixture, TestGetInstance)
{
    const TraceClientId client = 3;
    const DltMetaInfo meta_info{DltProperties{}};
    LocalDataChunkList lcl_data_chunk_list{{nullptr, 0}};
    trace_library_mock_instance_->uninject();

    auto result = GenericTraceAPI::Trace(client, meta_info, lcl_data_chunk_list);
    EXPECT_EQ(result.error(), ErrorCode::kClientNotFoundRecoverable);
}
