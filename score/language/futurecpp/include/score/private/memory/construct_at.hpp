///
/// \file
/// \copyright Copyright (C) 2020-2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_CONSTRUCT_AT_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_CONSTRUCT_AT_HPP

#include <memory>
#include <utility>

namespace score::cpp
{
namespace detail
{

/// \brief Creates a `T` object initialized with arguments `args...` at given address `p`.
///
/// Implements https://isocpp.org/files/papers/N4860.pdf#subsection.25.11.7
/// Non-conforming: Not constexpr and not constrained.
template <typename T, typename... Args>
T* construct_at(T* const p, Args&&... args)
{
    return ::new (const_cast<void*>(static_cast<const volatile void*>(p))) T(std::forward<Args>(args)...);
}

} // namespace detail
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_CONSTRUCT_AT_HPP
