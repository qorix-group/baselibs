/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
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
#ifndef SCORE_LIB_CONTAINERS_DYNAMIC_ARRAY_H
#define SCORE_LIB_CONTAINERS_DYNAMIC_ARRAY_H

#include <score/assert.hpp>
#include <score/utility.hpp>

#include <cstddef>
#include <cstring>
#include <memory>
#include <type_traits>

namespace score::containers
{

namespace test
{
template <typename ElementType, typename Allocator>
class DynamicArrayTestAttorney;
}

namespace detail_dynamic_array
{

template <class T>
constexpr T* to_address(T* p)
{
// Suppress "AUTOSAR C++14 A16-0-1", The rule states:
// "The pre-processor shall only be used for unconditional and conditional file
// inclusion and include guards, and using the following directives: (1) #ifndef,
// (2) #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif,
// (9) #include."
// This check is needed as to_address is available in C++ version 20 and above.
// coverity[autosar_cpp14_a16_0_1_violation]
#if __cplusplus < 202002L
    // to_address is provided in C++20
    static_assert(!std::is_function_v<T>);
    return p;
// Suppress "AUTOSAR C++14 A16-0-1"
// This check is needed as to_address is available in C++ version 20 and above.
// coverity[autosar_cpp14_a16_0_1_violation]
#else
    return std::to_address(p);
// Suppress "AUTOSAR C++14 A16-0-1"
// This check is needed as to_address is available in C++ version 20 and above.
// coverity[autosar_cpp14_a16_0_1_violation]
#endif
}

template <class T>
constexpr auto to_address(const T& p)
{
// Suppress "AUTOSAR C++14 A16-0-1"
// This check is needed as to_address is available in C++ version 20 and above.
// coverity[autosar_cpp14_a16_0_1_violation]
#if __cplusplus < 202002L
    // to_address is provided in C++20
    return to_address(p.operator->());
// Suppress "AUTOSAR C++14 A16-0-1"
// This check is needed as to_address is available in C++ version 20 and above.
// coverity[autosar_cpp14_a16_0_1_violation]
#else
    return std::to_address(p);
// Suppress "AUTOSAR C++14 A16-0-1"
// This check is needed as to_address is available in C++ version 20 and above.
// coverity[autosar_cpp14_a16_0_1_violation]
#endif
}

/// \brief conjunction of type-traits used for selection of initialize-method variant DynamicArray<>:Initialize()
/// \details The type trait std::is_trivially_copyable is included to allow the use of memset for initialization.
///          The type trait std::is_trivially_copy_assignable is also included because GCC issues a warning
///          (-Werror=class-memaccess) when memset is used to initialize a type that is not trivially assignable.
///          This warning is somewhat unexpected, but it is addressed by including the additional type trait.
template <class T>
constexpr bool trivially_initializable_v = std::conjunction<std::is_trivially_default_constructible<T>,
                                                            std::is_trivially_copy_assignable<T>,
                                                            std::is_trivially_copyable<T>,
                                                            std::is_standard_layout<T>>::value;

}  // namespace detail_dynamic_array

/// \brief Fixed-size array data structure whose size can be dynamically set at construction.
///
/// The DynamicArray should be used if a vector-like data structure is needed whose size is not known at compile time
/// but only needs to be set once (i.e. on construction of DynamicArray) and never resized.
///
/// \details The implementation also provides some "performance optimizations" in its iterator handling:
/// It makes sure to return "raw-pointers" as its begin/end-iterators. E.g. independent from the pointer-type, the
/// underlying Allocator provides, it doesn't return these pointer-types 1:1 as begin/end iterator, but returns
/// raw-pointers instead. This is a performance benefit, when we use DynamicArray in shared-memory, where we use
/// score::memory::shared::PolymorphicOffsetPtrAllocator as our allocator, which creates fancy OffsetPtr.
/// Dereferencing an OffsetPtr is expensive as it applies bounds-checking for safety-reasons. But when iterating over
/// a DynamicArray residing in shared-memory, doing a bounds-check for every element is unnecessary. Only the 1st/last
/// OffsetPtr needs to be checked. This is exactly the iterator optimization applied here: Only the start/end gets
/// bounds-checked and if this is successful a raw-pointer is handed out as an iterator.
template <typename ElementType, typename Allocator = std::allocator<ElementType>>
class DynamicArray
{
    // Suppress "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // The 'friend' class is employed to encapsulate non-public members. Friend classes provide controlled access to
    // private members, utilized internally, ensuring that end users cannot access implementation specifics. In this
    // case, it is used for testing only in order to simulate corruption of the underlying pointer type to the dynamic
    // storage, which definitely should not be in the public interface.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class test::DynamicArrayTestAttorney<ElementType, Allocator>;

