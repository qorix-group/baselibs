#include "score/analysis/tracing/library/interface/ara_com_properties.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace score::analysis::tracing;

TEST(ara_com_properties, MatchEqualityOperator)
{
    TracePointType tracepoint = TracePointType::SKEL_METHOD_CALL_RESULT_OK;
    ServiceInstanceElement service;
    service.service_id_ = 1;
    service.major_version_ = 2;
    service.minor_version_ = 3;
    service.instance_id_ = 4;
    service.element_id_ = 5u;

    AraComProperties first_proprities(tracepoint, service, 1);

    TracePointType tracepoint1 = TracePointType::SKEL_METHOD_CALL_RESULT_OK;
    ServiceInstanceElement service1;
    service1.service_id_ = 1;
    service1.major_version_ = 2;
    service1.minor_version_ = 3;
    service1.instance_id_ = 4;
    service1.element_id_ = 5u;
    AraComProperties second_proprities(tracepoint1, service1, 1);
    ASSERT_TRUE(first_proprities == second_proprities);
}

TEST(ara_com_properties, MismatchEqualityOperatorTracepoint)
{
    TracePointType tracepoint = TracePointType::SKEL_METHOD_CALL_RESULT_ERROR;
    ServiceInstanceElement service;
    service.service_id_ = 1;
    service.major_version_ = 2;
    service.minor_version_ = 3;
    service.instance_id_ = 4;
    service.element_id_ = 5u;

    AraComProperties first_proprities(tracepoint, service, 1);

    TracePointType tracepoint1 = TracePointType::SKEL_METHOD_CALL_RESULT_OK;
    ServiceInstanceElement service1;
    service1.service_id_ = 1;
    service1.major_version_ = 2;
    service1.minor_version_ = 3;
    service1.instance_id_ = 4;
    service1.element_id_ = 5u;
    AraComProperties second_proprities(tracepoint1, service1, 1);
    ASSERT_FALSE(first_proprities == second_proprities);
}

TEST(ara_com_properties, MismatchEqualityOperatorServiceId)
{
    TracePointType tracepoint = TracePointType::SKEL_METHOD_CALL_RESULT_OK;
    ServiceInstanceElement service;
    service.service_id_ = 123;
    service.major_version_ = 2;
    service.minor_version_ = 3;
    service.instance_id_ = 4;
    service.element_id_ = 5u;

    AraComProperties first_proprities(tracepoint, service, 1);

    TracePointType tracepoint1 = TracePointType::SKEL_METHOD_CALL_RESULT_OK;
    ServiceInstanceElement service1;
    service1.service_id_ = 1;
    service1.major_version_ = 2;
    service1.minor_version_ = 3;
    service1.instance_id_ = 4;
    service1.element_id_ = 5u;
    AraComProperties second_proprities(tracepoint1, service1, 1);
    ASSERT_FALSE(first_proprities == second_proprities);
}

TEST(ara_com_properties, MismatchEqualityOperatorTracepointDataId)
{
    TracePointType tracepoint = TracePointType::SKEL_METHOD_CALL_RESULT_OK;
    ServiceInstanceElement service;
    service.service_id_ = 1;
    service.major_version_ = 2;
    service.minor_version_ = 3;
    service.instance_id_ = 4;
    service.element_id_ = 5u;

    AraComProperties first_proprities(tracepoint, service, 1);

    TracePointType tracepoint1 = TracePointType::SKEL_METHOD_CALL_RESULT_OK;
    ServiceInstanceElement service1;
    service1.service_id_ = 1;
    service1.major_version_ = 2;
    service1.minor_version_ = 3;
    service1.instance_id_ = 4;
    service1.element_id_ = 5u;
    AraComProperties second_proprities(tracepoint1, service1, 2);
    ASSERT_FALSE(first_proprities == second_proprities);
}
