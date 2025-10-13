///
/// \file
/// \copyright Copyright (C) 2020-2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_SIMD_MATH_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_SIMD_MATH_HPP

#include <score/private/simd/simd_data_types.hpp>

// usable on QNX for ASIL B software. covered by requirement broken_link_c/issue/4049789
#define SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE __attribute__((__always_inline__))

namespace score::cpp
{

/// @brief Returns true if v is a NaN, false otherwise
///
/// [parallel] 9.7.7 ff
template <typename Abi>
inline simd_mask<float, Abi> SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE is_nan(const simd<float, Abi>& v) noexcept
{
    return simd_mask<float, Abi>{Abi::impl::is_nan(static_cast<typename Abi::impl::type>(v))};
}

/// @brief Returns true if v is a NaN, false otherwise
///
/// [parallel] 9.7.7 ff
template <typename Abi>
inline simd_mask<double, Abi> SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE is_nan(const simd<double, Abi>& v) noexcept
{
    return simd_mask<double, Abi>{Abi::impl::is_nan(static_cast<typename Abi::impl::type>(v))};
}

} // namespace score::cpp

#undef SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_SIMD_MATH_HPP
