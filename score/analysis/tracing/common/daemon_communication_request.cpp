///
/// @file daemon_communication_request.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API daemon communication request source file
///

#include "score/analysis/tracing/common/daemon_communication_request.h"

namespace score
{
namespace analysis
{
namespace tracing
{

void Request::SetMessageId(DaemonCommunicatorMessage message_id)
{
    message_id_ = message_id;
}

const DaemonCommunicatorMessage& Request::GetMessageId() const
{
    return message_id_;
}

const request::UnregisterSharedMemoryObject& Request::GetUnregisterSharedMemoryObject() const
{
    return score::cpp::get<request::UnregisterSharedMemoryObject>(request_);
}

void Request::SetUnregisterSharedMemoryObject(
    const request::UnregisterSharedMemoryObject& unregister_shared_memory_object_request)
{
    request_ = unregister_shared_memory_object_request;
}

const request::RegisterSharedMemoryObject& Request::GetSharedMemoryObject() const
{
    return score::cpp::get<request::RegisterSharedMemoryObject>(request_);
}

void Request::SetSharedMemoryObject(const request::RegisterSharedMemoryObject& register_shared_memory_object_request)
{
    request_ = register_shared_memory_object_request;
}

const request::RegisterClient& Request::GetRegisterClient() const
{
    return score::cpp::get<request::RegisterClient>(request_);
}

void Request::SetRegisterClient(const request::RegisterClient& register_client_request)
{
    request_ = register_client_request;
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
