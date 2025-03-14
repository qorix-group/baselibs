///
/// \file
/// \copyright Copyright (C) 2021, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.StopToken component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_STOP_TOKEN_NOSTOPSTATE_T_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_STOP_TOKEN_NOSTOPSTATE_T_HPP

namespace score::cpp
{

/// \brief Unit type intended for use as a placeholder in score::cpp::stop_source
/// non-default constructor, that makes the constructed score::cpp::stop_source empty with no associated stop-state.
struct nostopstate_t
{
    explicit nostopstate_t() = default;
};

/// \brief This is a constant object instance of score::cpp::nostopstate_t
/// for use in constructing an empty score::cpp::stop_source, as a placeholder value in the non-default constructor.
extern const nostopstate_t nostopstate;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_STOP_TOKEN_NOSTOPSTATE_T_HPP
