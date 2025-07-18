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
#ifndef GENERIC_TRACE_API_SERVICE_INSTANCE_ELEMENT_H
#define GENERIC_TRACE_API_SERVICE_INSTANCE_ELEMENT_H

#include <score/utility.hpp>
#include <score/variant.hpp>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief ServiceInstanceElement class
///
/// Class used to identify service instance element
class ServiceInstanceElement
{
  public:
    using ServiceIdType = std::uint32_t;   ///< Type used to store Service Id
    using InstanceIdType = std::uint32_t;  ///< Type used to store Instance Id
    using EventIdType = std::uint32_t;     ///< Type used to store Event Id
    using FieldIdType = std::uint32_t;     ///< Type used to store Field Id
    using MethodIdType = std::uint32_t;    ///< Type used to store Method Id
    using VariantType = score::cpp::variant<EventIdType, FieldIdType, MethodIdType>;
    // No harm to declare the members as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    ServiceIdType service_id_;
    // No harm to declare the members as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    std::uint32_t major_version_;
    // No harm to declare the members as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    std::uint32_t minor_version_;
    // No harm to declare the members as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    InstanceIdType instance_id_;
    // No harm to declare the members as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    VariantType element_id_;

    // No harm from defining the == operator as member function
    // coverity[autosar_cpp14_a13_5_5_violation]
    bool operator==(const ServiceInstanceElement& other) const
    {
        return ((((service_id_ == other.service_id_) && (major_version_ == other.major_version_)) &&
                 ((minor_version_ == other.minor_version_) && (instance_id_ == other.instance_id_))) &&
                (element_id_ == other.element_id_));
    }
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_SERVICE_INSTANCE_ELEMENT_H
