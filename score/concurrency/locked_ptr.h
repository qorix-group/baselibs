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
#ifndef SCORE_LIB_CONCURRENCY_LOCKED_PTR_H
#define SCORE_LIB_CONCURRENCY_LOCKED_PTR_H

#include "score/concurrency/unlock_guard.h"
#include "platform/aas/lib/type_traits/type_traits.h"

#include <utility>

namespace score
{
namespace concurrency
{

template <typename T,
          typename Lock,
          typename = std::enable_if_t<is_basic_lockable_v<Lock> && !std::is_reference_v<Lock>>>
class LockedPtr
{
    T* ptr_;
    Lock lock_;

  public:
    LockedPtr(T* ptr, Lock lock) : ptr_(ptr), lock_(std::move(lock)) {}
    ~LockedPtr() = default;

    LockedPtr(const LockedPtr&) = delete;
    LockedPtr& operator=(const LockedPtr&) = delete;

    LockedPtr(LockedPtr&& other) noexcept : ptr_(other.ptr_), lock_(std::move(other.lock_))
    {
        other.ptr_ = nullptr;
    }

    LockedPtr& operator=(LockedPtr&& other) noexcept
    {
        LockedPtr{std::move(other)}.swap(*this);
        return *this;
    }

    void swap(LockedPtr& other) noexcept
    {
        std::swap(ptr_, other.ptr_);
        std::swap(lock_, other.lock_);
    }

    T& operator*()
    {
        return *ptr_;
    }
    T* operator->()
    {
        return ptr_;
    }

    [[nodiscard]] T* get() const noexcept
    {
        return ptr_;
    }

    [[nodiscard]] UnlockGuard<Lock> unlock_guard()
    {
        return UnlockGuard<Lock>{lock_};
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return ptr_ != nullptr;
    }

    [[nodiscard]] bool operator==(const T* p) const noexcept
    {
        return ptr_ == p;
    }

    [[nodiscard]] bool operator==(std::nullptr_t) const noexcept
    {
        return ptr_ == nullptr;
    }
};

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_LOCKED_PTR_H
