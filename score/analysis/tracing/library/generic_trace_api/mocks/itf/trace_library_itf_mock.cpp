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
#include "score/analysis/tracing/library/generic_trace_api/mocks/itf/trace_library_itf_mock.h"
#include "score/analysis/tracing/library/generic_trace_api/object_factory.h"
#include "score/analysis/tracing/library/interface/generic_trace_api.h"
#include "score/concurrency/interruptible_wait.h"
#include "score/memory/shared/shared_memory_factory.h"
#include "score/os/utils/thread.h"
#include "score/mw/log/logging.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <thread>

namespace score
{
namespace analysis
{
namespace tracing
{
namespace
{
static constexpr auto kTmdSharedMemorySize = 3145728U;
}
std::string BindingTypeToString(BindingType binding)
{
    switch (binding)
    {
        case BindingType::kLoLa:
        {
            return "kLola";
        }
        case BindingType::kVector:
        {
            return "kVector";
        }
        case BindingType::kVectorZeroCopy:
        {
            return "kVectorZeroCopy";
        }
        case BindingType::kUndefined:
        default:
        {
            return "kUndefined";
        }
    }
}

TraceLibraryItfMock::TraceLibraryItfMock(std::unique_ptr<score::os::Unistd> unistd_impl)
    : ITraceLibrary(), unistd_impl_{std::move(unistd_impl)}
{
    client_ids = 1;
    GenericTraceAPI::InjectMock(this);
    CreateAndRegisterTraceMetaDataSharedMemory();
}
TraceLibraryItfMock::~TraceLibraryItfMock()
{
    GenericTraceAPI::InjectMock(nullptr);
    UnregisterAndRemoveTraceMetaDataSharedMemory();
}
RegisterClientResult TraceLibraryItfMock::RegisterClient(const BindingType binding,
                                                         const std::string& app_instance_identifier)
{
    if ((app_instance_identifier.empty()) || (binding >= BindingType::kUndefined))
    {
        score::mw::log::LogError()
            << "error(\"LIB\"): GenericTraceAPIImpl::RegisterClient: Invalid argument: app_instance_identifier";
        return MakeUnexpected(ErrorCode::kInvalidArgumentFatal);
    }
    // truncate the first 8 characters of the given app id
    AppIdType truncated_app_identifier{};
    // fill white spaces as a default value for the eight characters
    truncated_app_identifier.fill('\0');

    if (app_instance_identifier.size() >= kApplicationIdentifierLength)
    {
        std::copy(app_instance_identifier.cbegin(),
                  app_instance_identifier.cbegin() + kApplicationIdentifierLength,
                  truncated_app_identifier.begin());
    }
    else
    {
        std::copy(app_instance_identifier.cbegin(), app_instance_identifier.cend(), truncated_app_identifier.begin());
    }
    const auto register_client_id = client_ids.fetch_add(1);
    score::mw::log::LogInfo() << __func__ << " {\"binding\": \"" << BindingTypeToString(binding)
                            << "\", \"instance_indentifier\": \"" << app_instance_identifier
                            << "\", \"truncated_app_identifier\": \"" << truncated_app_identifier << "\", "
                            << "\", \"client_id\": " << register_client_id << " }";
    return register_client_id;
}
RegisterSharedMemoryObjectResult TraceLibraryItfMock::RegisterShmObject(const TraceClientId trace_client_id,
                                                                        const std::string& shm_object_path)
{
    score::mw::log::LogInfo() << __func__ << " { \"trace_client_id\": " << static_cast<unsigned>(trace_client_id)
                            << ", \"shm_object_path\": \"" << shm_object_path << "\" }";
    return {1};
}
RegisterSharedMemoryObjectResult TraceLibraryItfMock::RegisterShmObject(const TraceClientId trace_client_id,
                                                                        std::int32_t shm_object_fd)
{
    score::mw::log::LogInfo() << __func__ << " { \"trace_client_id\": " << static_cast<unsigned>(trace_client_id)
                            << ", \"shm_object_fd\": " << shm_object_fd << " }";
    return {1};
}
ResultBlank TraceLibraryItfMock::UnregisterShmObject(const TraceClientId trace_client_id, ShmObjectHandle handle)
{
    score::mw::log::LogInfo() << __func__ << " { \"trace_client_id\": " << static_cast<unsigned>(trace_client_id)
                            << ", \"handle\": " << handle << " }";
    return {};
}
RegisterTraceDoneCallBackResult TraceLibraryItfMock::RegisterTraceDoneCB(const TraceClientId trace_client_id,
                                                                         TraceDoneCallBackType callback)
{
    score::mw::log::LogInfo() << __func__ << " { \"trace_client_id\": " << static_cast<unsigned>(trace_client_id) << " }";
    if (callback_map_.find(trace_client_id) != callback_map_.end())
    {
        return MakeUnexpected(ErrorCode::kCallbackAlreadyRegisteredRecoverable);
    }
    callback_map_[trace_client_id] = std::move(callback);
    return {};
}

TraceResult TraceLibraryItfMock::Trace(const TraceClientId trace_client_id,
                                       const MetaInfoVariants::type&,
                                       ShmDataChunkList&,
                                       TraceContextId context_id)
{
    std::ostringstream logstream;
    const auto& now = std::chrono::steady_clock::now();
    logstream << __func__ << " { \"trace_client_id\": " << static_cast<unsigned>(trace_client_id) << ", "
              << "\"context_id\": " << static_cast<unsigned>(context_id) << ", ";
    auto& tt = trace_timing_map_[trace_client_id];
    // Call period calculation
    if (tt.has_value())
    {
        const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - tt.value());
        logstream << "\"last_call_ms_ago\": " << diff.count() << ", ";
    }
    else
    {
        logstream << "\"last_call_ms_ago\": null, ";
    }
    // Callback handle
    if (callback_map_.find(trace_client_id) != callback_map_.end())
    {
        logstream << "\"callback_called\": true";
        score::cpp::ignore = callback_map_.at(trace_client_id)(context_id);
    }
    else
    {
        logstream << "\"callback_called\": false";
    }
    logstream << " }";
    // Update
    tt = now;
    score::mw::log::LogInfo() << logstream.str();

