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
#include "score/result/result.h"

// These identifiers are shared by both C++ and Rust and allow for runtime checking
// that both sides were linked against the same C++ stdlib.
// These values were chosen to resist possible bitflips or wrong casting.
// On Rust side, these constants are defined at: score/result/rust/result.rs
#define LIBSTDCPP_IDENTIFIER 0x0123456789ABCDEF
#define LIBCXX_IDENTIFIER 0xFEDCBA9876543210

extern "C" void LibResultErrorDomainGetMessageForErrorCode(const score::result::ErrorDomain& domain,
                                                           score::result::ErrorCode code,
                                                           std::string_view& result) noexcept
{
    result = domain.MessageFor(code);
}

extern "C" std::uint64_t get_runtime_identifier() noexcept
{
#if defined(__GLIBCXX__)
    // Using GNU libstdc++
    (void)LIBCXX_IDENTIFIER;  // Suppress unused warning
    return LIBSTDCPP_IDENTIFIER;
#elif defined(_LIBCPP_VERSION)
    // Using LLVM libc++
    (void)LIBSTDCPP_IDENTIFIER;  // Suppress unused warning
    return LIBCXX_IDENTIFIER;
#else
#error "Unsupported C++ standard library"
#endif
}
