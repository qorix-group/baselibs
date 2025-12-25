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
#ifndef SCORE_LIB_CONCURRENCY_UNLOCK_GUARD_H
#define SCORE_LIB_CONCURRENCY_UNLOCK_GUARD_H

// #include "score/concurrency/scoped_operation.h"
// #include <functional>

namespace score::concurrency
{

template <typename Lockable>
class UnlockGuard
{
  public:
    explicit UnlockGuard(Lockable& lockable)
        : lockable_{lockable}  //, lock_on_destruction{std::bind(std::mem_fn(&Lockable::lock), lockable_)}
    {
        lockable_.unlock();
    }
    ~UnlockGuard()
    {
        lockable_.lock();
    }
    UnlockGuard(const UnlockGuard&) = delete;
    UnlockGuard(UnlockGuard&&) = delete;
    UnlockGuard& operator=(const UnlockGuard&) = delete;
    UnlockGuard& operator=(UnlockGuard&&) = delete;

  private:
    Lockable& lockable_;
    // scoped_operation<std::function<void()>> lock_on_destruction;
};

}  // namespace score::concurrency

#endif  // SCORE_LIB_CONCURRENCY_UNLOCK_GUARD_H
