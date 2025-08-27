///
/// @file atomic_ring_buffer.h
/// @copyright Copyright (C) 2025, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#ifndef SCORE_ANALYSIS_TRACING_LIBRARY_ATOMIC_RING_BUFFER_H
#define SCORE_ANALYSIS_TRACING_LIBRARY_ATOMIC_RING_BUFFER_H

#include "score/analysis/tracing/common/interface_types/types.h"
#include "score/analysis/tracing/library/generic_trace_api/containers/atomic_ring_buffer/atomic_ring_buffer_state.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"
#include "score/memory/shared/atomic_indirector.h"

#include <cassert>
#include <cstdint>

namespace score
{
namespace analysis
{
namespace tracing
{

template <class T,
          std::uint16_t AtomicRingBufferSize,
          template <class> class AtomicIndirectorType = memory::shared::AtomicIndirectorReal>
class AtomicRingBuffer
{
    template <typename AtomicType>
    struct atomic_underlying_type;

    template <typename AtomicType>
    struct atomic_underlying_type<std::atomic<AtomicType>>
    {
        using type = AtomicType;
    };

    template <typename AtomicType>
    using atomic_underlying_type_t =
        typename atomic_underlying_type<std::remove_cv_t<std::remove_reference_t<AtomicType>>>::type;

  public:
    struct AtomicRingBufferElement
    {
        // No harm from not declaring private members here
        // coverity[autosar_cpp14_m11_0_1_violation]
        T data_;
        // No harm from not declaring private members here
        // coverity[autosar_cpp14_m11_0_1_violation]
        std::atomic<bool> is_ready_ = false;
    };
    using AtomicRingBufferResult = score::Result<std::reference_wrapper<AtomicRingBufferElement>>;

    AtomicRingBuffer() : state_{AtomicRingBufferState{1U, 0U, 0U}}, empty_elements_count_{AtomicRingBufferSize} {};
    AtomicRingBuffer(const AtomicRingBuffer&) = delete;
    AtomicRingBuffer& operator=(const AtomicRingBuffer&) = delete;
    AtomicRingBuffer(AtomicRingBuffer&&) = delete;
    AtomicRingBuffer& operator=(AtomicRingBuffer&&) = delete;
    ~AtomicRingBuffer() = default;

    auto GetEmptyElementsCount() const noexcept -> std::size_t
    {
        return empty_elements_count_;
    }

    auto Size() const noexcept -> std::uint16_t
    {
        return AtomicRingBufferSize;
    }

    // "AUTOSAR C++14 A15-5-3" rule finding. This rule states: "The std::terminate()
    // function shall not be called implicitly"
    // container_.at() will not throw exception as passed index is always in boundary of array
    // coverity[autosar_cpp14_a15_5_3_violation]
    AtomicRingBufferResult GetEmptyElement() noexcept
    {
        constexpr std::uint32_t get_write_element_max_retries = 10U;
        constexpr std::uint16_t mask_15_bits = 0x7FFFU;

        for (std::uint32_t retry_counter = 0U; retry_counter < get_write_element_max_retries; retry_counter++)
        {
            AtomicRingBufferState current_state =
                AtomicIndirectorType<atomic_underlying_type_t<decltype(state_)>>::load(state_,
                                                                                       std::memory_order_acquire);
            if (!IsBufferFull(current_state))
            {
                const std::uint16_t previous_end_index = current_state.end;
                const auto new_end_index = static_cast<std::uint16_t>((previous_end_index + 1U) % AtomicRingBufferSize);
                AtomicRingBufferState new_state{0U,
                                                static_cast<std::uint16_t>(current_state.start & mask_15_bits),
                                                static_cast<std::uint16_t>(new_end_index & mask_15_bits)};
                if (AtomicIndirectorType<atomic_underlying_type_t<decltype(state_)>>::compare_exchange_strong(
                        state_, current_state, new_state, std::memory_order_acq_rel, std::memory_order_relaxed) == true)
                {
                    score::cpp::ignore =
                        AtomicIndirectorType<atomic_underlying_type_t<decltype(empty_elements_count_)>>::fetch_sub(
                            empty_elements_count_, 1U, std::memory_order_release);
                    // Suppress "AUTOSAR C++14 M5-0-3" rule findings. This rule states: "A cvalue expression shall
                    // not be implicitly converted to a different underlying type"
                    // False positive, container_.at(previous_end_index) is AtomicRingBufferElement type.
                    // coverity[autosar_cpp14_m5_0_3_violation]
                    return container_.at(previous_end_index);
                }
            }
            else
            {
                return score::MakeUnexpected(ErrorCode::kAtomicRingBufferFullRecoverable);
            }
        }
        return score::MakeUnexpected(ErrorCode::kAtomicRingBufferMaxRetriesRecoverable);
    }

