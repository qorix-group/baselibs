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

#include <functional>
#include <memory>
#include <mutex>
#include <type_traits>

namespace score
{
namespace concurrency
{

/**
 * @file synchronized.h
 * @brief Small helper providing serialized access to an object.
 *
 * @section what What is Synchronized?
 * Synchronized<T, Mutex> wraps an object of type T together with a Mutex
 * (std::mutex by default) and provides RAII-based, mutex-protected access
 * to that object. All reads and writes are expected to go through lock()
 * or with_lock(), making the relationship between the object and its lock
 * explicit and harder to misuse.
 *
 * @section how How does it work?
 * - Internally stores:
 *     - mutable Mutex mut;
 *     - T obj;
 * - lock():
 *     - Acquires mut via std::unique_lock.
 *     - Returns a std::unique_ptr<T> (or const T) with a custom deleter
 *       that releases the lock when the pointer goes out of scope.
 * - with_lock(Func&& f):
 *     - Acquires the lock.
 *     - Invokes the callable f(obj) while the lock is held.
 *     - Automatically releases the lock when f returns (even on exceptions).
 *
 * @section how_to_use How to use it?
 * Basic example:
 * @code
 * using score::concurrency::Synchronized;
 *
 * Synchronized<int> counter(0);
 *
 * // Update
 * counter.with_lock([](int& v) {
 *     ++v;
 * });
 *
 * // Read
 * int value = counter.with_lock([](const int& v) {
 *     return v;
 * });
 * @endcode
 *
 * For a longer critical section:
 * @code
 * Synchronized<std::vector<int>> data;
 *
 * {
 *     auto locked = data.lock();    // mutex locked
 *     locked->push_back(42);
 * } // locked destroyed, mutex unlocked
 * @endcode
 *
 * @note
 * - The default Mutex is std::mutex, which is non-recursive.
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
template <typename T, typename Mutex = std::mutex>
class Synchronized
{
  public:
    template <typename... Args>
    explicit Synchronized(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : obj(std::forward<Args>(args)...)
    {
    }

    // Explicitly non-copyable and non-movable (by design)
    Synchronized(const Synchronized&) = delete;
    Synchronized(Synchronized&&) = delete;
    Synchronized& operator=(const Synchronized&) = delete;
    Synchronized& operator=(Synchronized&&) = delete;

    [[nodiscard]] auto lock()
    {
        auto unlocker = [ul = std::unique_lock(mut)](T*) mutable {};
        return std::unique_ptr<T, decltype(unlocker)>(&obj, std::move(unlocker));
    }

    [[nodiscard]] auto lock() const
    {
        auto unlocker = [ul = std::unique_lock(mut)](const T*) mutable {};
        return std::unique_ptr<const T, decltype(unlocker)>(&obj, std::move(unlocker));
    }

    template <typename Func>
    auto with_lock(Func&& f)
    {
        auto locked_ptr = lock();
        return std::invoke(std::forward<Func>(f), *locked_ptr);
    }

    template <typename Func>
    auto with_lock(Func&& f) const
    {
        auto locked_ptr = lock();
        return std::invoke(std::forward<Func>(f), *locked_ptr);
    }

  private:
    mutable Mutex mut;
    T obj;
};

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_SYNCHRONIZED_H
