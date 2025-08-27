///
/// @file generic_trace_api.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API main file
///

#include "score/analysis/tracing/library/interface/generic_trace_api.h"
#include "score/analysis/tracing/library/generic_trace_api/generic_trace_api_impl.h"
#include "score/memory/shared/shared_memory_factory.h"

namespace score
{
namespace analysis
{
namespace tracing
{

ITraceLibrary* GenericTraceAPI::mock_{nullptr};

RegisterClientResult GenericTraceAPI::RegisterClient(const BindingType type, const std::string& app_instance_identifier)
{
    return GetInstance().RegisterClient(type, app_instance_identifier);
}
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
RegisterSharedMemoryObjectResult GenericTraceAPI::RegisterShmObject(const TraceClientId client,
                                                                    const std::string& shm_object_path) noexcept
{
    // Suppress "AUTOSAR C++14 A15-4-2" rule findings. This rule states: "Throwing an exception in a "noexcept"
    // function." In this case it is ok, because the system anyways forces the process to terminate if an exception is
    // thrown.
    // coverity[autosar_cpp14_a15_4_2_violation]
    return GetInstance().RegisterShmObject(client, shm_object_path);
}
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
RegisterSharedMemoryObjectResult GenericTraceAPI::RegisterShmObject(const TraceClientId client,
                                                                    const std::int32_t shm_object_fd) noexcept
{
    // Suppress "AUTOSAR C++14 A15-4-2" rule findings. This rule states: "Throwing an exception in a "noexcept"
    // function." In this case it is ok, because the system anyways forces the process to terminate if an exception is
    // thrown.
    // coverity[autosar_cpp14_a15_4_2_violation]
    return GetInstance().RegisterShmObject(client, shm_object_fd);
}

ResultBlank GenericTraceAPI::UnregisterShmObject(const TraceClientId client, const ShmObjectHandle handle)
{
    return GetInstance().UnregisterShmObject(client, handle);
}
// No issue here as this api is defined also in class geneirc_trace_api_imp
// coverity[autosar_cpp14_m3_9_1_violation]
RegisterTraceDoneCallBackResult GenericTraceAPI::RegisterTraceDoneCB(TraceClientId client,
                                                                     TraceDoneCallBackType trace_done_callback)
{
    return GetInstance().RegisterTraceDoneCB(client, std::move(trace_done_callback));
}

TraceResult GenericTraceAPI::Trace(const TraceClientId client,
                                   const MetaInfoVariants::type& meta_info,
                                   ShmDataChunkList& data,
                                   TraceContextId context_id)
{
    return GetInstance().Trace(client, meta_info, data, context_id);
}

// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
TraceResult GenericTraceAPI::Trace(const TraceClientId client,
                                   const MetaInfoVariants::type& meta_info,
                                   LocalDataChunkList& data) noexcept
{
    return GetInstance().Trace(client, meta_info, data);
}

ITraceLibrary& GenericTraceAPI::GetInstance()
{
    if (mock_ != nullptr)
    {
        return *mock_;
    }
    // Maintain temporary the order of the destruction of the static instances inside the shared memory library
    // TODO : Ticket-110455
    // No harm from ignorning the returned value here
    //   coverity[autosar_cpp14_a0_1_2_violation]
    score::memory::shared::MemoryResourceRegistry::getInstance();
    // No harm from ignorning the returned value here
    //   coverity[autosar_cpp14_a0_1_2_violation]
    score::memory::shared::SharedMemoryFactory::GetControlBlockSize();
    // No harm from defining static variables here
    //   coverity[autosar_cpp14_a3_3_2_violation]
    static GenericTraceAPIImpl instance_{};
    return instance_;
}

// Suppress "AUTOSAR C++14 A0-1-3" rule finding. This rule states: "Every function defined in an anonymous
// namespace, or static function with internal linkage, or private member function shall be used.".
// It's design choice as this method is used only for singleton testability.
// coverity[autosar_cpp14_a0_1_3_violation : FALSE]
void GenericTraceAPI::InjectMock(ITraceLibrary* mock) noexcept
{
    mock_ = mock;
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
