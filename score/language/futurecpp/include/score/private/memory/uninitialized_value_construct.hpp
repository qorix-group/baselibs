///
/// \file
/// \copyright Copyright (C) 2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_UNINITIALIZED_VALUE_CONSTRUCT_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_UNINITIALIZED_VALUE_CONSTRUCT_HPP

#include <score/private/memory/construct_at.hpp>
#include <iterator>
#include <memory>

namespace score::cpp
{

/// \brief Backport of C++17 std::uninitialized_value_construct_n().
template <typename ForwardIter, typename Size>
ForwardIter uninitialized_value_construct_n(ForwardIter first, Size n)
{
    auto current = first;
    try
    {
        for (; n > 0; --n)
        {
            static_cast<void>(score::cpp::detail::construct_at(std::addressof(*current)));
            static_cast<void>(++current);
        }
        return current;
    }
    catch (...)
    {
        using value_type = typename std::iterator_traits<ForwardIter>::value_type;
        for (; first != current; ++first)
        {
            first->~value_type();
        }
        throw;
    }
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_UNINITIALIZED_VALUE_CONSTRUCT_HPP
