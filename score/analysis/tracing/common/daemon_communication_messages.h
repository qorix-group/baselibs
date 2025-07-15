///
/// @file daemon_communication_messages.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API communication message IDs header
///

#ifndef GENERIC_TRACE_API_DAEMON_COMMUNICATION_MESSAGES_H
#define GENERIC_TRACE_API_DAEMON_COMMUNICATION_MESSAGES_H

#include <score/variant.hpp>
#include <cstdint>

namespace score
{
namespace analysis
{
namespace tracing
{

constexpr static auto GetServerAddressName()
{
    return "daemon_communication";
}

enum class DaemonCommunicatorMessage : std::uint8_t
{
    kSharedMemoryObjectRegistrationRequest = 0x00,
    kSharedMemoryObjectUnregisterRequest = 0x01,
    kDaemonProcessIdRequest = 0x02,
    kRegisterClientRequest = 0x03,
    kLibraryProcessTerminationRequest = 0x04,

    kUndefined = 0xFE,
    kLast = 0xFF
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_DAEMON_COMMUNICATION_MESSAGES_H
