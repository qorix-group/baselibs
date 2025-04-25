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
///
/// @file fs_crypto.cpp
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief fs_crypto functions OSAL class
/// [QNX fs_crypto
/// documentation](https://www.qnx.com/developers/docs/7.1/index.html#com.qnx.doc.neutrino.utilities/topic/f/fsencrypt.html)
///

#include "score/os/qnx/fs_crypto_impl.h"

std::unique_ptr<score::os::qnx::FsCrypto> score::os::qnx::FsCrypto::createFsCryptoInstance() noexcept
{
    return std::make_unique<score::os::qnx::FsCryptoImpl>();
}
