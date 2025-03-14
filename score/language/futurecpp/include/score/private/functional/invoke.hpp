///
/// \file
/// \copyright Copyright (C) 2016-2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_FUNCTIONAL_INVOKE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_FUNCTIONAL_INVOKE_HPP

#include <functional>
#include <type_traits>
#include <utility>

namespace score::cpp
{
namespace detail
{

/// \brief C++14 version of std::invoke
/// Only available if the callable is a member function pointer
///
/// \tparam Callable The callable type to invoke
/// \tparam Args Argument types for callable f
/// \param f The callable that shall be invoked
/// \param args Arguments to invoke callable f
/// \return Return value of callable f
template <typename Callable,
          typename... Args,
          std::enable_if_t<std::is_member_pointer<std::decay_t<Callable>>{}, int> = 0>
constexpr auto invoke(Callable&& f, Args&&... args) noexcept(noexcept(std::mem_fn(f)(std::forward<Args>(args)...)))
    -> decltype(auto)
{
    return std::mem_fn(f)(std::forward<Args>(args)...);
}

/// \brief C++14 version of std::invoke
/// Only available if the callable is not a member function pointer
///
/// \tparam Callable The callable type to invoke
/// \tparam Args Argument types for callable f
/// \param f The callable that shall be invoked
/// \param args Arguments to invoke callable f
/// \return Return value of callable f
template <typename Callable,
          typename... Args,
          std::enable_if_t<!std::is_member_pointer<std::decay_t<Callable>>{}, int> = 0>
constexpr auto invoke(Callable&& f,
                      Args&&... args) noexcept(noexcept(std::forward<Callable>(f)(std::forward<Args>(args)...)))
    -> decltype(auto)
{
    return std::forward<Callable>(f)(std::forward<Args>(args)...);
}

} // namespace detail
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_FUNCTIONAL_INVOKE_HPP
