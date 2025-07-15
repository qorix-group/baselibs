///
/// @file daemon_communication_response.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API daemon communication response header file
///

#ifndef GENERIC_TRACE_API_DAEMON_COMMUNICATION_RESPONSE_H
#define GENERIC_TRACE_API_DAEMON_COMMUNICATION_RESPONSE_H

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
namespace response
{
struct RegisterSharedMemoryObject
{
    /// @brief Index to link the trace job with the shared memory pointer
    ShmObjectHandle shared_memory_object_index;
};

struct UnregisterSharedMemoryObject
{  // No harm from using boolean
    // coverity[autosar_cpp14_a9_6_1_violation]
    bool is_unregister_successful;
};

struct DaemonProcessId
{
    // No harm from using this naming convention as pid_t is a POSIX-compliant type
    //  coverity[autosar_cpp14_a9_6_1_violation]
    pid_t daemon_process_id;
};

struct RegisterClient
{
    score::analysis::tracing::TraceClientId trace_client_id;
};

struct ErrorCode
{
    score::result::ErrorCode error_code;
};
}  // namespace response

enum class VariantType : std::uint8_t
{
    RegisterSharedMemoryObject = 0,
    UnregisterSharedMemoryObject,
    DaemonProcessId,
    RegisterClient,
    ErrorCode
};

class Response
{
  public:
    const response::UnregisterSharedMemoryObject& GetUnregisterSharedMemoryObject() const;
    void SetUnregisterSharedMemoryObject(
        const response::UnregisterSharedMemoryObject& unregister_shared_memory_object_response);

    const response::RegisterSharedMemoryObject& GetRegisterSharedMemoryObject() const;
    void SetRegisterSharedMemoryObject(
        const response::RegisterSharedMemoryObject& register_shared_memory_object_response);

    void SetDaemonProcessId(const response::DaemonProcessId& daemon_process_id_request);
    const response::DaemonProcessId& GetDaemonProcessId() const;

    void SetTraceClientId(const response::RegisterClient& trace_client_id_response);
    const response::RegisterClient& GetTraceClientId() const;

    void SetErrorCode(const response::ErrorCode& error_code);
    const response::ErrorCode& GetErrorCode() const;

    VariantType GetVariantType() const;

  private:
    score::cpp::variant<response::RegisterSharedMemoryObject,
                 response::UnregisterSharedMemoryObject,
                 response::DaemonProcessId,
                 response::RegisterClient,
                 response::ErrorCode>
        response_{};
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_DAEMON_COMMUNICATION_RESPONSE_H