    // "AUTOSAR C++14 A15-5-3" rule finding. This rule states: "The std::terminate()
    // function shall not be called implicitly"
    // container_.at() will not throw exception as passed index is always in boundary of array
    // coverity[autosar_cpp14_a15_5_3_violation]
    AtomicRingBufferResult GetReadyElement() noexcept
    {
        constexpr std::uint32_t get_read_element_max_retries = 10U;
        for (std::uint32_t retry_counter = 0U; retry_counter < get_read_element_max_retries; retry_counter++)
        {
            AtomicRingBufferState current_state =
                AtomicIndirectorType<atomic_underlying_type_t<decltype(state_)>>::load(state_,
                                                                                       std::memory_order_acquire);
            if (current_state.empty != 0U)
            {
                return score::MakeUnexpected(ErrorCode::kAtomicRingBufferEmptyRecoverable);
            }

            const std::uint16_t start_index = current_state.start;
            // Suppress "AUTOSAR C++14 M5-0-3" rule findings. This rule states: "A cvalue expression shall
            // not be implicitly converted to a different underlying type"
            // False positive, is_ready_ is bool type.
            // coverity[autosar_cpp14_m5_0_3_violation]
            if (container_.at(start_index).is_ready_)
            {
                // Suppress "AUTOSAR C++14 M5-0-3" rule findings. This rule states: "A cvalue expression shall
                // not be implicitly converted to a different underlying type"
                // False positive, container_.at(previous_end_index) is AtomicRingBufferElement type.
                // coverity[autosar_cpp14_m5_0_3_violation]
                return container_.at(start_index);
            }
        }
        return score::MakeUnexpected(ErrorCode::kAtomicRingBufferMaxRetriesRecoverable);
    }

    bool ReleaseReadyElement() noexcept
    {
        constexpr std::uint32_t get_read_element_max_retries = 10U;
        constexpr std::uint16_t mask_15_bits = 0x7FFFU;

        for (std::uint32_t retry_counter = 0U; retry_counter < get_read_element_max_retries; retry_counter++)
        {
            AtomicRingBufferState current_state =
                AtomicIndirectorType<atomic_underlying_type_t<decltype(state_)>>::load(state_,
                                                                                       std::memory_order_acquire);

            if (current_state.empty != 0U)
            {
                return false;
            }

            const std::uint16_t start_index = current_state.start;
            const auto new_start_index = static_cast<std::uint16_t>((start_index + 1U) % AtomicRingBufferSize);
            AtomicRingBufferState new_state{(new_start_index & mask_15_bits) == (current_state.end & mask_15_bits)
                                                ? static_cast<std::uint8_t>(1U)
                                                : static_cast<std::uint8_t>(0U),
                                            static_cast<std::uint16_t>(new_start_index & mask_15_bits),
                                            static_cast<std::uint16_t>(current_state.end & mask_15_bits)};
            if (AtomicIndirectorType<atomic_underlying_type_t<decltype(state_)>>::compare_exchange_strong(
                    state_, current_state, new_state, std::memory_order_acq_rel, std::memory_order_relaxed) == true)
            {
                score::cpp::ignore =
                    AtomicIndirectorType<atomic_underlying_type_t<decltype(empty_elements_count_)>>::fetch_add(
                        empty_elements_count_, 1U, std::memory_order_acquire);
                return true;
            }
        }
        return false;
    }

  private:
    bool IsBufferFull(AtomicRingBufferState buffer_state)
    {
        return (buffer_state.start == buffer_state.end) && (buffer_state.empty == 0U);
    }

    std::atomic<AtomicRingBufferState> state_;
    std::atomic<std::size_t> empty_elements_count_;
    alignas(64) std::array<AtomicRingBufferElement, AtomicRingBufferSize> container_;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_LIBRARY_ATOMIC_RING_BUFFER_H
