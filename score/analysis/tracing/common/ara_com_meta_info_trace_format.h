///
/// @file ara_com_meta_info_trace_format.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#ifndef GENERIC_TRACE_API_ARA_COM_META_INFO_TRACE_FORMAT_H
#define GENERIC_TRACE_API_ARA_COM_META_INFO_TRACE_FORMAT_H

#include "interface_types/types.h"
#include "score/analysis/tracing/library/interface/ara_com_meta_info.h"
#include <cstdint>

namespace score
{
namespace analysis
{
namespace tracing
{

using ServiceId = std::uint32_t;
using InstanceId = std::uint32_t;
using ElementId = std::uint32_t;
using TracePointDataId = std::uint32_t;
using IpcBindingType = std::uint8_t;
using TraceStatus = std::uint8_t;
using Version = std::uint32_t;

class __attribute__((packed)) AraComMetaInfoTraceFormat
{
  public:
    // No harm to define it as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    IpcBindingType ipc_flavor_;
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    TraceStatus trace_status_;
    // No harm to define it as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    AppIdType app_identifier_{};
    // No harm to define it as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    TracePointType trace_point_type_;
    // No harm to define it as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    ServiceId service_id_;
    // No harm to define it as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    Version major_version_;
    // No harm to define it as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    Version minor_version_;
    // No harm to define it as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    InstanceId instance_id_;
    // No harm to define it as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    ElementId element_id_;
    // No harm to define it as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    TracePointDataId trace_point_data_id_;  ///< Set to 0 if value is not present

    AraComMetaInfoTraceFormat(AraComMetaInfo ara_com_info, BindingType binding_type, AppIdType app_identifier);
};

bool operator==(const AraComMetaInfoTraceFormat& lhs, const AraComMetaInfoTraceFormat& rhs) noexcept;

static_assert(sizeof(AraComMetaInfoTraceFormat) == 35,
              "Wrong size of AraComMetaInfoTraceFormat, should be 35 bytes, no padding");
static_assert(std::is_standard_layout<AraComMetaInfoTraceFormat>::value,
              "AraComMetaInfoTraceFormat must have standard layout");

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_ARA_COM_META_INFO_TRACE_FORMAT_H
