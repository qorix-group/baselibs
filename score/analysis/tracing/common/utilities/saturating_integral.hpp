/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
#ifndef SCORE_ANALYSIS_TRACING_COMMON_UTILITIES_saturating_integral_HPP_
#define SCORE_ANALYSIS_TRACING_COMMON_UTILITIES_saturating_integral_HPP_

/**
 * @file saturating_integral.hpp
 * @brief Provides a small atomic counter that saturates and sets a flag bit on saturation.
 *
 * The counter uses @p N bits for the counter value and one additional bit as a saturation flag.
 * When the counter reaches its maximum value $(2^N - 1)$, further increments do not change the
 * counter and the saturation flag is set.
 */

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

namespace score
{
namespace analysis
{
namespace tracing
{

namespace detail
{

/**
 * @brief Reasons why an increment may not succeed.
 */

enum class ReasonToNotIncrement
{
    /** @brief The counter is saturated. */
    kSaturated,
    /** @brief The increment operation could not complete within the retry budget. */
    kRetriesExhausted
};

}  // namespace detail

/**
 * @brief Atomic saturating counter.
 *
 * The value is stored as an unsigned integer that packs:
 * - the counter in bits `[0, N-1]`
 * - the saturation flag in bit `N`
 *
 * Once the counter reaches its maximum value $(2^N - 1)$, it saturates and the flag bit is set.
 * Further increments return kSaturated.
 *
 * @tparam N Number of bits reserved for the counter (one extra bit is used for the flag).
 * @tparam MaxRetries Maximum number of retries for the increment operation to tolerate contention.
 */
template <std::size_t N>
class SaturatingIntegral
{
    static_assert(N < 64U, "SaturatingIntegral<N>: N must be < 64 (needs N counter bits plus one flag bit)");
    /**
     * @brief Selects an unsigned integer return type based on the required counter width.
     *
     * Return type selection follows these rules:
     * - If @p N <= 8: use `std::uint8_t`
     * - If 8 < @p N <= 16: use `std::uint16_t`
     * - If 16 < @p N <= 32: use `std::uint32_t`
     * - If 32 < @p N < 64: use `std::uint64_t`
     */
    struct return_type
    {
        using type =
            std::conditional_t<(N <= 8U),
                               std::uint8_t,
                               std::conditional_t<(N <= 16U),
                                                  std::uint16_t,
                                                  std::conditional_t<(N <= 32U), std::uint32_t, std::uint64_t>>>;
    };

    /**
     * @brief Selects an unsigned integer storage type based on the required counter width.
     *
     * Storage selection follows these rules:
     * - If @p N < 8: use `std::uint8_t`
     * - If 8 <= @p N < 16: use `std::uint16_t`
     * - If 16 <= @p N < 32: use `std::uint32_t`
     * - If 32 <= @p N < 64: use `std::uint64_t`
     */
    struct storage_type
    {
        using type = std::conditional_t<
            (N < 8U),
            std::uint8_t,
            std::conditional_t<(N < 16U), std::uint16_t, std::conditional_t<(N < 32U), std::uint32_t, std::uint64_t>>>;
    };

    using DataType = typename storage_type::type;

  public:
    using ReturnType = typename return_type::type;

  private:
    static constexpr DataType FLAG_MASK = static_cast<DataType>(DataType{1U} << N);
    static constexpr DataType COUNTER_MASK = static_cast<DataType>(FLAG_MASK - 1U);

  public:
    /**
     * @brief Constructs a SaturatingIntegral object with a specified maximum number of retries.
     *
     * @param max_retries The maximum number of retries allowed. Defaults to 100.
     *                     This value is stored and can be used to limit retry attempts
     *                     in saturating integral operations.
     *
     * @note The internal value is initialized to 0.
     */
    explicit SaturatingIntegral(std::uint32_t max_retries = 100U) : max_retries_(max_retries), value_(0U) {};
    /**
     * @brief Maximum representable counter value (before saturation).
     *
     * This is equal to $(2^N - 1)$.
     */
    static constexpr ReturnType kMaxCounterValue = static_cast<ReturnType>(COUNTER_MASK);

    /**
     * @brief Indicates whether the underlying atomic storage is always lock-free.
     *
     * @return True if `std::atomic<DataType>` is guaranteed to be lock-free for all
     *         objects of this type; false otherwise.
     */
    constexpr bool IsAlwaysLockFree() const noexcept
    {
        return std::atomic<DataType>::is_always_lock_free;
    }

    /**
     * @brief Atomically increments the counter by 1.
     *
     * If the counter has already saturated, no increment is performed.
     * The operation retries a limited number of times to tolerate contention.
     *
     * @return `score::cpp::blank{}` on success; otherwise an error describing why the increment did not happen.
     */

    score::cpp::expected_blank<detail::ReasonToNotIncrement> Increment()
    {
        DataType old = value_.load(std::memory_order_relaxed);

        for (std::uint32_t retries = 0U; retries < max_retries_; ++retries)
        {
            if (old >= FLAG_MASK)
            {
                // Saturation reached
                return score::cpp::unexpected(detail::ReasonToNotIncrement::kSaturated);
            }

            DataType counter = old & COUNTER_MASK;
            if (value_.compare_exchange_weak(old, counter + 1U, std::memory_order_relaxed, std::memory_order_relaxed))
            {
                return score::cpp::blank{};
            }
        }
        return score::cpp::unexpected(detail::ReasonToNotIncrement::kRetriesExhausted);
    }

    /**
     * @brief Loads the current counter value (flag bit masked out).
     *
     * @return The counter value in the range `[0, 2^N - 1]`.
     */
    ReturnType Load() const
    {
        return static_cast<ReturnType>(value_.load(std::memory_order_relaxed) & COUNTER_MASK);
    }

    /**
     * @brief Checks whether this instance is saturated.
     *
     * @return True if the saturation flag bit is set.
     */
    bool IsSaturated() const
    {
        return (value_.load(std::memory_order_relaxed) & FLAG_MASK) != 0;
    }

    /**
     * @brief Atomically exchanges the internal value with zero.
     *
     * On success, returns the counter value with the saturation flag masked out.
     * If the exchanged value was saturated (flag bit set), returns `score::cpp::unexpected(true)`.
     *
     * @return `std::pair<ReturnType, bool>` containing the counter value, or an error indicating
     *         saturation.
     */
    std::pair<ReturnType, bool> ExchangeWithZero()
    {
        DataType val = value_.exchange(0U, std::memory_order_relaxed);
        if (val >= FLAG_MASK)
        {
            return {kMaxCounterValue, true};
        }
        else
        {
            return {static_cast<ReturnType>(val & COUNTER_MASK), false};
        }
    }

  private:
    std::uint32_t max_retries_;
    std::atomic<DataType> value_;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_COMMON_UTILITIES_saturating_integral_HPP_
