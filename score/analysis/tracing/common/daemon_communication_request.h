///
/// @file daemon_communication_request.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API daemon communication request header file
///

#ifndef GENERIC_TRACE_API_DAEMON_COMMUNICATION_REQUEST_H
#define GENERIC_TRACE_API_DAEMON_COMMUNICATION_REQUEST_H

#include "interface_types/types.h"
#include "score/analysis/tracing/common/daemon_communication_messages.h"
#include <score/variant.hpp>
#include <sys/mman.h>

namespace score
{
namespace analysis
{
namespace tracing
{
namespace request
{

struct RegisterSharedMemoryObject
{
    /// @brief Result of shm_create_handle
    shm_handle_t shared_memory_handle;
};

struct UnregisterSharedMemoryObject
{
    /// @brief Index to link the trace job with the shared memory pointer
    ShmObjectHandle shared_memory_object_index;
};

struct RegisterClient
{
    // No harm here to define the variable here as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    BindingType binding_type;
    // No harm here to define the variable here as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    AppIdType app_instance_identifier;
};
}  // namespace request

class Request
{
  public:
    void SetMessageId(DaemonCommunicatorMessage message_id);
    const DaemonCommunicatorMessage& GetMessageId() const;

    const request::UnregisterSharedMemoryObject& GetUnregisterSharedMemoryObject() const;
    void SetUnregisterSharedMemoryObject(
        const request::UnregisterSharedMemoryObject& unregister_shared_memory_object_request);

    const request::RegisterSharedMemoryObject& GetSharedMemoryObject() const;
    void SetSharedMemoryObject(const request::RegisterSharedMemoryObject& register_shared_memory_object_request);

    const request::RegisterClient& GetRegisterClient() const;
    void SetRegisterClient(const request::RegisterClient& register_client_request);

  private:
    DaemonCommunicatorMessage message_id_{DaemonCommunicatorMessage::kUndefined};
    score::cpp::variant<request::RegisterSharedMemoryObject, request::UnregisterSharedMemoryObject, request::RegisterClient>
        request_{};
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_DAEMON_COMMUNICATION_REQUEST_H
