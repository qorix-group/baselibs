///
/// @file atomic_container.h
/// @copyright Copyright (C) 2025, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief AtomicContainer header file
///

#ifndef SCORE_ANALYSIS_TRACING_LIBRARY_ATOMIC_CONTAINER_H
#define SCORE_ANALYSIS_TRACING_LIBRARY_ATOMIC_CONTAINER_H

#include "score/analysis/tracing/common/interface_types/types.h"
#include "score/memory/shared/atomic_indirector.h"

#include <cassert>
#include <cstdint>
#include <thread>
#include <type_traits>

namespace score
{
namespace analysis
{
namespace tracing
{

/**
 * @brief A lock-free, thread-safe container for managing a fixed-size array with atomic elements.
 * @tparam T The type of elements stored in the container.
 * @tparam AtomicContainerSize The fixed size of the container.
 * @tparam AtomicIndirectorType A template parameter for mocking the atomic operations.
 */
template <class T,
          std::size_t AtomicContainerSize,
          template <class> class AtomicIndirectorType = memory::shared::AtomicIndirectorReal>
class AtomicContainer
{
    static_assert(AtomicContainerSize > 0, "AtomicContainerSize must be greater than 0");
    class AtomicContainerElement;
    using ReferenceT = typename std::reference_wrapper<T>;
    using ConstReferenceT = typename std::reference_wrapper<const T>;

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
    using iterator = typename std::array<AtomicContainerElement, AtomicContainerSize>::iterator;
    using const_iterator = const typename std::array<AtomicContainerElement, AtomicContainerSize>::iterator;

    /**
     * @brief Constructs an atomic container with all elements marked as not occupied.
     */
    AtomicContainer() : first_empty_{0U}, empty_elements_count_{AtomicContainerSize} {};

    AtomicContainer(const AtomicContainer&) = delete;
    AtomicContainer& operator=(const AtomicContainer&) = delete;
    AtomicContainer(AtomicContainer&&) = delete;
    AtomicContainer& operator=(AtomicContainer&&) = delete;
    ~AtomicContainer() = default;

    /**
     * @brief Accesses an element by index.
     * @param[in] index The index of the element.
     * @return Reference to the element.
     * @note The behavior is undefined if the index is out of bounds.
     */
    auto operator[](const std::size_t index) noexcept -> T&
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(index < AtomicContainerSize, "Index out of bounds");
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index): bounds already checked
        return atomic_container_[index].data_;
    }

