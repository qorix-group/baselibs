///
/// @file library_state.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#ifndef GENERIC_TRACE_API_LIBRARY_STATE_H
#define GENERIC_TRACE_API_LIBRARY_STATE_H

#include <cstdint>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief Current state of Generic Trace API
enum class LibraryState : std::uint8_t
{
    kNotInitialized = 0x00U,     ///< Library is not initialized, can't perform normal operation
    kInitialized = 0x01U,        ///< Library is initialized, can perform all the operations specified in the API
    kDaemonInitialized = 0x02u,  ///< Library is not initialized, can register clients, (de)register SHM objects
    kDaemonDisconnected =
        0x03u,  ///< Library is not functional anymore as the daemon is disconnected after successful connection
    kGenericError = 0xFFU  ///< Library is in undefined state, some error occurred
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_LIBRARY_STATE_H