    return {};
}

TraceResult TraceLibraryItfMock::Trace(const TraceClientId trace_client_id,
                                       const MetaInfoVariants::type&,
                                       LocalDataChunkList&) noexcept
{
    std::ostringstream logstream;
    const auto& now = std::chrono::steady_clock::now();
    logstream << __func__ << " { \"trace_client_id\": " << static_cast<unsigned>(trace_client_id) << ", "
              << "\"context_id\": null, ";
    auto& tt = trace_timing_map_[trace_client_id];
    // Call period calculation
    if (tt.has_value())
    {
        const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - tt.value());
        logstream << "\"last_call_ms_ago\": " << diff.count() << ", ";
    }
    else
    {
        logstream << "\"last_call_ms_ago\": null, ";
    }
    logstream << "\"callback_called\": false";
    logstream << " }";
    // Update
    tt = now;
    score::mw::log::LogInfo() << logstream.str();
    return {};
}

RegisterSharedMemoryObjectResult TraceLibraryItfMock::CreateAndRegisterTraceMetaDataSharedMemory()
{
    memory::shared::SharedMemoryFactory::WorldWritable permissions{};
    // the shared-memory path should be dev_tmd_{pid}
    // Concatenate the PID to the path_name
    std::stringstream path_stream;
    path_stream << "/dev_tmd_" << unistd_impl_->getpid();
    auto len = path_stream.str().size();
    // No harm from ignorning the returned value here
    //   coverity[autosar_cpp14_a0_1_2_violation]
    std::strncpy(
        trace_meta_data_shared_memory_path_.data(), path_stream.str().c_str(), kMaxxSizeForTmdSharedMemoryPath);

    if ((len <= 0U) ||
        (static_cast<std::size_t>(len) >= trace_meta_data_shared_memory_path_.size()))  // Ensure buffer safety
    {
        return MakeUnexpected(ErrorCode::kSharedMemoryObjectRegistrationFailedFatal);
    }

    memory::shared::SharedMemoryFactory::SetTypedMemoryProvider(memory::shared::TypedMemory::Default());
    // create the shared memory for trace meta data
    trace_metadata_memory_resource_ = memory::shared::SharedMemoryFactory::Create(
        std::string(static_cast<const char*>(trace_meta_data_shared_memory_path_.data())),
        [](auto&&) {},
        kTmdSharedMemorySize,
        {permissions},
        true);

    if ((trace_metadata_memory_resource_ == nullptr) || (trace_metadata_memory_resource_->getBaseAddress() == nullptr))
    {
        std::cerr << "debug(\"LIB\"): " << std::string(static_cast<const char*>(__func__))
                  << " Failed to create shared memory region";
        score::memory::shared::SharedMemoryFactory::Remove(
            std::string(static_cast<const char*>(trace_meta_data_shared_memory_path_.data())));
        return MakeUnexpected(ErrorCode::kSharedMemoryObjectRegistrationFailedFatal);
    }

    if (!trace_metadata_memory_resource_->IsShmInTypedMemory())
    {  // clang-format off
        // No harm from calling the function in that format
        //   coverity[autosar_cpp14_m8_4_4_violation]
        std::cerr << "debug(\"LIB\"): " << std::string(static_cast<const char*>(__func__))<< " The allocated shared memory region is not typed memory" << std::endl;
        // clang-format on

        score::memory::shared::SharedMemoryFactory::Remove(trace_meta_data_shared_memory_path_.data());
        return MakeUnexpected(ErrorCode::kSharedMemoryObjectNotInTypedMemoryFatal);
    }

    return {0};
}

ResultBlank TraceLibraryItfMock::UnregisterAndRemoveTraceMetaDataSharedMemory()
{
    memory::shared::SharedMemoryFactory::Remove(trace_meta_data_shared_memory_path_.data());
    return {};
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
