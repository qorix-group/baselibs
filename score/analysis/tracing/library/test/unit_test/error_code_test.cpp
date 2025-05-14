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
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

using namespace score::analysis::tracing;

TEST(ERROR_CODE, MakeErrorMatchMessageAndUserMessage)
{
    auto error = MakeError(ErrorCode::kNoErrorRecoverable, "Error message");
    ASSERT_TRUE(error.UserMessage().compare("Error message") == 0);
    ASSERT_TRUE(error.Message().compare("No error occured") == 0);
}

TEST(ERROR_CODE, MessageForMatchWithErrorCode)
{
    RecordProperty("Verifies", "SCR-39688772");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "Test verifies if GTL returns a meaningful error message if the Trace API fails");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    GenericTraceAPIErrorDomain generic_trace_api_error_domain;

    ASSERT_TRUE(
        generic_trace_api_error_domain.MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kNoErrorRecoverable))
            .compare("No error occured") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kDaemonNotConnectedFatal))
                    .compare("Daemon was unexpectedly disconnected") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kInvalidAppInstanceIdFatal))
                    .compare("Invalid app instance ID") == 0);
    ASSERT_TRUE(
        generic_trace_api_error_domain.MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kInvalidArgumentFatal))
            .compare("Invalid argument") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kInvalidBindingTypeFatal))
                    .compare("Invalid binding type") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kNotEnoughMemoryRecoverable))
                    .compare("Not enough memory for allocation") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kNoMetaInfoProvidedRecoverable))
                    .compare("No meta info were provided") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kNotEnoughMemoryInContainerRecoverable))
                    .compare("Not enough memory in trace job container") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kAtomicRingBufferFullRecoverable))
                    .compare("Atomic ring buffer full") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kAtomicRingBufferEmptyRecoverable))
                    .compare("Atomic ring buffer empty") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kAtomicRingBufferMaxRetriesRecoverable))
                    .compare("Atomic ring buffer access failed max retry times") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kModuleNotInitializedRecoverable))
                    .compare("Module not initialized") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kModuleInitializedRecoverable))
                    .compare("Module already initialized") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kDaemonNotConnectedRecoverable))
                    .compare("Daemon is not yet available") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferFullRecoverable))
                    .compare("Ring buffer full") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferEmptyRecoverable))
                    .compare("Ring buffer empty") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferNoEmptyElementRecoverable))
                    .compare("No empty element in the ring buffer could be acquired") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferNoReadyElementRecoverable))
                    .compare("No ready element in the ring buffer could be acquired") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferNotInitializedRecoverable))
                    .compare("Ring buffer not initialized") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferInitializedRecoverable))
                    .compare("Ring buffer already initialized") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferInvalidStateRecoverable))
                    .compare("Invalid state of ring buffer") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferTooLargeRecoverable))
                    .compare("Not enough space to allocate ring buffer with desired size") == 0);
    ASSERT_TRUE(
        generic_trace_api_error_domain
            .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferInvalidMemoryResourceRecoverable))
            .compare("Invalid memory resource passed to constructor") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kDaemonConnectionFailedFatal))
                    .compare("Daemon connection failed") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kDaemonCommunicatorNotSupportedFatal))
                    .compare("Daemon communication is supported only with QNX") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kServerConnectionNameOpenFailedFatal))
                    .compare("Server name open failed") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kInvalidShmObjectHandleFatal))
                    .compare("Invalid SHM object handle") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kNoSpaceLeftForAllocationRecoverable))
                    .compare("No space to allocate in TMD shared memory") == 0);
    ASSERT_TRUE(
        generic_trace_api_error_domain
            .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kIndexOutOfBoundsInSharedListRecoverable))
            .compare("Index is out of bounds") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kMessageSendFailedRecoverable))
                    .compare("Failed to send the message") == 0);
    ASSERT_TRUE(
        generic_trace_api_error_domain
            .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kSharedMemoryObjectRegistrationFailedFatal))
            .compare("Failed to register a shared-memory object") == 0);
    ASSERT_TRUE(
        generic_trace_api_error_domain
            .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kSharedMemoryObjectAlreadyRegisteredRecoverable))
            .compare("Shared-memory object is already registered with this file descriptor/path") == 0);
    ASSERT_TRUE(
        generic_trace_api_error_domain
            .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kSharedMemoryObjectNotInTypedMemoryFatal))
            .compare("Request to register non typed-memory") == 0);
    ASSERT_TRUE(
        generic_trace_api_error_domain
            .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kSharedMemoryObjectUnregisterFailedFatal))
            .compare("Failed to unregister a shared-memory object") == 0);
    ASSERT_TRUE(
        generic_trace_api_error_domain
            .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kSharedMemoryObjectHandleCreationFailedFatal))
            .compare("Failed to create a shared-memory object handle") == 0);
    ASSERT_TRUE(
        generic_trace_api_error_domain
            .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kSharedMemoryObjectFlagsRetrievalFailedFatal))
            .compare("Failed to retrieve the shared memory object's flags") == 0);
    ASSERT_TRUE(
        generic_trace_api_error_domain
            .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kSharedMemoryObjectHandleDeletionFailedFatal))
            .compare("Failed to delete a shared-memory object handle") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kWrongMessageIdRecoverable))
                    .compare("Wrong message id") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kWrongClientIdRecoverable))
                    .compare("Wrong client id") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kClientNameAlreadyUsedRecoverable))
                    .compare("Client name is already used by different process") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kBadFileDescriptorFatal))
                    .compare("Bad file descriptor") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kChannelCreationFailedFatal))
                    .compare("Channel creation failed") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kNameAttachFailedFatal))
                    .compare("Name attach failed") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kNameDetachFailedFatal))
                    .compare("Name detach failed") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kDispatchDestroyFailedRecoverable))
                    .compare("Dispatch destroy failed") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kWrongHandleRecoverable))
                    .compare("Wrong memory handle of local trace job") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kCallbackAlreadyRegisteredRecoverable))
                    .compare("Callback already registered for the specific client id") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kNoFreeSlotToSaveTheCallbackRecoverable))
                    .compare("No free slot found to save the callback") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kClientNotFoundRecoverable))
                    .compare("Client ID not found") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kNoDeallocatorCallbackRegisteredFatal))
                    .compare("No deallocator callback registered") == 0);
    ASSERT_TRUE(
        generic_trace_api_error_domain.MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kTerminalFatal))
            .compare("Terminal Fatal") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kDaemonNotAvailableFatal))
                    .compare("LTPM Daemon not available") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kFailedRegisterCachedClientsFatal))
                    .compare("Failed to register the cached client registration requests") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kFailedRegisterCachedShmObjectsFatal))
                    .compare("Failed to register the cached SHM object registration requests") == 0);
    ASSERT_TRUE(
        generic_trace_api_error_domain
            .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kTraceJobAllocatorInitializationFailedFatal))
            .compare("TraceJobAllocator initialization failed") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kGenericErrorRecoverable))
                    .compare("Unknown generic error") == 0);
    ASSERT_TRUE(
        generic_trace_api_error_domain.MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kLastRecoverable))
            .compare("Unknown generic error") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kNoMoreSpaceForNewClientFatal))
                    .compare("No More Space for new client, Max Number of Clients allocated") == 0);
    ASSERT_TRUE(generic_trace_api_error_domain
                    .MessageFor(static_cast<score::result::ErrorCode>(ErrorCode::kNoMoreSpaceForNewShmObjectFatal))
                    .compare("No More Space for new SHM object, Max Number of SHM objects allocated") == 0);
}