    using ptr_index_type =
        typename std::pointer_traits<typename std::allocator_traits<Allocator>::pointer>::difference_type;

  public:
    using value_type = ElementType;
    using reference = ElementType&;
    using const_reference = const ElementType&;
    using iterator = ElementType*;
    using const_iterator = const ElementType*;
    using size_type = std::size_t;

    /// \brief Constructor which default initialises the elements underlying array of size array_size
    /// \param number_of_elements The size of the underlying array
    /// \param alloc allocator instance to be used, defaults to default constructed allocator of type Allocator

    // In this case, we can't delegate to the default-value extended ctor and vice versa as the dynamic_array_
    // initialization is specific to each case.
    // coverity[autosar_cpp14_a12_1_5_violation]
    explicit DynamicArray(const size_type number_of_elements, Allocator alloc = Allocator())
        : dynamic_array_{initialize_array(number_of_elements, alloc)},
          alloc_{std::move(alloc)},
          size_{number_of_elements}
    {
    }

    /// \brief Constructor which creates a DynamicArray with number_of_elements copies of elements with value value.
    /// \attention Obviously in this case ElementType needs to be copyable.
    /// \param number_of_elements The size of the underlying array
    /// \param value the value to copy elements from
    /// \param alloc allocator instance to be used, defaults to default constructed allocator of type Allocator
    explicit DynamicArray(const size_type number_of_elements, const ElementType& value, Allocator alloc = Allocator())
        : dynamic_array_{initialize_array(number_of_elements, value, alloc)},
          alloc_{std::move(alloc)},
          size_{number_of_elements}
    {
    }

    /// \brief We currently delete copy-construct, as we don't want to introduce req. for ElementType to be copyable.
    DynamicArray(const DynamicArray& other);
    DynamicArray(DynamicArray&& other) noexcept;

    /// \brief We currently delete copy-assign, as we don't want to introduce req. for ElementType to be copyable.
    DynamicArray& operator=(const DynamicArray& other) = delete;
    DynamicArray& operator=(DynamicArray&& other) noexcept;

    ~DynamicArray() noexcept
    {
        Destruct();
    }

    ElementType& at(const size_type index);

    const ElementType& at(const size_type index) const;

    ElementType& operator[](const size_type index);

    const ElementType& operator[](const size_type index) const;

    size_type size() const noexcept
    {
        return size_;
    }

    value_type* data();

    const value_type* data() const;

    /// \brief returns iterator (pointer) to the 1st element of the dynamic array
    /// \attention The returned iterator (pointer) isn't suited to be placed into shared-memory! I.e. even if the
    /// underlying allocator is a shared-memory enabled alloc providing fancy/offset-ptr, the returned iterator is
    /// semantically a raw-pointer.
    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;

    /// \brief returns past the end iterator (pointer) of the dynamic array
    iterator end();
    const_iterator end() const;
    const_iterator cend() const;

  private:
    using pointer = typename std::allocator_traits<Allocator>::pointer;

    /// \brief Internal array initialization in case of a Element type is NOT a POD type.
    /// \details In this case, we do an element wise construction via delegation to Alloc::construct(). We make it
    /// static to ensure that no member variables are modified by another process (in case the DynamicArray is in shared
    /// memory) while initializing the data.
    /// Note. No bounds checking has to be done during the initialization, as there's no data in shared memory at this
    /// point.
    /// \tparam Alloc Allocator being used - this is the class template argument Allocator
    /// \tparam Element Element type being used - this is the class template argument ElementType
    /// \param number_of_elements number of elements of type Element, which the array shall hold.
    template <typename Alloc = Allocator,
              typename Element = ElementType,
              typename std::enable_if<!detail_dynamic_array::trivially_initializable_v<Element>, bool>::type = true>
    static pointer initialize_array(size_type number_of_elements, Alloc& allocator);

