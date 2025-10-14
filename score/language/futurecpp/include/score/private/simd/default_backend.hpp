///
/// \file
/// \copyright Copyright (C) 2020-2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_DEFAULT_BACKEND_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_DEFAULT_BACKEND_HPP

#include <score/private/simd/abi.hpp>
#include <score/private/simd/array.hpp>

#include <score/float.hpp>
#include <score/math.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace score::cpp
{
namespace simd
{

namespace detail
{

template <typename T, std::int32_t N>
struct simd_vector
{
    alignas(N * sizeof(T)) T v[static_cast<unsigned>(N)];
};

template <std::int32_t N>
struct simd_mask_default_backend
{
    using type = simd_vector<bool, N>;
    static constexpr std::size_t width{N};

    static simd_vector<bool, N> broadcast(const bool v) noexcept
    {
        simd_vector<bool, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = v;
        }
        return r;
    }

    template <typename G, std::size_t... Is>
    static simd_vector<bool, N> init(G&& gen, const std::index_sequence<Is...>) noexcept
    {
        static_assert(N == sizeof...(Is), "size mismatch");
        return {gen(std::integral_constant<std::size_t, Is>{})...};
    }

    static bool extract(const simd_vector<bool, N>& v, const size_t i) noexcept { return v.v[i]; }

    static simd_vector<bool, N> logical_not(const simd_vector<bool, N>& v) noexcept
    {
        simd_vector<bool, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = !v.v[i];
        }
        return r;
    }

    static simd_vector<bool, N> logical_and(const simd_vector<bool, N>& a, const simd_vector<bool, N>& b) noexcept
    {
        simd_vector<bool, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = a.v[i] && b.v[i];
        }
        return r;
    }

    static simd_vector<bool, N> logical_or(const simd_vector<bool, N>& a, const simd_vector<bool, N>& b) noexcept
    {
        simd_vector<bool, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = a.v[i] || b.v[i];
        }
        return r;
    }

    static bool all_of(const simd_vector<bool, N>& v) noexcept
    {
        for (std::int32_t i{}; i < N; ++i)
        {
            if (!v.v[i])
            {
                return false;
            }
        }
        return true;
    }

    static bool any_of(const simd_vector<bool, N>& v) noexcept
    {
        for (std::int32_t i{}; i < N; ++i)
        {
            if (v.v[i])
            {
                return true;
            }
        }
        return false;
    }

    static bool none_of(const simd_vector<bool, N>& v) noexcept
    {
        for (std::int32_t i{}; i < N; ++i)
        {
            if (v.v[i])
            {
                return false;
            }
        }
        return true;
    }
};

template <typename T, std::int32_t N>
struct simd_default_backend
{
    using type = simd_vector<T, N>;
    using mask_type = typename simd_mask_default_backend<N>::type;
    static constexpr std::size_t width{N};

