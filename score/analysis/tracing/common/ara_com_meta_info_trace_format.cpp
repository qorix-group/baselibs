///
/// @file ara_com_meta_info_trace_format.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#include "score/analysis/tracing/common/ara_com_meta_info_trace_format.h"
#include <netinet/in.h>
#include <cstring>
#include <utility>

namespace score
{
namespace analysis
{
namespace tracing
{

// No harm from using strncpy (m3-2-4)and Aware of using static_cast and it is safe(m3-2-2)
// coverity[autosar_cpp14_m3_2_4_violation]
// coverity[autosar_cpp14_m3_2_2_violation]
AraComMetaInfoTraceFormat::AraComMetaInfoTraceFormat(AraComMetaInfo ara_com_info,
                                                     BindingType binding_type,
                                                     AppIdType app_identifier)
    : ipc_flavor_{static_cast<std::underlying_type_t<BindingType>>(binding_type)},
      trace_status_{static_cast<std::uint8_t>(ara_com_info.trace_status_.to_ulong())},
      app_identifier_{app_identifier},
      trace_point_type_{static_cast<std::uint8_t>(ara_com_info.properties_.trace_point_id_.first)},
      // No harm from using c-style functions here
      //  coverity[autosar_cpp14_a5_2_2_violation]
      service_id_{htonl(ara_com_info.properties_.trace_point_id_.second.service_id_)},
      // No harm from using c-style functions here
      //  coverity[autosar_cpp14_a5_2_2_violation]
      major_version_{htonl(ara_com_info.properties_.trace_point_id_.second.major_version_)},
      // No harm from using c-style functions here
      //  coverity[autosar_cpp14_a5_2_2_violation]
      minor_version_{htonl(ara_com_info.properties_.trace_point_id_.second.minor_version_)},
      // No harm from using c-style functions here
      //  coverity[autosar_cpp14_a5_2_2_violation]
      instance_id_{htonl(ara_com_info.properties_.trace_point_id_.second.instance_id_)},
      element_id_{},
      // No harm from using c-style functions here
      //  coverity[autosar_cpp14_a5_2_2_violation]
      trace_point_data_id_{htonl(ara_com_info.properties_.trace_point_data_id_.value_or(0u))}
{
    // The value that would be stored would always be the first one which is EventIdType
    // No harm from using c-style functions here
    //  coverity[autosar_cpp14_a5_2_2_violation]
    element_id_ = htonl(
        score::cpp::get<ServiceInstanceElement::EventIdType>(ara_com_info.properties_.trace_point_id_.second.element_id_));
}

// clang-format off
bool operator==(const AraComMetaInfoTraceFormat& lhs, const AraComMetaInfoTraceFormat& rhs) noexcept
{
    // No harm as there are already parenthesis around the logical operators
    // coverity[autosar_cpp14_a5_2_6_violation]
    return (((lhs.ipc_flavor_ == rhs.ipc_flavor_) &&
             (lhs.trace_status_ == rhs.trace_status_) &&
             ((std::strncmp(lhs.app_identifier_.data(), rhs.app_identifier_.data(), sizeof(lhs.app_identifier_))) == 0) &&
             (lhs.trace_point_type_ == rhs.trace_point_type_) &&
             (lhs.service_id_ == rhs.service_id_) &&
             (lhs.major_version_ == rhs.major_version_) &&
             (lhs.minor_version_ == rhs.minor_version_) &&
             (lhs.instance_id_ == rhs.instance_id_) &&
             (lhs.element_id_ == rhs.element_id_) &&
             (lhs.trace_point_data_id_ == rhs.trace_point_data_id_)));

}
// clang-format on
}  // namespace tracing
}  // namespace analysis
}  // namespace score