    /// \brief Internal array initialization in case of Element type is a POD type, which can be initialized/default
    ///        constructed by simply zeroing out the underlying memory.
    /// \details In this case, we do a simple/fast memset to zero-out the underlying storage. We make it
    /// static to ensure that no member variables are modified by another process (in case the DynamicArray is in shared
    /// memory) while initializing the data.
    /// Note. No bounds checking has to be done during the initialization, as there's no data in shared memory at this
    /// point.
    /// \tparam Alloc Allocator being used - this is the class template argument Allocator
    /// \tparam Element Element type being used - this is the class template argument ElementType
    /// \param number_of_elements number of elements of type Element, which the array shall hold.
    template <typename Alloc = Allocator,
              typename Element = ElementType,
              typename std::enable_if<detail_dynamic_array::trivially_initializable_v<Element>, bool>::type = true>
    static pointer initialize_array(size_type number_of_elements, Alloc& allocator);

    /// \brief Internal array initialization, which initializes all elements with the given value (by
    /// copy-construction).
    ///        This function is used for POD and non-POD types, as there is no specific optimization potential for
    ///        POD-types in this case (as in the default-init case)
    /// \attention This function is not usable/applicable for non-copyable ElementType, because it uses
    ///            copy-construction. This is also the reason, that we can't combine different variants of
    ///            #initialize_array into one implementation with different paths (with or without init-value) as this
    ///            implementation would be viable for non-copyable ElementType.
    ///
    /// \param number_of_elements number of elements of type Element, which the array shall hold.
    /// \param value value from which the elemet copies get initialized
    /// \param allocator Allocator being used - this is the class template argument Allocator
    /// \return pointer to the allocated/initialized storage
    static pointer initialize_array(size_type number_of_elements, const ElementType& value, Allocator& allocator);

    template <typename Alloc = Allocator,
              typename Element = ElementType,
              typename std::enable_if<!detail_dynamic_array::trivially_initializable_v<Element>, bool>::type = true>
    // Must be noexcept because it's used in the destructor.
    // Though deallocate/destroy aren’t formally noexcept, we assume they don't throw.
    // coverity[autosar_cpp14_a15_5_3_violation]
    void Destruct() noexcept;

    template <typename Alloc = Allocator,
              typename Element = ElementType,
              typename std::enable_if<detail_dynamic_array::trivially_initializable_v<Element>, bool>::type = true>
    // Must be noexcept because it's used in the destructor.
    // Though deallocate/destroy aren’t formally noexcept, we assume they don't throw.
    // coverity[autosar_cpp14_a15_5_3_violation]
    void Destruct() noexcept;
    iterator GetPastTheEndIterator() const;

    /// \brief Gets an iterator to the first element of the array. Returns nullptr if size_ == 0
    ///
    /// Function is static to ensure that no member variables are modified by another process (in case the DynamicArray
    /// is in shared memory) while getting the iterator
    static auto GetFirstElement(const pointer dynamic_array, const std::size_t dynamic_array_size) -> iterator;

    /// \brief Gets an iterator to the last element of the array. Returns nullptr if size_ == 0
    ///
    /// Function is static to ensure that no member variables are modified by another process (in case the DynamicArray
    /// is in shared memory) while getting the iterator
    static auto GetLastElement(const pointer dynamic_array, const std::size_t dynamic_array_size) -> iterator;

