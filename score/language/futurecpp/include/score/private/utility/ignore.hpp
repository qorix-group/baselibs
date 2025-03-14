///
/// \file
/// \copyright Copyright (C) 2017-2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_IGNORE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_IGNORE_HPP

namespace score::cpp
{
namespace detail
{

struct swallow
{
    template <typename T>
    // NOLINTNEXTLINE(misc-unconventional-assign-operator) not allow chaining so return value is `const swallow&`
    constexpr const swallow& operator=(T&&) const
    {
        return *this;
    }
};

} // namespace detail

///
/// \brief Swallows the value assigned to it.
///
/// Shall be used to explicitly state that a return value shall be ignored. The value assigned to ignore is ignored and
/// side-effect free.
///
/// Note that the implementation of std::ignore is "unspecified". So who knows what operations are done for the
/// assignment in std::ignore and even than it can change with newer C++ versions.
///
constexpr score::cpp::detail::swallow ignore{};

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_IGNORE_HPP