    /**
     * @brief Accesses an element by index (const version).
     * @param[in] index The index of the element.
     * @return Const reference to the element.
     */
    auto operator[](const std::size_t index) const noexcept -> const T&
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(index < AtomicContainerSize, "Index out of bounds");
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index): bounds already checked
        return atomic_container_[index].data_;
    }

    /**
     * @brief Finds the first element that satisfies a predicate (const version).
     * @param[in] predicate A function to test elements.
     * @return Optional containing a reference to the found element, or nullopt if not found.
     */
    auto FindIf(const std::function<bool(const T&)>& predicate) const noexcept -> std::optional<ConstReferenceT>
    {
        for (std::size_t index = 0U; index < AtomicContainerSize; ++index)
        {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index): bounds already checked
            const auto& atomic_element = atomic_container_[index];
            if (AtomicIndirectorType<bool>::load(atomic_element.is_occupied_, std::memory_order_acquire) &&
                predicate(atomic_element.data_))
            {
                return atomic_element.data_;
            }
        }
        return std::nullopt;
    }

    /**
     * @brief Finds the first element that satisfies a predicate (non-const version).
     * @param[in] predicate A function to test elements.
     * @return Optional containing a reference to the found element, or nullopt if not found.
     */
    auto FindIf(const std::function<bool(const T&)>& predicate) noexcept -> std::optional<ReferenceT>
    {
        for (std::size_t index = 0U; index < AtomicContainerSize; ++index)
        {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index): bounds already checked
            auto& atomic_element = atomic_container_[index];
            if (AtomicIndirectorType<bool>::load(atomic_element.is_occupied_, std::memory_order_acquire) &&
                predicate(atomic_element.data_))
            {
                return atomic_element.data_;
            }
        }
        return std::nullopt;
    }

    /**
     * @brief Releases an element by value.
     * @param[in] to_release The element to release.
     */
    auto Release(const T& to_release) noexcept -> void
    {
        for (auto& atomic_element : atomic_container_)
        {
            bool was_occupied = true;
            if (atomic_element.data_ == to_release &&
                AtomicIndirectorType<bool>::compare_exchange_strong(atomic_element.is_occupied_,
                                                                    was_occupied,
                                                                    false,
                                                                    std::memory_order_acq_rel,
                                                                    std::memory_order_release))
            {
                std::ignore =
                    AtomicIndirectorType<atomic_underlying_type_t<decltype(empty_elements_count_)>>::fetch_add(
                        empty_elements_count_, 1U, std::memory_order_acq_rel);
                return;
            }
        }
    }

    /**
     * @brief Releases an element by index.
     * @param[in] index The index of the element to release.
     */
    auto Release(std::size_t index) noexcept -> void
    {
        if (index < AtomicContainerSize)
        {
            bool was_occupied = true;
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index): bounds already checked
            if (AtomicIndirectorType<bool>::compare_exchange_strong(atomic_container_[index].is_occupied_,
                                                                    was_occupied,
                                                                    false,
                                                                    std::memory_order_acq_rel,
                                                                    std::memory_order_release))
            {
                std::ignore =
                    AtomicIndirectorType<atomic_underlying_type_t<decltype(empty_elements_count_)>>::fetch_add(
                        empty_elements_count_, 1U, std::memory_order_acq_rel);
            }
        }
    }

    /**
     * @brief Acquires an available element.
     * @return Optional containing a reference to the acquired element, or nullopt if none are available.
     */
    auto Acquire() noexcept -> std::optional<ReferenceT>
    {
        if (AtomicIndirectorType<atomic_underlying_type_t<decltype(empty_elements_count_)>>::load(
                empty_elements_count_, std::memory_order_acquire) == 0U)
        {
            return std::nullopt;
        }

        const std::size_t start_index = AtomicIndirectorType<atomic_underlying_type_t<decltype(first_empty_)>>::load(
            first_empty_, std::memory_order_acquire);
        for (std::size_t i = 0U; i < AtomicContainerSize; ++i)
        {
            // No possibility for having data loss here
            // coverity[autosar_cpp14_a4_7_1_violation]
            auto index = (start_index + i) % AtomicContainerSize;
            bool not_occupied = false;
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index): bounds already checked
            auto& atomic_element = atomic_container_[index];
            if (AtomicIndirectorType<bool>::compare_exchange_weak(atomic_element.is_occupied_,
                                                                  not_occupied,
                                                                  true,
                                                                  std::memory_order_acquire,
                                                                  std::memory_order_relaxed))
            {
                std::ignore =
                    AtomicIndirectorType<atomic_underlying_type_t<decltype(empty_elements_count_)>>::fetch_sub(
                        empty_elements_count_, 1U, std::memory_order_acq_rel);
                AtomicIndirectorType<atomic_underlying_type_t<decltype(first_empty_)>>::store(
                    first_empty_, (index + 1U) % AtomicContainerSize, std::memory_order_relaxed);
                return atomic_element.data_;
            }
        }
        return std::nullopt;
    }

    /**
     * @brief Checks if an element at a given index is free.
     * @param[in] index The index of the element.
     * @return True if the element is free, false otherwise.
     */
    auto IsFreeAt(const std::size_t index) const noexcept -> bool
    {
        if (index >= AtomicContainerSize)
        {
            return false;
        }
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index): bounds already checked
        return !AtomicIndirectorType<bool>::load(atomic_container_[index].is_occupied_, std::memory_order_acquire);
    }

    /**
     * @brief Retrieves the number of free elements in the container.
     * @return The count of free elements.
     */
    auto GetEmptyElementsCount() const noexcept -> std::size_t
    {
        return empty_elements_count_;
    }

    /**
     * @brief Gets the total size of the container.
     * @return The total capacity of the container.
     */
    auto Size() const noexcept -> std::size_t
    {
        return AtomicContainerSize;
    }

  private:
    /**
     * @brief Internal structure representing an atomic container element.
     */
    class AtomicContainerElement
    {
      public:
        AtomicContainerElement() : data_{}, is_occupied_{false} {}
        /// @brief The actual data stored in the element.
        // member definition as public is intented
        // coverity[autosar_cpp14_m11_0_1_violation]
        T data_;
        /// @brief Atomic flag indicating whether the element is occupied.
        // member definition as public is intented
        // coverity[autosar_cpp14_m11_0_1_violation]
        std::atomic<bool> is_occupied_;  // MS
    };

    /// @brief Index of the first available element (not guaranteed accurate in concurrent usage).
    std::atomic<std::size_t> first_empty_;

    /// @brief Number of currently free elements in the container.
    std::atomic<std::size_t> empty_elements_count_;

    /// @brief Fixed-size array of atomic container elements, ensuring cache-line alignment.
    alignas(64) std::array<AtomicContainerElement, AtomicContainerSize> atomic_container_;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_LIBRARY_ATOMIC_CONTAINER_H