    typename std::allocator_traits<Allocator>::pointer dynamic_array_;
    Allocator alloc_;
    size_type size_;
};

template <typename ElementType, typename Allocator>
// false-positive: size_t and pointer is trivial, standard allocator move is noexcept
// coverity[autosar_cpp14_a15_5_3_violation]
// coverity[autosar_cpp14_a12_8_1_violation] false-positive
DynamicArray<ElementType, Allocator>::DynamicArray(DynamicArray&& other) noexcept
    // coverity[autosar_cpp14_a12_8_4_violation] false-positive: no copy semantic is used
    : dynamic_array_{std::move(other.dynamic_array_)}, alloc_{std::move(other.alloc_)}, size_{std::move(other.size_)}
{
    other.size_ = 0u;
}

template <typename ElementType, typename Allocator>
DynamicArray<ElementType, Allocator>::DynamicArray(const DynamicArray& other)
    : dynamic_array_{nullptr}, alloc_{other.alloc_}, size_{other.size_}
{
    // Early return prevents memory leaks with custom allocators that track zero-size allocations.
    // Some allocators (e.g., PolymorphicOffsetPtrAllocator) may return valid pointers and track resources
    // even for zero-byte allocations, but the destructor skips deallocation for size_= 0, causing leaks.
    if (size_ == 0U)
    {
        return;
    }

    using alloc_traits = std::allocator_traits<Allocator>;
    auto storage = alloc_traits::allocate(alloc_, size_);
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(nullptr != storage, "no memory allocated");

    // To avoid costly bounds checking when dereferencing cur_pointer (if the allocator is using OffsetPtr), we iterate
    // over raw pointers to the allocated storage.
    auto cur_pointer = detail_dynamic_array::to_address(storage);
    auto source_it = other.cbegin();

    for (size_t i = 0U; i < size_; i++)
    {
        // Suppress "AUTOSAR C++14 A5-3-2" rule finding.
        // This rule states: "Null pointers shall not be dereferenced.".
        // Justification: The above check ensures the memory is allocated and not null.
        // Therefore, dereferencing "cur_pointer" is safe.
        // coverity[autosar_cpp14_a5_3_2_violation]
        alloc_traits::construct(alloc_, cur_pointer, *source_it);
        source_it++;
        std::advance(cur_pointer, 1);
    }

    dynamic_array_ = storage;
}

template <typename ElementType, typename Allocator>
// false-positive: size_t and pointer is trivial, standard allocator move is noexcept
// coverity[autosar_cpp14_a15_5_3_violation]
// coverity[autosar_cpp14_a6_2_1_violation] false-positive: no side effects
auto DynamicArray<ElementType, Allocator>::operator=(DynamicArray&& other) noexcept -> DynamicArray&
{
    if (this != &other)
    {
        std::swap(size_, other.size_);
        std::swap(alloc_, other.alloc_);
        // coverity[autosar_cpp14_a6_2_1_violation] false-positive: no side effects
        std::swap(dynamic_array_, other.dynamic_array_);
    }
    return *this;
}

template <typename ElementType, typename Allocator>
template <typename Alloc,
          typename Element,
          typename std::enable_if<!detail_dynamic_array::trivially_initializable_v<Element>, bool>::type>
inline auto DynamicArray<ElementType, Allocator>::initialize_array(size_type number_of_elements, Alloc& allocator)
    -> pointer
{
    if (number_of_elements == 0U)
    {
        return nullptr;
    }
    using alloc_traits = std::allocator_traits<Alloc>;
    auto storage = alloc_traits::allocate(allocator, number_of_elements);
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(nullptr != storage, "no memory allocated");

    // To avoid costly bounds checking when dereferencing cur_pointer (if the allocator is using OffsetPtr), we iterate
    // over raw pointers to the allocated storage.
    auto cur_pointer = detail_dynamic_array::to_address(storage);
    for (size_t i = 0U; i < number_of_elements; i++)
    {
        // Suppress "AUTOSAR C++14 A5-3-2" rule finding.
        // This rule states: "Null pointers shall not be dereferenced.".
        // Justification: The above check ensures the memory is allocated and not null.
        // Therefore, dereferencing "cur_pointer" is safe.
        // coverity[autosar_cpp14_a5_3_2_violation]
        alloc_traits::construct(allocator, cur_pointer);
        std::advance(cur_pointer, 1);
    }
    return storage;
}

template <typename ElementType, typename Allocator>
inline auto DynamicArray<ElementType, Allocator>::initialize_array(size_type number_of_elements,
                                                                   const ElementType& value,
                                                                   Allocator& allocator) -> pointer
{
    if (number_of_elements == 0U)
    {
        return nullptr;
    }
    using alloc_traits = std::allocator_traits<Allocator>;
    auto storage = alloc_traits::allocate(allocator, number_of_elements);
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(nullptr != storage, "no memory allocated");

    // To avoid costly bounds checking when dereferencing cur_pointer (if the allocator is using OffsetPtr), we iterate
    // over raw pointers to the allocated storage.
    auto cur_pointer = detail_dynamic_array::to_address(storage);
    for (size_t i = 0U; i < number_of_elements; i++)
    {
        // Suppress "AUTOSAR C++14 A5-3-2" rule finding.
        // This rule states: "Null pointers shall not be dereferenced.".
        // Justification: The above check ensures the memory is allocated and not null.
        // Therefore, dereferencing "cur_pointer" is safe.
        // coverity[autosar_cpp14_a5_3_2_violation]
        alloc_traits::construct(allocator, cur_pointer, value);
        std::advance(cur_pointer, 1);
    }
    return storage;
}

template <typename ElementType, typename Allocator>
template <typename Alloc,
          typename Element,
          typename std::enable_if<detail_dynamic_array::trivially_initializable_v<Element>, bool>::type>
inline auto DynamicArray<ElementType, Allocator>::initialize_array(size_type number_of_elements, Alloc& allocator)
    -> pointer
{
    if (number_of_elements == 0U)
    {
        return nullptr;
    }
    using alloc_traits = std::allocator_traits<Alloc>;
    auto storage = alloc_traits::allocate(allocator, number_of_elements);
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(nullptr != storage, "no memory allocated");

    const auto number_of_bytes_to_initialise = number_of_elements * sizeof(Element);

    // Using memset to zero-initialize storage for trivially copyable and trivially default constructible types.
    // This is necessary to ensure all elements are properly initialized.
    // NOLINTBEGIN(score-banned-function): see comment above
    // Justification: The check below ensures that calculating number_of_bytes_to_initialise did not overflow.
    // coverity[autosar_cpp14_a4_7_1_violation] deliberate check
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD((number_of_bytes_to_initialise == 0U) ||
                         ((number_of_bytes_to_initialise / sizeof(Element)) == number_of_elements));
    // Therefore, dereferencing "storage" is safe.
    // coverity[autosar_cpp14_a5_3_2_violation] nullptr check above ensures that storage cannot be a nullptr
    // coverity[autosar_cpp14_a12_0_2_violation] memset is allowed for this type (standard-layout or trivially copyable)
    // coverity[autosar_cpp14_a4_7_1_violation] false-positive: precondition prevents data loss scenario
    score::cpp::ignore = std::memset(detail_dynamic_array::to_address(storage), 0, number_of_bytes_to_initialise);
    // NOLINTEND(score-banned-function): see comment above
    return storage;
}

template <typename ElementType, typename Allocator>
// coverity[autosar_cpp14_a15_5_3_violation] only precondition may throw, which is intentional
inline ElementType& DynamicArray<ElementType, Allocator>::at(const size_type index)
{
    // check that this element is in shared memory.
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(index < size_, "index out of bounds");
    // Suppress "AUTOSAR C++14 A9-3-1" rule finding. This rule states: "An object shall not be accessed outside of its
    // lifetime.".
    // Suppress "AUTOSAR C++14 A9-3-1" rule finding. This rule states:"Member functions shall not return
    // non-const “raw” pointers or references to private or protected data owned by the class.".
    // Suppress "AUTOSAR C++14 M7-5-1" rule finding. This rule states: "A function shall not return a reference or a
    // pointer to an automatic variable (including parameters), defined within the function.".
    // Suppress "AUTOSAR C++14 M7-5-2" rule finding. This rule states: "The address of an object with automatic storage
    // shall not be assigned to another object that may persist after the first object has ceased to exist.".
    // Justification: As with a normal container, an underlying item should be accessible as a non-const reference
    // from outside by index. The result reference remains valid as long as the DynamicArray instance is alive.
    // coverity[autosar_cpp14_a3_8_1_violation]
    // coverity[autosar_cpp14_a9_3_1_violation]
    // coverity[autosar_cpp14_m7_5_1_violation]
    // coverity[autosar_cpp14_m7_5_2_violation]
    return *(std::next(dynamic_array_, static_cast<ptr_index_type>(index)));
}

template <typename ElementType, typename Allocator>
inline const ElementType& DynamicArray<ElementType, Allocator>::at(const size_type index) const
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(index < size_, "index out of bounds");
    // Suppress "AUTOSAR C++14 A3-8-1" rule finding.
    // This rule states: "An object shall not be accessed outside of its lifetime.".
    // Suppress "AUTOSAR C++14 M7-5-1" rule finding.
    // This rule states:"A function shall not return a reference or a pointer to an automatic
    // variable (including parameters), defined within the function.".
    // Suppress "AUTOSAR C++14 M7-5-2" rule finding. This rule states: "The address of an object with automatic storage
    // shall not be assigned to another object that may persist after the first object has ceased to exist.".
    // Justification: The index is checked to be within bounds, ensuring the object is within its lifetime;
    // As with a normal container, an underlying item should be accessible by reference from outside by index.
    // coverity[autosar_cpp14_a3_8_1_violation]
    // coverity[autosar_cpp14_m7_5_1_violation]
    // coverity[autosar_cpp14_m7_5_2_violation]
    return *(std::next(dynamic_array_, static_cast<ptr_index_type>(index)));
}