/// @brief class for error code helper function parameterized test
class ErrorCodeTest : public ::testing::TestWithParam<std::tuple<ErrorCode, bool>>
{
};

// test cases for parameterized ErrorCodeTest
std::vector<std::tuple<ErrorCode, bool>> test_cases{
    // No error occured (Recoverable)
    std::make_tuple(ErrorCode::kNoErrorRecoverable, true),
    // Daemon is not connected currently (Fatal)
    std::make_tuple(ErrorCode::kDaemonNotConnectedFatal, false),
    // Invalid argument (Fatal)
    std::make_tuple(ErrorCode::kInvalidArgumentFatal, false),
    // Not enough memory for allocation (Recoverable)
    std::make_tuple(ErrorCode::kNotEnoughMemoryRecoverable, true),
    // No Meta info were provided (Recoverable)
    std::make_tuple(ErrorCode::kNoMetaInfoProvidedRecoverable, true),
    // Not enough memory for trace job allocation (Recoverable)
    std::make_tuple(ErrorCode::kNotEnoughMemoryInContainerRecoverable, true),
    ///< Atomic ring buffer full (Recoverable)
    std::make_tuple(ErrorCode::kAtomicRingBufferFullRecoverable, true),
    ///< Atomic ring buffer empty (Recoverable)
    std::make_tuple(ErrorCode::kAtomicRingBufferEmptyRecoverable, true),
    ///< Atomic ring buffer access failed max retry times (Recoverable)
    std::make_tuple(ErrorCode::kAtomicRingBufferMaxRetriesRecoverable, true),
    // Module not initialized (Recoverable)
    std::make_tuple(ErrorCode::kModuleNotInitializedRecoverable, true),
    // Module already initialized (Recoverable)
    std::make_tuple(ErrorCode::kModuleInitializedRecoverable, true),
    // Daemon is not yet available (Recoverable)
    std::make_tuple(ErrorCode::kDaemonNotConnectedRecoverable, true),
    // Ring buffer full (Recoverable)
    std::make_tuple(ErrorCode::kRingBufferFullRecoverable, true),
    // Ring buffer empty (Recoverable)
    std::make_tuple(ErrorCode::kRingBufferEmptyRecoverable, true),
    // No empty element in the ring buffer was found (recoverable)
    std::make_tuple(ErrorCode::kRingBufferNoEmptyElementRecoverable, true),
    // No ready element in the ring buffer was found (recoverable)
    std::make_tuple(ErrorCode::kRingBufferNoReadyElementRecoverable, true),
    // Ring buffer not initialized (Recoverable)
    std::make_tuple(ErrorCode::kRingBufferNotInitializedRecoverable, true),
    // Ring buffer already initialized (Recoverable)
    std::make_tuple(ErrorCode::kRingBufferInitializedRecoverable, true),
    // Invalid buffer state (Recoverable)
    std::make_tuple(ErrorCode::kRingBufferInvalidStateRecoverable, true),
    // Buffer size too large (Recoverable)
    std::make_tuple(ErrorCode::kRingBufferTooLargeRecoverable, true),
    // Buffer size too small (Recoverable)
    std::make_tuple(ErrorCode::kRingBufferInvalidMemoryResourceRecoverable, true),
    // Daemon connection failed (Fatal)
    std::make_tuple(ErrorCode::kDaemonConnectionFailedFatal, false),
    // Daemon connection is not supported (Fatal)
    std::make_tuple(ErrorCode::kDaemonCommunicatorNotSupportedFatal, false),
    // QNX name open failed (Fatal)
    std::make_tuple(ErrorCode::kServerConnectionNameOpenFailedFatal, false),
    // Callback already registered the client id (Recoverable)
    std::make_tuple(ErrorCode::kCallbackAlreadyRegisteredRecoverable, true),
    // No free slot is found to save the callback (Recoverable)
    std::make_tuple(ErrorCode::kNoFreeSlotToSaveTheCallbackRecoverable, true),
    // No deallocator callback registered (Fatal)
    std::make_tuple(ErrorCode::kNoDeallocatorCallbackRegisteredFatal, false),
    // Message sending failed (Recoverable)
    std::make_tuple(ErrorCode::kMessageSendFailedRecoverable, true),
    // Shared-memory object registration failed (Fatal)
    std::make_tuple(ErrorCode::kSharedMemoryObjectRegistrationFailedFatal, false),
    // Shared-memory object is already registered (Recoverable)
    std::make_tuple(ErrorCode::kSharedMemoryObjectAlreadyRegisteredRecoverable, true),
    // Tried to register non-typed memory. (Fatal)
    std::make_tuple(ErrorCode::kSharedMemoryObjectNotInTypedMemoryFatal, false),
    // Shared-memory object unregister failed (Fatal)
    std::make_tuple(ErrorCode::kSharedMemoryObjectUnregisterFailedFatal, false),
    // Shared-memory object creation failed (Fatal)
    std::make_tuple(ErrorCode::kSharedMemoryObjectHandleCreationFailedFatal, false),
    // Shared-memory object flags retrieval failed (Fatal)
    std::make_tuple(ErrorCode::kSharedMemoryObjectFlagsRetrievalFailedFatal, false),
    // Shared-memory object deletion failed (Fatal)
    std::make_tuple(ErrorCode::kSharedMemoryObjectHandleDeletionFailedFatal, false),
    // Wrong message id in request (Recoverable)
    std::make_tuple(ErrorCode::kWrongMessageIdRecoverable, true),
    // Wrong client id in response (Recoverable)
    std::make_tuple(ErrorCode::kWrongClientIdRecoverable, true),
    // Client name is already used by different process (Recoverable)
    std::make_tuple(ErrorCode::kClientNameAlreadyUsedRecoverable, true),
    // Bad file discriptor (Fatal)
    std::make_tuple(ErrorCode::kBadFileDescriptorFatal, false),
    // QNX channel creation failed (Fatal)
    std::make_tuple(ErrorCode::kChannelCreationFailedFatal, false),
    // QNX name_attach failed (Fatal)
    std::make_tuple(ErrorCode::kNameAttachFailedFatal, false),
    // QNX name_detach failed (Fatal)
    std::make_tuple(ErrorCode::kNameDetachFailedFatal, false),
    // QNX dispatch_destroy failed (Recoverable)
    std::make_tuple(ErrorCode::kDispatchDestroyFailedRecoverable, true),
    // Wrong handle of local trace job (Recoverable)
    std::make_tuple(ErrorCode::kWrongHandleRecoverable, true),
    // to be used only for overflow check (Recoverable)
    std::make_tuple(ErrorCode::kLastRecoverable, true),
    // Generic error (Recoverable)
    std::make_tuple(ErrorCode::kGenericErrorRecoverable, true),
    // Invalid app instance ID (Fatal)
    std::make_tuple(ErrorCode::kInvalidAppInstanceIdFatal, false),
    // Invalid binding type (Fatal)
    std::make_tuple(ErrorCode::kInvalidBindingTypeFatal, false),
    // Client not registered (Recoverable)
    std::make_tuple(ErrorCode::kClientNotFoundRecoverable, true),
    // Invalid SHM Object handle (Fatal)
    std::make_tuple(ErrorCode::kInvalidShmObjectHandleFatal, false),
    // No more space to allocate in TMD (Recoverable)
    std::make_tuple(ErrorCode::kNoSpaceLeftForAllocationRecoverable, true),
    // Element not found to deallocate (Recoverable)
    std::make_tuple(ErrorCode::kIndexOutOfBoundsInSharedListRecoverable, true),
    // No More Space for new client to register (Fatal)
    std::make_tuple(ErrorCode::kNoMoreSpaceForNewClientFatal, false),
    // No More Space for new client to register (Fatal)
    std::make_tuple(ErrorCode::kNoMoreSpaceForNewShmObjectFatal, false),
    // LTPM Daemon not available (Fatal)
    std::make_tuple(ErrorCode::kDaemonNotAvailableFatal, false),
    // Failed to register the cached client registration requests (Fatal)
    std::make_tuple(ErrorCode::kFailedRegisterCachedClientsFatal, false),
    // Failed to register the cached SHM object registration requests (Fatal)
    std::make_tuple(ErrorCode::kFailedRegisterCachedShmObjectsFatal, false),
    // TraceJobAllocator initialization failed (Fatal)
    std::make_tuple(ErrorCode::kTraceJobAllocatorInitializationFailedFatal, false),
    // Terminal (Fatal)
    std::make_tuple(ErrorCode::kTerminalFatal, false)};

INSTANTIATE_TEST_SUITE_P(ErrorCodeTest, ErrorCodeTest, ::testing::ValuesIn(test_cases));

/// @brief test function IsErrorRecoverable
///
/// test case: all error code defined in ErrorCode enum
/// @output: output expected value for all test cases
TEST_P(ErrorCodeTest, IsErrorRecoverable)
{
    bool expected = std::get<1>(GetParam());
    ErrorCode code = std::get<0>(GetParam());

    ASSERT_EQ(expected, IsErrorRecoverable(code));
}

/// @brief test if all error codes are in the test cases above
///
/// @output: success if the number of test cases equals to the number of error codes
TEST_F(ErrorCodeTest, NumOfEnumTest)
{
    ASSERT_EQ(test_cases.size(), static_cast<int32_t>(ErrorCode::kLastRecoverable) + 1);
}
