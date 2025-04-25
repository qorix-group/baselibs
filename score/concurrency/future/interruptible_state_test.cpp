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
#include "score/concurrency/future/interruptible_future.h"
#include "score/concurrency/future/interruptible_promise.h"

#include "score/stop_token.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <future>

namespace score
{
namespace concurrency
{
namespace detail
{
namespace
{

TEST(InterruptibleStateTest, Destruction)
{
    /*
     * BaseInterruptibleState inherits from std::enable_shared_from_this. Thus, it should never (!) be allocated on the
     * stack or via a unique_ptr on the heap!
     *
     * Because GCC emits symbols for all constructors and destructors defined in the Itanium C++ ABI independent of
     * their usage, we need to test these to achieve the required function coverage (100%). Hence, for this test, we
     * ignore above requirement and construct on stack, and on heap with unique pointers.
     *
     * Specifically, we use unique pointers because constructing a shared pointer using std::make_shared will not (!)
     * call the deleting destructor (D0) of BaseInterruptibleState. Instead, it will call the deleting destructor of
     * std::shared_ptr and that will in turn call the complete object destructor (D1) of BaseInterruptibleState.
     */

    // Required to cover D0 destructor
    auto heap_base_state = std::make_unique<InterruptibleState<void>>();
    heap_base_state.reset();

    // Required to cover D1 destructors
    {
        InterruptibleState<void> stack_base_state{};
        (void)stack_base_state;
    }

    // Required to cover D2 destructor
    std::shared_ptr<InterruptibleState<void>> shared_heap_base_state;
    {
        shared_heap_base_state = std::make_shared<InterruptibleState<void>>();
    }
    shared_heap_base_state.reset();
}

}  // namespace
}  // namespace detail
}  // namespace concurrency
}  // namespace score