template <typename ElementType, typename Allocator>
inline ElementType& DynamicArray<ElementType, Allocator>::operator[](const size_type index)
{
    return this->at(index);
}

template <typename ElementType, typename Allocator>
inline const ElementType& DynamicArray<ElementType, Allocator>::operator[](const size_type index) const
{
    return this->at(index);
}

template <typename ElementType, typename Allocator>
inline auto DynamicArray<ElementType, Allocator>::data() -> value_type*
{
    // If the DynamicArray is in shared memory, then dynamic_array_ may be corrupted by another process while this
    // function is running. Therefore, we make a stack copy of it at the start of the function and use that copy for all
    // checks and for generating the final iterator. Theoretically, the size_ variable could also be corrupted. However,
    // in that case, the bounds checking will not be affected.
    const auto dynamic_array_copy = dynamic_array_;

    // Certain allocators use pointers which perform bounds checking (e.g. PolymorphicOffsetPtrAllocator and
    // OffsetPtrs). For these, it's important that we can get both the first and last elements of the underlying
    // array without terminating. Since the array is contiguous, then as long as the first and last elements are
    // within the correct bounds, the whole array is.
    score::cpp::ignore = GetLastElement(dynamic_array_copy, size_);
    return GetFirstElement(dynamic_array_copy, size_);
}

