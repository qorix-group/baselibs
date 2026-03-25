/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
#ifndef SCORE_MW_LOG_RECORDER_CONFIG_H
#define SCORE_MW_LOG_RECORDER_CONFIG_H

namespace score
{
namespace mw
{
namespace log
{

/// \brief Feature constants for safety-critical backend exclusion.
///
/// When KCONSOLE_LOGGING is not defined, the console backend is not registered.
/// When KFILE_LOGGING is not defined, the file backend is not registered.
/// When KREMOTE_LOGGING is not defined, the remote backend is not registered.
/// When KCUSTOM_LOGGING is not defined (default), the custom backend is not registered.
/// Unregistered backends leave their slot empty in the backend table;
/// callers handle the fallback centrally (e.g. via EmptyRecorder).

/*
Deviation from Rule A16-0-1:
- Rule A16-0-1 (required, implementation, automated)
The pre-processor shall only be used for unconditional and conditional file
inclusion and include guards.
Justification:
- Feature flags to enable/disable logging backends on production images.
*/
// coverity[autosar_cpp14_a16_0_1_violation] see above
#ifdef KCONSOLE_LOGGING
static constexpr bool kConsoleLoggingEnabled{true};
#else
static constexpr bool kConsoleLoggingEnabled{false};
// coverity[autosar_cpp14_a16_0_1_violation] see above
#endif

// coverity[autosar_cpp14_a16_0_1_violation] see above
#ifdef KFILE_LOGGING
static constexpr bool kFileLoggingEnabled{true};
#else
static constexpr bool kFileLoggingEnabled{false};
// coverity[autosar_cpp14_a16_0_1_violation] see above
#endif

// coverity[autosar_cpp14_a16_0_1_violation] see above
#ifdef KREMOTE_LOGGING
static constexpr bool kRemoteLoggingEnabled{true};
#else
static constexpr bool kRemoteLoggingEnabled{false};
// coverity[autosar_cpp14_a16_0_1_violation] see above
#endif

// coverity[autosar_cpp14_a16_0_1_violation] see above
#ifdef KCUSTOM_LOGGING
static constexpr bool kCustomLoggingEnabled{true};
#else
static constexpr bool kCustomLoggingEnabled{false};
// coverity[autosar_cpp14_a16_0_1_violation] see above
#endif

}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_RECORDER_CONFIG_H
