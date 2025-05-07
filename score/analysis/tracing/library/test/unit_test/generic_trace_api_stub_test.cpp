/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
#include "score/analysis/tracing/library/generic_trace_api/generic_trace_api.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <thread>
using namespace score::analysis::tracing;
using ::testing::Return;

namespace score
{
namespace analysis
{
namespace tracing
{

TEST(GenericTraceAPIImpl, CheckMethodsReturn)
{
    const BindingType type = BindingType::kVector;
    const std::string app_instance_identifier = "TestApp";
    const TraceClientId client = 0;
    const std::string shm_object_path = "/dev/test/path";
    const std::int32_t shm_object_fd = 0x00BBCCDD;
    const ShmObjectHandle handle = 0;
    TraceDoneCallBackType trace_done_callback = score::safecpp::MoveOnlyScopedFunction<void(TraceContextId)>{};
    const DltMetaInfo meta_info{DltProperties{}};
    ShmDataChunkList shm_data_chunk_list{{{0, 0}, 0}};
    const TraceContextId context_id = 23;
    LocalDataChunkList local_data_chunk_list{{nullptr, 0}};

    auto register_client_result = GenericTraceAPI::RegisterClient(type, app_instance_identifier);
    EXPECT_TRUE(register_client_result.has_value());
    EXPECT_EQ(register_client_result.value(), client);

    auto register_shm_object_path_result = GenericTraceAPI::RegisterShmObject(client, shm_object_path);
    EXPECT_TRUE(register_shm_object_path_result.has_value());
    EXPECT_EQ(register_shm_object_path_result.value(), handle);

    auto register_shm_object_fd_result = GenericTraceAPI::RegisterShmObject(client, shm_object_fd);
    EXPECT_TRUE(register_shm_object_fd_result.has_value());
    EXPECT_EQ(register_shm_object_fd_result.value(), handle);

    auto unregister_shm_object_result = GenericTraceAPI::UnregisterShmObject(client, handle);
    EXPECT_TRUE(unregister_shm_object_result.has_value());

    auto register_trace_done_cb_result = GenericTraceAPI::RegisterTraceDoneCB(client, std::move(trace_done_callback));
    EXPECT_TRUE(register_trace_done_cb_result.has_value());

    auto trace_shm_result = GenericTraceAPI::Trace(client, meta_info, shm_data_chunk_list, context_id);
    EXPECT_TRUE(trace_shm_result.has_value());

    auto trace_local_result = GenericTraceAPI::Trace(client, meta_info, local_data_chunk_list);
    EXPECT_TRUE(trace_local_result.has_value());
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
