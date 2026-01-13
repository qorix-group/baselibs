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
#ifndef SCORE_LIB_CONCURRENCY_SYNCHRONIZED_H
#define SCORE_LIB_CONCURRENCY_SYNCHRONIZED_H

#include "score/concurrency/locked_ptr.h"
#include "platform/aas/lib/type_traits/type_traits.h"

#include <functional>
#include <memory>
#include <mutex>
#include <tuple>
#include <type_traits>

namespace score::concurrency
{

/**
 * @file synchronized.h
 * @brief Small helper providing serialized access to an object.
 *
 * @section what What is Synchronized?
 * Synchronized<T, Lockable> wraps an object of type T together with a Lockable
 * (std::mutex by default) and provides RAII-based, mutex-protected access
 * to that object. All reads and writes are expected to go through lock()
 * or with_lock(), making the relationship between the object and its lock
 * explicit and harder to misuse.
 *
 * @section member_types Member Types
 * - value_type: The type T of the synchronized object.
 * - pointer: LockedPtr<T, std::unique_lock<Lockable>>
 * - const_pointer: LockedPtr<const T, std::unique_lock<Lockable>>
 *
 * @section how How does it work?
 * - Internally stores:
 *     - mutable Lockable mut;
 *     - T obj;
 * - lock():
 *     - Acquires mut via std::unique_lock.
 *     - Returns a pointer (or a const_pointer for const Synchronized)
 *       with the lock held which is released when the returned pointer goes out of scope.
 * - with_lock(Func&& f):
 *     - Acquires the lock.
 *     - Invokes the callable f(pointer) or f(const_pointer) with the lock held.
 *     - The lock is released when f returns (or exits due to an exception).
 *
 * @section usage Usage Example
 * Basic example:
 * @code
 * using score::concurrency::Synchronized;
 *
 * Synchronized<Sender> sender(local_endpoint, remote_endpoint);
 *
 * // Modify
 * bool success = sender.with_lock([message](pointer sp) {
 *     return sp->send(message);
 * });
 *
 * // Read
 * auto total_messages = sender.with_lock([](const_pointer sp) {
 *     return sp->stats.num_successful_messages + sp->stats.num_failed_messages;
 * });
 * @endcode
 *
 * For a longer critical section:
 * @code
 * Synchronized<std::vector<dtc_status>> dtc_statuses;
 *
 * if (auto new_dtcs = receive_dtcs(); !new_dtcs.empty()) {
 *     auto dtcs = dtc_statuses.lock();    // mutex locked
 *     dtcs->reserve(dtcs->size() + new_dtcs.size());
 *     dtcs->insert(dtcs->end(), new_dtcs.begin(), new_dtcs.end());
 * } // dtcs goes out of scope, mutex unlocked
 * @endcode
 *
 * @note
 * - The default Lockable is std::mutex, which is non-recursive.
 *   Locking the same mutex multiple times from the same thread has
 *   undefined behaviour
 *   (see https://en.cppreference.com/w/cpp/thread/mutex/lock).
 * - Therefore, calling lock() / with_lock() re-entrantly on the same
 *   Synchronized instance is not supported and must be avoided.
 *
 * The intended usage is simple, non-reentrant critical sections:
 * @code
 * Synchronized<Foo> f;
 * f.with_lock([](Foo& x) { x.do_something(); });
 * @endcode
 */
template <typename T,
          typename Lockable = std::mutex,
          // std::is_default_constructible_v<Lockable> applies to Mutex Named Requirement
          // but not for non-standard non-default constructible Lockable
          typename = std::enable_if_t<  // std::is_default_constructible_v<Lockable> &&
              std::is_destructible_v<Lockable> && is_basic_lockable_v<Lockable>>>
class Synchronized
{
  public:
    using value_type = T;
    using pointer = LockedPtr<T, std::unique_lock<Lockable>>;
    using const_pointer = LockedPtr<const T, std::unique_lock<Lockable>>;

    template <typename... Args>
    explicit Synchronized(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...> &&
                                                   std::is_nothrow_constructible_v<Lockable>)
        : obj(std::forward<Args>(args)...)
    {
    }

    template <typename U>
    explicit Synchronized(std::initializer_list<U> ilist) noexcept(
        std::is_nothrow_constructible_v<T, std::initializer_list<U>> && std::is_nothrow_constructible_v<Lockable>)
        : obj(ilist)
    {
    }

    /**
     * @brief Piecewise constructor for Synchronized.
     * Allows constructing T and Lockable with separate argument lists.
     *
     * @param t_args Arguments for constructing T
     * @param lockable_args Arguments for constructing Lockable
     */
    template <typename... TArgs,
              typename... LockableArgs,
              typename = std::enable_if_t<std::is_constructible_v<T, TArgs...>>,
              typename = std::enable_if_t<std::is_constructible_v<Lockable, LockableArgs...>>>
    explicit Synchronized(
        std::piecewise_construct_t,
        std::tuple<TArgs...> t_args,
        std::tuple<LockableArgs...> lockable_args) noexcept(std::is_nothrow_constructible_v<T, TArgs...> &&
                                                            std::is_nothrow_constructible_v<Lockable, LockableArgs...>)
        : mut(std::make_from_tuple<Lockable>(lockable_args)), obj(std::make_from_tuple<T>(t_args))
    {
    }

    // Explicitly non-copyable and non-movable (by design)
    Synchronized(const Synchronized&) = delete;
    Synchronized(Synchronized&&) = delete;
    Synchronized& operator=(const Synchronized&) = delete;
    Synchronized& operator=(Synchronized&&) = delete;

    [[nodiscard]] pointer lock()
    {
        return pointer{&obj, std::unique_lock{mut}};
    }

    [[nodiscard]] const_pointer lock() const
    {
        return const_pointer{&obj, std::unique_lock{mut}};
    }

    template <typename Func, typename = std::enable_if_t<std::is_invocable_v<Func, pointer>>>
    auto with_lock(Func&& f) noexcept(std::is_nothrow_invocable_v<Func, pointer>)
    {
        return std::invoke(std::forward<Func>(f), lock());
    }

    template <typename Func, typename = std::enable_if_t<std::is_invocable_v<Func, const_pointer>>>
    auto with_lock(Func&& f) const noexcept(std::is_nothrow_invocable_v<Func, const_pointer>)
    {
        return std::invoke(std::forward<Func>(f), lock());
    }

  private:
    mutable Lockable mut;
    T obj;
};

}  // namespace score::concurrency

#endif  // SCORE_LIB_CONCURRENCY_SYNCHRONIZED_H
