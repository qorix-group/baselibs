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
#ifndef LIB_UTILS_SRC_PIMPL_PTR_H_
#define LIB_UTILS_SRC_PIMPL_PTR_H_

#include <cstddef>
#include <type_traits>
#include <utility>

namespace score
{
namespace utils
{

template <typename T, std::size_t Len, std::size_t Align>
class pimpl_ptr
{
  public:
    T* operator->() noexcept
    {
        return get();
    }

    T* get() noexcept
    {
        return m_ptr;
    }

    const T* operator->() const noexcept
    {
        return get();
    }

    const T* get() const noexcept
    {
        return m_ptr;
    }

    template <typename... Args>
    void construct(Args&&... args)
    {
        static_assert(sizeof(T) <= Len, "Buffer too small to allocate private class");
        static_assert(alignof(T) <= Align, "Wrong alignment for private class");
        // use of "placement new" is fullfiled according to "AUTOSAR C++14 A18-5-10" rule
        // Pointer storage is aligned to underlying type.
        // NOLINTNEXTLINE(score-no-dynamic-raw-memory) see above justification
        m_ptr = new (&m_storage) T(std::forward<Args>(args)...);
    }

    ~pimpl_ptr() noexcept
    {
        auto ptr = get();
        if (ptr != nullptr)
        {
            ptr->~T();
        }
    }

    pimpl_ptr() noexcept = default;

    pimpl_ptr(pimpl_ptr&& rhs) noexcept
    {
        move(rhs);
    }

    pimpl_ptr& operator=(pimpl_ptr&& rhs) noexcept
    {
        move(rhs);
        return *this;
    }

    pimpl_ptr(const pimpl_ptr&) = delete;
    pimpl_ptr& operator=(const pimpl_ptr&) = delete;

  private:
    void move(pimpl_ptr& rhs) noexcept
    {
        auto ptr = get();
        if (ptr != nullptr)
        {
            ptr->~T();
            this->m_ptr = nullptr;
        }
        this->m_storage = std::move(rhs.m_storage);
        std::swap(this->m_ptr, rhs.m_ptr);
    }

    std::aligned_storage_t<Len, Align> m_storage{};
    T* m_ptr = nullptr;
};

template <typename T>
using pimpl_ptr_16 = pimpl_ptr<T, 16, alignof(std::max_align_t)>;

template <typename T>
using pimpl_ptr_32 = pimpl_ptr<T, 32, alignof(std::max_align_t)>;

template <typename T>
using pimpl_ptr_64 = pimpl_ptr<T, 64, alignof(std::max_align_t)>;

template <typename T>
using pimpl_ptr_128 = pimpl_ptr<T, 128, alignof(std::max_align_t)>;

template <typename T>
using pimpl_ptr_256 = pimpl_ptr<T, 256, alignof(std::max_align_t)>;

template <typename T>
using pimpl_ptr_512 = pimpl_ptr<T, 512, alignof(std::max_align_t)>;

template <typename T>
using pimpl_ptr_1024 = pimpl_ptr<T, 1024, alignof(std::max_align_t)>;

template <typename T, std::size_t Len, std::size_t Align, typename... Args>
pimpl_ptr<T, Len, Align> make_pimpl(Args&&... args)
{
    pimpl_ptr<T, Len, Align> ptr;
    ptr.construct(std::forward<Args>(args)...);
    return ptr;
}

template <typename T, typename... Args>
auto make_pimpl_16(Args&&... args)
{
    return make_pimpl<T, 16, alignof(std::max_align_t)>(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
auto make_pimpl_32(Args&&... args)
{
    return make_pimpl<T, 32, alignof(std::max_align_t)>(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
auto make_pimpl_64(Args&&... args)
{
    return make_pimpl<T, 64, alignof(std::max_align_t)>(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
auto make_pimpl_128(Args&&... args)
{
    return make_pimpl<T, 128, alignof(std::max_align_t)>(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
auto make_pimpl_256(Args&&... args)
{
    return make_pimpl<T, 256, alignof(std::max_align_t)>(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
auto make_pimpl_512(Args&&... args)
{
    return make_pimpl<T, 512, alignof(std::max_align_t)>(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
auto make_pimpl_1024(Args&&... args)
{
    return make_pimpl<T, 1024, alignof(std::max_align_t)>(std::forward<Args>(args)...);
}

}  // namespace utils
}  // namespace score

#endif  // LIB_UTILS_SRC_PIMPL_PTR_H_
