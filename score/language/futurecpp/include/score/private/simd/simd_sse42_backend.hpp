///
/// \file
/// \copyright Copyright (C) 2020-2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_SIMD_SSE42_BACKEND_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_SIMD_SSE42_BACKEND_HPP

#include <cstddef>
#include <cstdint>
#include <utility>
// usable on QNX for ASIL B software. covered by requirement broken_link_c/issue/4036540
#include <nmmintrin.h> // only include SSE4.2.

// usable on QNX for ASIL B software. covered by requirement broken_link_c/issue/4049789
#define SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE __attribute__((__always_inline__))

namespace score::cpp
{
namespace simd_abi
{
namespace detail
{

template <typename T>
struct sse42_mask_backend;

template <>
struct sse42_mask_backend<std::int32_t>
{
    using type = __m128i;

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE broadcast(const bool v) noexcept
    {
        return _mm_set1_epi32(static_cast<std::int32_t>(-static_cast<std::uint32_t>(v)));
    }

    template <typename G>
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE init(G&& gen, const std::index_sequence<0U, 1U, 2U, 3U>) noexcept
    {
        return _mm_set_epi32(
            static_cast<std::int32_t>(-static_cast<std::uint32_t>(gen(std::integral_constant<std::size_t, 3U>{}))),
            static_cast<std::int32_t>(-static_cast<std::uint32_t>(gen(std::integral_constant<std::size_t, 2U>{}))),
            static_cast<std::int32_t>(-static_cast<std::uint32_t>(gen(std::integral_constant<std::size_t, 1U>{}))),
            static_cast<std::int32_t>(-static_cast<std::uint32_t>(gen(std::integral_constant<std::size_t, 0U>{}))));
    }

    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE extract(const type v, const std::size_t i) noexcept
    {
        return (_mm_movemask_epi8(v) & (1 << (4U * i))) != 0;
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE logical_not(const type v) noexcept
    {
        return _mm_cmpeq_epi32(v, _mm_setzero_si128());
    }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE logical_and(const type a, const type b) noexcept { return _mm_and_si128(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE logical_or(const type a, const type b) noexcept { return _mm_or_si128(a, b); }

    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE all_of(const type v) noexcept { return _mm_movemask_epi8(v) == 0xFFFF; }
    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE any_of(const type v) noexcept { return _mm_movemask_epi8(v) != 0; }
    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE none_of(const type v) noexcept { return _mm_movemask_epi8(v) == 0; }
};

template <>
struct sse42_mask_backend<float>
{
    using type = __m128;

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE broadcast(const bool v) noexcept
    {
        return _mm_castsi128_ps(_mm_set1_epi32(static_cast<std::int32_t>(-static_cast<std::uint32_t>(v))));
    }

    template <typename G>
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE init(G&& gen, const std::index_sequence<0U, 1U, 2U, 3U>) noexcept
    {
        return _mm_castsi128_ps(_mm_set_epi32(
            static_cast<std::int32_t>(-static_cast<std::uint32_t>(gen(std::integral_constant<std::size_t, 3U>{}))),
            static_cast<std::int32_t>(-static_cast<std::uint32_t>(gen(std::integral_constant<std::size_t, 2U>{}))),
            static_cast<std::int32_t>(-static_cast<std::uint32_t>(gen(std::integral_constant<std::size_t, 1U>{}))),
            static_cast<std::int32_t>(-static_cast<std::uint32_t>(gen(std::integral_constant<std::size_t, 0U>{})))));
    }

    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE extract(const type v, const std::size_t i) noexcept
    {
        return (_mm_movemask_ps(v) & (1 << i)) != 0;
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE logical_not(const type v) noexcept { return _mm_cmpeq_ps(v, _mm_setzero_ps()); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE logical_and(const type a, const type b) noexcept { return _mm_and_ps(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE logical_or(const type a, const type b) noexcept { return _mm_or_ps(a, b); }

    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE all_of(const type v) noexcept { return _mm_movemask_ps(v) == 0b1111; }
    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE any_of(const type v) noexcept { return _mm_movemask_ps(v) != 0; }
    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE none_of(const type v) noexcept { return _mm_movemask_ps(v) == 0; }
};

template <>
struct sse42_mask_backend<double>
{
    using type = __m128d;

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE broadcast(const bool v) noexcept
    {
        return _mm_castsi128_pd(_mm_set1_epi64x(static_cast<std::int64_t>(-static_cast<std::uint64_t>(v))));
    }

    template <typename G>
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE init(G&& gen, const std::index_sequence<0U, 1U>) noexcept
    {
        return _mm_castsi128_pd(_mm_set_epi64x(
            static_cast<std::int64_t>(-static_cast<std::uint64_t>(gen(std::integral_constant<std::size_t, 1U>{}))),
            static_cast<std::int64_t>(-static_cast<std::uint64_t>(gen(std::integral_constant<std::size_t, 0U>{})))));
    }

    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE extract(const type v, const std::size_t i) noexcept
    {
        return (_mm_movemask_pd(v) & (1 << i)) != 0;
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE logical_not(const type v) noexcept { return _mm_cmpeq_pd(v, _mm_setzero_pd()); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE logical_and(const type a, const type b) noexcept { return _mm_and_pd(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE logical_or(const type a, const type b) noexcept { return _mm_or_pd(a, b); }

    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE all_of(const type v) noexcept { return _mm_movemask_pd(v) == 0b11; }
    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE any_of(const type v) noexcept { return _mm_movemask_pd(v) != 0; }
    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE none_of(const type v) noexcept { return _mm_movemask_pd(v) == 0; }
};

template <typename To, typename From>
struct converter;
template <>
struct converter<std::int32_t, float>
{
    static __m128i SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE run(const __m128 v) noexcept { return _mm_cvttps_epi32(v); }
};
template <>
struct converter<float, std::int32_t>
{
    static __m128 SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE run(const __m128i v) noexcept { return _mm_cvtepi32_ps(v); }
};

template <typename T>
struct sse42_backend;

template <>
struct sse42_backend<std::int32_t>
{
    using type = __m128i;
    static constexpr std::size_t width{4};

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE broadcast(const std::int32_t v) noexcept { return _mm_set1_epi32(v); }

    template <typename G>
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE init(G&& gen, const std::index_sequence<0U, 1U, 2U, 3U>) noexcept
    {
        return _mm_set_epi32(gen(std::integral_constant<std::size_t, 3U>{}),
                             gen(std::integral_constant<std::size_t, 2U>{}),
                             gen(std::integral_constant<std::size_t, 1U>{}),
                             gen(std::integral_constant<std::size_t, 0U>{}));
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE load(const std::int32_t* const v) noexcept
    {
        return _mm_loadu_si128(reinterpret_cast<const __m128i*>(v));
    }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE load_aligned(const std::int32_t* const v) noexcept
    {
        return _mm_load_si128(reinterpret_cast<const __m128i*>(v));
    }
    static void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE store(std::int32_t* const v, const type a) noexcept
    {
        _mm_storeu_si128(reinterpret_cast<__m128i*>(v), a);
    }
    static void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE store_aligned(std::int32_t* const v, const type a) noexcept
    {
        _mm_store_si128(reinterpret_cast<__m128i*>(v), a);
    }

    static std::int32_t SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE extract(const type v, const std::size_t i) noexcept
    {
        alignas(16) std::int32_t tmp[width];
        _mm_store_si128(reinterpret_cast<__m128i*>(tmp), v);
        return tmp[i];
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE add(const type a, const type b) noexcept { return _mm_add_epi32(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE subtract(const type a, const type b) noexcept { return _mm_sub_epi32(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE multiply(const type a, const type b) noexcept { return _mm_mullo_epi32(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE divide(const type a, const type b) noexcept
    {
        __m128i r0{};
        r0 = _mm_insert_epi32(r0, _mm_extract_epi32(a, 0) / _mm_extract_epi32(b, 0), 0);
        r0 = _mm_insert_epi32(r0, _mm_extract_epi32(a, 1) / _mm_extract_epi32(b, 1), 1);

        __m128i r1{};
        r1 = _mm_insert_epi32(r1, _mm_extract_epi32(a, 2) / _mm_extract_epi32(b, 2), 0);
        r1 = _mm_insert_epi32(r1, _mm_extract_epi32(a, 3) / _mm_extract_epi32(b, 3), 1);

        return _mm_unpacklo_epi64(r0, r1);
    }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE negate(const type v) noexcept { return _mm_sub_epi32(_mm_setzero_si128(), v); }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE equal(const type a, const type b) noexcept { return _mm_cmpeq_epi32(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE not_equal(const type a, const type b) noexcept
    {
        return _mm_cmpeq_epi32(_mm_cmpeq_epi32(a, b), _mm_setzero_si128());
    }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE less_than(const type a, const type b) noexcept { return _mm_cmplt_epi32(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE less_equal(const type a, const type b) noexcept
    {
        return _mm_cmpeq_epi32(_mm_cmpgt_epi32(a, b), _mm_setzero_si128());
    }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE greater_than(const type a, const type b) noexcept
    {
        return _mm_cmpgt_epi32(a, b);
    }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE greater_equal(const type a, const type b) noexcept
    {
        return _mm_cmpeq_epi32(_mm_cmplt_epi32(a, b), _mm_setzero_si128());
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE min(const type a, const type b) noexcept { return _mm_min_epi32(b, a); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE max(const type a, const type b) noexcept { return _mm_max_epi32(b, a); }

    template <typename To>
    static __m128 SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE convert(const type v) noexcept
    {
        static_assert(sizeof(To) == sizeof(std::int32_t), "Mismatch in number of elements");
        return converter<To, std::int32_t>::run(v);
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE blend(const type a, const type b, const type c) noexcept
    {
        return _mm_blendv_epi8(a, b, c);
    }
};

template <>
struct sse42_backend<float>
{
    using type = __m128;
    static constexpr std::size_t width{4};

    static __m128 SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE broadcast(const float v) noexcept { return _mm_set1_ps(v); }

    template <typename G>
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE init(G&& gen, const std::index_sequence<0U, 1U, 2U, 3U>) noexcept
    {
        return _mm_set_ps(gen(std::integral_constant<std::size_t, 3U>{}),
                          gen(std::integral_constant<std::size_t, 2U>{}),
                          gen(std::integral_constant<std::size_t, 1U>{}),
                          gen(std::integral_constant<std::size_t, 0U>{}));
    }

    static __m128 SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE load(const float* const v) noexcept { return _mm_loadu_ps(v); }
    static __m128 SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE load_aligned(const float* const v) noexcept { return _mm_load_ps(v); }
    static void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE store(float* const v, const __m128 a) noexcept { _mm_storeu_ps(v, a); }
    static void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE store_aligned(float* const v, const __m128 a) noexcept { _mm_store_ps(v, a); }

    static float SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE extract(const __m128 v, const std::size_t i) noexcept
    {
        alignas(16) float tmp[width];
        _mm_store_ps(&tmp[0], v);
        return tmp[i];
    }

    static __m128 SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE add(const __m128 a, const __m128 b) noexcept { return _mm_add_ps(a, b); }
    static __m128 SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE subtract(const __m128 a, const __m128 b) noexcept { return _mm_sub_ps(a, b); }
    static __m128 SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE multiply(const __m128 a, const __m128 b) noexcept { return _mm_mul_ps(a, b); }
    static __m128 SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE divide(const __m128 a, const __m128 b) noexcept { return _mm_div_ps(a, b); }
    static __m128 SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE negate(const __m128 v) noexcept { return _mm_xor_ps(v, _mm_set1_ps(-0.0F)); }

    static __m128 SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE equal(const __m128 a, const __m128 b) noexcept { return _mm_cmpeq_ps(a, b); }
    static __m128 SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE not_equal(const __m128 a, const __m128 b) noexcept
    {
        return _mm_cmpneq_ps(a, b);
    }
    static __m128 SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE less_than(const __m128 a, const __m128 b) noexcept
    {
        return _mm_cmplt_ps(a, b);
    }
    static __m128 SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE less_equal(const __m128 a, const __m128 b) noexcept
    {
        return _mm_cmple_ps(a, b);
    }
    static __m128 SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE greater_than(const __m128 a, const __m128 b) noexcept
    {
        return _mm_cmpgt_ps(a, b);
    }
    static __m128 SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE greater_equal(const __m128 a, const __m128 b) noexcept
    {
        return _mm_cmpge_ps(a, b);
    }

    static __m128 SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE min(const __m128 a, const __m128 b) noexcept { return _mm_min_ps(b, a); }
    static __m128 SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE max(const __m128 a, const __m128 b) noexcept { return _mm_max_ps(b, a); }

    template <typename To>
    static __m128i SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE convert(const __m128 v) noexcept
    {
        static_assert(sizeof(To) == sizeof(float), "Mismatch in number of elements");
        return converter<To, float>::run(v);
    }

    static __m128 SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE is_nan(const __m128 v) noexcept { return _mm_cmpunord_ps(v, v); }

    static __m128 SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE blend(const __m128 a, const __m128 b, const __m128 c) noexcept
    {
        return _mm_blendv_ps(a, b, c);
    }
};

template <>
struct sse42_backend<double>
{
    using type = __m128d;
    static constexpr std::size_t width{2};

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE broadcast(const double v) noexcept { return _mm_set1_pd(v); }

    template <typename G>
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE init(G&& gen, const std::index_sequence<0U, 1U>) noexcept
    {
        return _mm_set_pd(gen(std::integral_constant<std::size_t, 1U>{}),
                          gen(std::integral_constant<std::size_t, 0U>{}));
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE load(const double* const v) noexcept { return _mm_loadu_pd(v); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE load_aligned(const double* const v) noexcept { return _mm_load_pd(v); }
    static void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE store(double* const v, const type a) noexcept { _mm_storeu_pd(v, a); }
    static void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE store_aligned(double* const v, const type a) noexcept { _mm_store_pd(v, a); }

    static double SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE extract(const type v, const std::size_t i) noexcept
    {
        alignas(16) double tmp[width];
        _mm_store_pd(&tmp[0], v);
        return tmp[i];
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE add(const type a, const type b) noexcept { return _mm_add_pd(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE subtract(const type a, const type b) noexcept { return _mm_sub_pd(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE multiply(const type a, const type b) noexcept { return _mm_mul_pd(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE divide(const type a, const type b) noexcept { return _mm_div_pd(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE negate(const type v) noexcept { return _mm_xor_pd(v, _mm_set1_pd(-0.0)); }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE equal(const type a, const type b) noexcept { return _mm_cmpeq_pd(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE not_equal(const type a, const type b) noexcept { return _mm_cmpneq_pd(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE less_than(const type a, const type b) noexcept { return _mm_cmplt_pd(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE less_equal(const type a, const type b) noexcept { return _mm_cmple_pd(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE greater_than(const type a, const type b) noexcept { return _mm_cmpgt_pd(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE greater_equal(const type a, const type b) noexcept { return _mm_cmpge_pd(a, b); }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE min(const type a, const type b) noexcept { return _mm_min_pd(b, a); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE max(const type a, const type b) noexcept { return _mm_max_pd(b, a); }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE is_nan(const type v) noexcept { return _mm_cmpunord_pd(v, v); }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE blend(const type a, const type b, const type c) noexcept
    {
        return _mm_blendv_pd(a, b, c);
    }
};

template <int N>
struct fixed_size
{
};

template <typename T>
struct compatible
{
};

template <typename T, typename Abi>
struct deduce;

template <>
struct deduce<std::int32_t, fixed_size<4>>
{
    using impl = sse42_backend<std::int32_t>;
    using mask_impl = sse42_mask_backend<std::int32_t>;
};
template <>
struct deduce<float, fixed_size<4>>
{
    using impl = sse42_backend<float>;
    using mask_impl = sse42_mask_backend<float>;
};
template <>
struct deduce<double, fixed_size<2>>
{
    using impl = sse42_backend<double>;
    using mask_impl = sse42_mask_backend<double>;
};

template <>
struct deduce<std::int32_t, compatible<std::int32_t>>
{
    using impl = sse42_backend<std::int32_t>;
    using mask_impl = sse42_mask_backend<std::int32_t>;
};
template <>
struct deduce<float, compatible<float>>
{
    using impl = sse42_backend<float>;
    using mask_impl = sse42_mask_backend<float>;
};
template <>
struct deduce<double, compatible<double>>
{
    using impl = sse42_backend<double>;
    using mask_impl = sse42_mask_backend<double>;
};

} // namespace detail
} // namespace simd_abi
} // namespace score::cpp

#undef SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_SIMD_SSE42_BACKEND_HPP
