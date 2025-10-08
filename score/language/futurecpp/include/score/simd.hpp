///
/// \file
/// \copyright Copyright (C) 2020-2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.Simd component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_SIMD_HPP
#define SCORE_LANGUAGE_FUTURECPP_SIMD_HPP

#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
#include <score/private/simd/simd_sse42_backend.hpp>
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
#include <score/private/simd/simd_aarch64_neon_backend.hpp>
#else
#include <score/private/simd/simd_default_backend.hpp>
#endif
#include <score/private/simd/simd_data_types.hpp> // IWYU pragma: export
#include <score/private/simd/simd_math.hpp>       // IWYU pragma: export

#endif // SCORE_LANGUAGE_FUTURECPP_SIMD_HPP
