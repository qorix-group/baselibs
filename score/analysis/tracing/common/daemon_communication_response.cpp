///
/// @file daemon_communication_response.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API daemon communication response source file
///

#include "score/analysis/tracing/common/daemon_communication_response.h"

namespace score
{
namespace analysis
{
namespace tracing
{

const response::UnregisterSharedMemoryObject& Response::GetUnregisterSharedMemoryObject() const
{
    return score::cpp::get<response::UnregisterSharedMemoryObject>(response_);
}

void Response::SetUnregisterSharedMemoryObject(
    const response::UnregisterSharedMemoryObject& unregister_shared_memory_object_response)
{
    response_ = unregister_shared_memory_object_response;
}

const response::RegisterSharedMemoryObject& Response::GetRegisterSharedMemoryObject() const
{
    return score::cpp::get<response::RegisterSharedMemoryObject>(response_);
}

void Response::SetRegisterSharedMemoryObject(
    const response::RegisterSharedMemoryObject& register_shared_memory_object_response)
{
    response_ = register_shared_memory_object_response;
}

void Response::SetDaemonProcessId(const response::DaemonProcessId& daemon_process_id_request)
{
    response_ = daemon_process_id_request;
}

const response::DaemonProcessId& Response::GetDaemonProcessId() const
{
    return score::cpp::get<response::DaemonProcessId>(response_);
}

void Response::SetTraceClientId(const response::RegisterClient& trace_client_id_response)
{
    response_ = trace_client_id_response;
}

const response::RegisterClient& Response::GetTraceClientId() const
{
    return score::cpp::get<response::RegisterClient>(response_);
}

void Response::SetErrorCode(const response::ErrorCode& error_code)
{
    response_ = error_code;
}

const response::ErrorCode& Response::GetErrorCode() const
{
    return score::cpp::get<response::ErrorCode>(response_);
}

VariantType Response::GetVariantType() const
{
    if (score::cpp::holds_alternative<response::RegisterSharedMemoryObject>(response_))
    {
        return VariantType::RegisterSharedMemoryObject;
    }
    if (score::cpp::holds_alternative<response::UnregisterSharedMemoryObject>(response_))
    {
        return VariantType::UnregisterSharedMemoryObject;
    }
    if (score::cpp::holds_alternative<response::DaemonProcessId>(response_))
    {
        return VariantType::DaemonProcessId;
    }
    if (score::cpp::holds_alternative<response::RegisterClient>(response_))
    {
        return VariantType::RegisterClient;
    }
    return VariantType::ErrorCode;
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