template <typename ElementType, typename Allocator>
inline auto DynamicArray<ElementType, Allocator>::data() const -> const value_type*
{
    // If the DynamicArray is in shared memory, then dynamic_array_ may be corrupted by another process while this
    // function is running. Therefore, we make a stack copy of it at the start of the function and use that copy for all
    // checks and for generating the final iterator. Theoretically, the size_ variable could also be corrupted. However,
    // in that case, the bounds checking will not be affected.
    const auto dynamic_array_copy = dynamic_array_;

    // Certain allocators use pointers which perform bounds checking (e.g. PolymorphicOffsetPtrAllocator and
    // OffsetPtrs). For these, it's important that we can get both the first and last elements of the underlying array
    // without terminating. Since the array is contiguous, then as long as the first and last elements are within the
    // correct bounds, the whole array is.
    score::cpp::ignore = GetLastElement(dynamic_array_copy, size_);
    return GetFirstElement(dynamic_array_copy, size_);
}

template <typename ElementType, typename Allocator>
template <typename Alloc,
          typename Element,
          typename std::enable_if<!detail_dynamic_array::trivially_initializable_v<Element>, bool>::type>
void DynamicArray<ElementType, Allocator>::Destruct() noexcept
{
    if (size_ == 0U)
    {
        return;
    }
    using alloc_traits = std::allocator_traits<decltype(alloc_)>;
    auto cur_pointer = dynamic_array_;
    for (size_type i = 0u; i < size_; i++)
    {
        alloc_traits::destroy(alloc_, detail_dynamic_array::to_address(cur_pointer));
        std::advance(cur_pointer, 1);
    }
    alloc_traits::deallocate(alloc_, dynamic_array_, size_);
}

template <typename ElementType, typename Allocator>
template <typename Alloc,
          typename Element,
          typename std::enable_if<detail_dynamic_array::trivially_initializable_v<Element>, bool>::type>
inline void DynamicArray<ElementType, Allocator>::Destruct() noexcept
{
    if (size_ == 0u)
    {
        return;
    }
    using alloc_traits = std::allocator_traits<decltype(alloc_)>;
    alloc_traits::deallocate(alloc_, dynamic_array_, size_);
}

