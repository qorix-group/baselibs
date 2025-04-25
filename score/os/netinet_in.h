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
#ifndef BASELIBS_SCORE_OS_NETINET_IN_H
#define BASELIBS_SCORE_OS_NETINET_IN_H

#include <netinet/in.h>

namespace score
{
namespace os
{

using InAddrT = in_addr_t;
using InAddr = in_addr;

}  // namespace os
}  // namespace score

#endif  // BASELIBS_SCORE_OS_NETINET_IN_H