    static simd_vector<T, N> broadcast(const T v) noexcept
    {
        simd_vector<T, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = v;
        }
        return r;
    }

    template <typename G, std::size_t... Is>
    static simd_vector<T, N> init(G&& gen, const std::index_sequence<Is...>) noexcept
    {
        static_assert(N == sizeof...(Is), "size mismatch");
        return {gen(std::integral_constant<std::size_t, Is>{})...};
    }

    static simd_vector<T, N> load(const T* const v)
    {
        simd_vector<T, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = v[i];
        }
        return r;
    }

    static simd_vector<T, N> load_aligned(const T* const v) { return load(v); }

    static void store(T* const v, const simd_vector<T, N>& a)
    {
        for (std::int32_t i{0}; i < N; ++i)
        {
            v[i] = a.v[i];
        }
    }

    static void store_aligned(T* const v, const simd_vector<T, N>& a) { store(v, a); }

    static T extract(const simd_vector<T, N>& v, const size_t i) noexcept { return v.v[i]; }

    static simd_vector<T, N> add(const simd_vector<T, N>& a, const simd_vector<T, N>& b) noexcept
    {
        simd_vector<T, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = a.v[i] + b.v[i];
        }
        return r;
    }

    static simd_vector<T, N> subtract(const simd_vector<T, N>& a, const simd_vector<T, N>& b) noexcept
    {
        simd_vector<T, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = a.v[i] - b.v[i];
        }
        return r;
    }

    static simd_vector<T, N> multiply(const simd_vector<T, N>& a, const simd_vector<T, N>& b) noexcept
    {
        simd_vector<T, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = a.v[i] * b.v[i];
        }
        return r;
    }

    static simd_vector<T, N> divide(const simd_vector<T, N>& a, const simd_vector<T, N>& b) noexcept
    {
        simd_vector<T, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = a.v[i] / b.v[i];
        }
        return r;
    }

    static simd_vector<T, N> negate(const simd_vector<T, N>& v) noexcept
    {
        simd_vector<T, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = -v.v[i];
        }
        return r;
    }

    static simd_vector<bool, N> equal(const simd_vector<T, N>& a, const simd_vector<T, N>& b) noexcept
    {
        simd_vector<bool, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = score::cpp::equals_bitexact(a.v[i], b.v[i]);
        }
        return r;
    }

    static simd_vector<bool, N> not_equal(const simd_vector<T, N>& a, const simd_vector<T, N>& b) noexcept
    {
        simd_vector<bool, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = !score::cpp::equals_bitexact(a.v[i], b.v[i]);
        }
        return r;
    }

    static simd_vector<bool, N> less_than(const simd_vector<T, N>& a, const simd_vector<T, N>& b) noexcept
    {
        simd_vector<bool, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = a.v[i] < b.v[i];
        }
        return r;
    }

    static simd_vector<bool, N> less_equal(const simd_vector<T, N>& a, const simd_vector<T, N>& b) noexcept
    {
        simd_vector<bool, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = a.v[i] <= b.v[i];
        }
        return r;
    }

    static simd_vector<bool, N> greater_than(const simd_vector<T, N>& a, const simd_vector<T, N>& b) noexcept
    {
        simd_vector<bool, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = a.v[i] > b.v[i];
        }
        return r;
    }

    static simd_vector<bool, N> greater_equal(const simd_vector<T, N>& a, const simd_vector<T, N>& b) noexcept
    {
        simd_vector<bool, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = a.v[i] >= b.v[i];
        }
        return r;
    }

    static simd_vector<T, N> min(const simd_vector<T, N>& a, const simd_vector<T, N>& b) noexcept
    {
        simd_vector<T, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = std::min(a.v[i], b.v[i]);
        }
        return r;
    }

    static simd_vector<T, N> max(const simd_vector<T, N>& a, const simd_vector<T, N>& b) noexcept
    {
        simd_vector<T, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = std::max(a.v[i], b.v[i]);
        }
        return r;
    }

    template <typename To>
    static simd_vector<To, N> convert(const simd_vector<T, N>& v, To) noexcept
    {
        static_assert(sizeof(To) == sizeof(T), "Mismatch in number of elements");
        simd_vector<To, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = static_cast<To>(v.v[i]);
        }
        return r;
    }

    static simd_vector<bool, N> is_nan(const simd_vector<T, N>& v) noexcept
    {
        static_assert(std::is_floating_point<T>::value, "not a floating point type");
        simd_vector<bool, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = score::cpp::isnan(v.v[i]);
        }
        return r;
    }

    static simd_vector<T, N>
    blend(const simd_vector<T, N>& a, const simd_vector<T, N>& b, const simd_vector<bool, N>& c) noexcept
    {
        simd_vector<T, N> r;
        for (std::int32_t i{0}; i < N; ++i)
        {
            r.v[i] = c.v[i] ? b.v[i] : a.v[i];
        }
        return r;
    }
};

template <typename T>
struct scalar_abi;

template <>
struct scalar_abi<std::int32_t>
{
    using impl = simd_default_backend<std::int32_t, 4>;
    using mask_impl = simd_mask_default_backend<4>;
};
template <>
struct scalar_abi<float>
{
    using impl = simd_default_backend<float, 4>;
    using mask_impl = simd_mask_default_backend<4>;
};
template <>
struct scalar_abi<double>
{
    using impl = simd_default_backend<double, 2>;
    using mask_impl = simd_mask_default_backend<2>;
};

template <typename T, std::size_t N>
struct scalar_array_abi;

template <>
struct scalar_array_abi<float, 16>
{
    using impl = array<float, simd_default_backend<float, 4>, simd_mask_default_backend<4>, 0, 1, 2, 3>;
    using mask_impl = array_mask<float, simd_mask_default_backend<4>, 0, 1, 2, 3>;
};

template <>
struct native_abi<std::int32_t>
{
    using type = scalar_abi<std::int32_t>;
};
template <>
struct native_abi<float>
{
    using type = scalar_abi<float>;
};
template <>
struct native_abi<double>
{
    using type = scalar_abi<double>;
};

template <>
struct deduce_abi<std::int32_t, 4>
{
    using type = native_abi<std::int32_t>::type;
};
template <>
struct deduce_abi<float, 4>
{
    using type = native_abi<float>::type;
};
template <>
struct deduce_abi<float, 16>
{
    using type = scalar_array_abi<float, 16>;
};
template <>
struct deduce_abi<double, 2>
{
    using type = native_abi<double>::type;
};

} // namespace detail

template <>
struct is_abi_tag<detail::scalar_abi<std::int32_t>> : std::true_type
{
};
template <>
struct is_abi_tag<detail::scalar_abi<float>> : std::true_type
{
};
template <>
struct is_abi_tag<detail::scalar_array_abi<float, 16>> : std::true_type
{
};
template <>
struct is_abi_tag<detail::scalar_abi<double>> : std::true_type
{
};

} // namespace simd
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_DEFAULT_BACKEND_HPP