template <typename ElementType, typename Allocator>
auto DynamicArray<ElementType, Allocator>::begin() -> iterator
{
    return this->data();
}

template <typename ElementType, typename Allocator>
auto DynamicArray<ElementType, Allocator>::cbegin() const -> const_iterator
{
    return this->data();
}

template <typename ElementType, typename Allocator>
auto DynamicArray<ElementType, Allocator>::begin() const -> const_iterator
{
    return this->data();
}

template <typename ElementType, typename Allocator>
inline auto DynamicArray<ElementType, Allocator>::end() -> iterator
{
    return GetPastTheEndIterator();
}

template <typename ElementType, typename Allocator>
auto DynamicArray<ElementType, Allocator>::end() const -> const_iterator
{
    return static_cast<const_iterator>(GetPastTheEndIterator());
}

template <typename ElementType, typename Allocator>
auto DynamicArray<ElementType, Allocator>::cend() const -> const_iterator
{
    return static_cast<const_iterator>(GetPastTheEndIterator());
}

template <typename ElementType, typename Allocator>
inline auto DynamicArray<ElementType, Allocator>::GetPastTheEndIterator() const -> iterator
{
    // If the DynamicArray is in shared memory, then dynamic_array_ may be corrupted by another process while this
    // function is running. Therefore, we make a stack copy of it at the start of the function and use that copy for all
    // checks and for generating the final iterator.
    const auto dynamic_array_copy = dynamic_array_;

    // Certain allocators use pointers which perform bounds checking (e.g. PolymorphicOffsetPtrAllocator and
    // OffsetPtrs). For these, it's important that we can get both the first and last elements of the underlying array
    // without terminating. Since the array is contiguous, then as long as the first and last elements are within the
    // correct bounds, the whole array is.
    score::cpp::ignore = GetFirstElement(dynamic_array_copy, size_);

    auto last_element_raw_iterator = GetLastElement(dynamic_array_copy, size_);
    if (last_element_raw_iterator == nullptr)
    {
        return nullptr;
    }
    auto past_the_end_raw_iterator = std::next(last_element_raw_iterator, 1);

    // Suppress "AUTOSAR C++14 A3-8-1" rule finding.
    // This rule states: "An object shall not be accessed outside of its lifetime.".
    // Suppress "AUTOSAR C++14 M7-5-1" rule finding.
    // This rule states: "A function shall not return a reference or a pointer to an automatic
    // variable (including parameters), defined within the function.".
    // Justification: The local variable "past_the_end_pointer" is a pointer to an internal member.
    // This does not cause the object to outlive its intended scope.
    // coverity[autosar_cpp14_a3_8_1_violation]
    // coverity[autosar_cpp14_m7_5_1_violation]
    return ::score::containers::detail_dynamic_array::to_address(past_the_end_raw_iterator);
}

template <typename ElementType, typename Allocator>
// coverity[autosar_cpp14_a0_1_3_violation] false-positive: used in data(), GetPastTheEndIterator()
inline auto DynamicArray<ElementType, Allocator>::GetFirstElement(const pointer dynamic_array,
                                                                  const std::size_t dynamic_array_size) -> iterator
{
    if (dynamic_array_size == 0U)
    {
        return nullptr;
    }
    return ::score::containers::detail_dynamic_array::to_address(dynamic_array);
}

template <typename ElementType, typename Allocator>
// coverity[autosar_cpp14_a0_1_3_violation] false-positive: used in data(), GetPastTheEndIterator()
inline auto DynamicArray<ElementType, Allocator>::GetLastElement(const pointer dynamic_array,
                                                                 const std::size_t dynamic_array_size) -> iterator
{
    if (dynamic_array_size == 0U)
    {
        return nullptr;
    }
    // cast is safe due to 0 check
    // coverity[autosar_cpp14_m5_0_9_violation]
    auto last_element_fancy_pointer = std::next(dynamic_array, static_cast<ptr_index_type>(dynamic_array_size - 1U));
    return ::score::containers::detail_dynamic_array::to_address(last_element_fancy_pointer);
}

}  // namespace score::containers

#endif  // SCORE_LIB_CONTAINERS_DYNAMIC_ARRAY_H
