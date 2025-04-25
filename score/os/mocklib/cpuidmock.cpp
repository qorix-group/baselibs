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
#include "score/os/mocklib/cpuidmock.h"

#include <array>
#include <string>

namespace score
{
namespace os
{

namespace
{
constexpr std::uint32_t leafValue = 0x40000000U;
std::uint32_t eaxValue = 0x0U;
}  // namespace

void CpuIdMock::SetExpextedCallIsQemu()
{
    // Following hex values represents string "KVMKVMKVM", which is used to assert if we are running on qemu.
    const std::array<uint32_t, 3> sig{0x4B4D564BU, 0x564B4D56U, 0x4DU};
    using namespace ::testing;
    // WillRepeatedly is set because we assume that for one test case, we can call is_qemu more than once.
    EXPECT_CALL(*this, cpuid(leafValue, eaxValue, _, _, _))
        .Times(AtLeast(1))
        .WillRepeatedly(DoAll(SetArgReferee<2>(sig.at(0)), SetArgReferee<3>(sig.at(1)), SetArgReferee<4>(sig.at(2))));
}

void CpuIdMock::SetExpextedCallIsHw()
{
    const std::array<uint32_t, 3> sig{0U, 0U, 0U};
    using namespace ::testing;
    // WillRepeatedly is set because we assume that for one test case, we can call is_qemu more than once.
    EXPECT_CALL(*this, cpuid(leafValue, eaxValue, _, _, _))
        .Times(AtLeast(1))
        .WillRepeatedly(DoAll(SetArgReferee<2>(sig.at(0)), SetArgReferee<3>(sig.at(1)), SetArgReferee<4>(sig.at(2))));
}

}  // namespace os
}  // namespace score
