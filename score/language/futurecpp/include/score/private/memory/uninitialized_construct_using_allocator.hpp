///
/// \file
/// \copyright Copyright (C) 2020-2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_UNINITIALIZED_CONSTRUCT_USING_ALLOCATOR_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_UNINITIALIZED_CONSTRUCT_USING_ALLOCATOR_HPP

#include <utility>

#include <score/private/memory/construct_at.hpp>
#include <score/private/memory/uses_allocator_construction_args.hpp>
#include <score/apply.hpp>

namespace score::cpp
{
namespace detail
{

/// \brief Creates a `T` object by means of uses-allocator construction at given address `p`.
template <typename T, typename Alloc, typename... Args>
constexpr T* uninitialized_construct_using_allocator(T* p, const Alloc& alloc, Args&&... args)
{
    return score::cpp::apply([p](auto&&... xs) { return score::cpp::detail::construct_at(p, std::forward<decltype(xs)>(xs)...); },
                      score::cpp::detail::uses_allocator_construction_args<T>(alloc, std::forward<Args>(args)...));
}

} // namespace detail
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_UNINITIALIZED_CONSTRUCT_USING_ALLOCATOR_HPP
