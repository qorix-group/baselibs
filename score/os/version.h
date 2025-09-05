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
#ifndef SCORE_LIB_OS_VERSION_H
#define SCORE_LIB_OS_VERSION_H

// Helper macros for version comparisons

#define SPP_OS_VERSION_GTE(version, major, minor) ((version) >= ((major * 100) + minor))
#define SPP_OS_VERSION_LT(version, major, minor) ((version) < ((major * 100) + minor))

// SPP OS-specific macros

#if defined(__QNX__)
#include "score/os/qnx/qnx_version.h"

#define SPP_OS_QNX_VERSION _SPP_QNX_VERSION_

#if SPP_OS_VERSION_GTE(SPP_OS_QNX_VERSION, 8, 0) && SPP_OS_VERSION_LT(SPP_OS_QNX_VERSION, 9, 0)
#define SPP_OS_QNX8
#endif
#endif  // __QNX__

#endif  // SCORE_LIB_OS_VERSION_H
