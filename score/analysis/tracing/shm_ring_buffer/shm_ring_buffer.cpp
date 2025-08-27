///
/// @file shm_ring_buffer.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#include "score/analysis/tracing/shm_ring_buffer/shm_ring_buffer.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"
#include "score/memory/shared/shared_memory_factory.h"
#include <iostream>
#include <thread>

namespace score
{
namespace analysis
{
namespace tracing
{
namespace detail
{

namespace
{
static constexpr std::uint16_t kMax15Bits = 0x7FFFU;
static constexpr std::uint32_t kMaxGetElementRetries = 10U;
// No harm from defining static variables here
// coverity[autosar_cpp14_a3_3_2_violation]
static const std::string kShmRingBufferStatisticsPath{"/dev_shmem_stat"};

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

template <typename T, template <class> class IndirectorType>
using IndirectorUnderlying = IndirectorType<atomic_underlying_type_t<T>>;
}  // namespace

namespace detail
{
/// @brief Check if buffer is full
///
/// @param buffer_state buffer state
///
/// @return True if buffer is full, false otherwise
static bool IsBufferFull(const ShmRingBufferState& buffer_state)
{
    return (buffer_state.start == buffer_state.end) && (buffer_state.empty == 0U);  // 0 means false
}

/// @brief Check if buffer is empty
///
/// @param buffer_state buffer state
///
/// @return True if buffer is empty, false otherwise
static bool IsBufferEmpty(const ShmRingBufferState& buffer_state)
{
    return (buffer_state.start == buffer_state.end) && (buffer_state.empty == 1);
}
}  // namespace detail

/// @brief Check if element is ready to process
///
/// @param buffer Reference to RingBufferElement struct to check
///
/// @return True if buffer is ready to process, false otherwise
template <template <class> class IndirectorType = score::memory::shared::AtomicIndirectorReal>
static bool IsElementReady(const ShmRingBufferElement& element);

/// @brief Check if element is invalid
///
/// @param buffer Reference to RingBufferElement struct to check
///
/// @return True if element is invalid, false otherwise
template <template <class> class IndirectorType = score::memory::shared::AtomicIndirectorReal>
static bool IsElementInvalid(const ShmRingBufferElement& element);

/// @brief Check if element is empty
///
/// @param buffer Reference to RingBufferElement struct to check
///
/// @return True if element is empty, false otherwise
template <template <class> class IndirectorType = score::memory::shared::AtomicIndirectorReal>
static bool IsElementEmpty(const ShmRingBufferElement& element);

template <template <class> class IndirectorType>
ShmRingBufferImpl<IndirectorType>::ShmRingBufferImpl(const std::string& path,
                                                     const std::size_t number_of_elements,
                                                     const bool are_statistics_enabled)
    : IShmRingBuffer(),
      shm_data_{nullptr},
      memory_resource_{nullptr},
      initialized_{false},
      creator_{false},
      size_{0U},
      are_statistics_enabled_{are_statistics_enabled},
      statistics_{nullptr},
      path_{path},
      number_of_elements_{number_of_elements}
{
}

template <template <class> class IndirectorType>
Result<std::uint16_t> ShmRingBufferImpl<IndirectorType>::GetSize() const
{
    if (!initialized_)
    {
        return score::MakeUnexpected(ErrorCode::kRingBufferNotInitializedRecoverable);
    }

    return size_;
}

template <template <class> class AtomicIndirectorType>
// This is intended by design
// NOLINTBEGIN(google-default-arguments) see comment above
score::Result<Blank> ShmRingBufferImpl<AtomicIndirectorType>::CreateOrOpen(bool is_owner)
// NOLINTEND(google-default-arguments) see comment above
{
    score::memory::shared::SharedMemoryFactory::WorldWritable permissions{};
    // Rationale: False positive, variable is used.
    // coverity[autosar_cpp14_a0_1_1_violation]
    bool is_buffer_empty = true;
    bool shall_create = false;
    memory_resource_ = score::memory::shared::SharedMemoryFactory::Open(path_, true);
    if (memory_resource_ != nullptr)
    {
        auto res = Open();
        if (!res.has_value())
        {
            // clang-format off
            std::cerr << __func__ << ": " << "Failed to Open Ring Buffer library ";  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)tolerated
            // clang-format on
            return score::MakeUnexpected<Blank>(res.error());
        }
        is_buffer_empty = IsBufferEmpty();
    }
    else
    {
        if (!is_owner)
        {
            shall_create = true;
        }
    }
    if (is_owner && (memory_resource_ == nullptr || !is_buffer_empty))
    {
        score::memory::shared::SharedMemoryFactory::Remove(path_);
        memory_resource_.reset();
        initialized_ = false;
        shall_create = true;
    }

    if (shall_create)
    {
        memory_resource_ = score::memory::shared::SharedMemoryFactory::Create(
            path_,  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) tolerated
            [](auto&&) {},
            kRingBufferSharedMemorySize,
            {permissions});
        if (memory_resource_ == nullptr)
        {
            // clang-format off
            std::cerr << __func__ << ": " << "Failed to create Ring Buffer shared memory"<<std::endl;  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)tolerated
            // clang-format on
            return score::MakeUnexpected(ErrorCode::kRingBufferInvalidMemoryResourceRecoverable);
        }
        auto result = Create();
        if (!result.has_value())
        {
            // clang-format off
            std::cerr << __func__ << ": " << "Failed to Create Ring Buffer library";  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)tolerated
            // clang-format on
            return score::MakeUnexpected<Blank>(result.error());
        }
    }
    return {};
}

template <template <class> class AtomicIndirectorType>
// This is intended by design
// NOLINTBEGIN(google-default-arguments) see comment above
void ShmRingBufferImpl<AtomicIndirectorType>::Reset()
// NOLINTEND(google-default-arguments) see comment above
{
    shm_data_->use_count_--;
    /*Only one instance is opening ring buffer, so we can safely reset it, note that reset is called once
       LtpmDaemon is disconnected. */
    if (shm_data_->use_count_ == 1)  // LCOV_EXCL_BR_LINE false positive as decision true is taken (tool issue).
    {
        for (auto& element : shm_data_->vector_)
        {
            element.status_.store(TraceJobStatus::kEmpty, std::memory_order_release);
        }
        auto current_state = IndirectorUnderlying<decltype(shm_data_->state_), AtomicIndirectorType>::load(
            shm_data_->state_, std::memory_order_acquire);
        constexpr ShmRingBufferState new_state{1U, static_cast<std::uint16_t>(0U), static_cast<std::uint16_t>(0U)};
        for (std::uint8_t retry_counter = 0U; retry_counter < kMaxGetElementRetries; ++retry_counter)
        {
            if (!TryUpdateStateAtomically(current_state, new_state))
            {
                continue;
            }
        }
        ResetStatistics();
    }
}

template <template <class> class AtomicIndirectorType>
// This is intended by design
// NOLINTBEGIN(google-default-arguments) see comment above
void ShmRingBufferImpl<AtomicIndirectorType>::Close()
// NOLINTEND(google-default-arguments) see comment above
{
    initialized_ = false;
    // Only last client do the unlink.
    if (shm_data_->use_count_ == 1)
    {
        // Unlink the RingBuffer shared memory object
        // NOLINTBEGIN(hicpp-no-array-decay, cppcoreguidelines-pro-bounds-array-to-pointer-decay): tolerated
        memory::shared::SharedMemoryFactory::Remove(kShmRingBufferStatisticsPath);
        memory::shared::SharedMemoryFactory::Remove(kRingBufferSharedMemoryPath);
        // NOLINTEND(hicpp-no-array-decay, cppcoreguidelines-pro-bounds-array-to-pointer-decay): tolerated
    }
    shm_data_->use_count_--;
    memory_resource_.reset();
    shm_data_ = nullptr;
    creator_ = false;
    size_ = kMax15Bits + 1;
    memory_resource_statistics_.reset();
    statistics_ = nullptr;
}

template <template <class> class IndirectorType>
score::Result<std::uint32_t> ShmRingBufferImpl<IndirectorType>::GetUseCount()
{
    if (!initialized_)
    {
        return score::MakeUnexpected(ErrorCode::kRingBufferNotInitializedRecoverable);
    }

    ShmRingBufferState current_state = IndirectorUnderlying<decltype(shm_data_->state_), IndirectorType>::load(
        shm_data_->state_, std::memory_order_acquire);
    if (!IsValidState(current_state))
    {
        return score::MakeUnexpected(ErrorCode::kRingBufferInvalidStateRecoverable);
    }

    if (static_cast<bool>(current_state.empty))
    {
        return 0;
    }

    return static_cast<std::uint32_t>(current_state.end) > static_cast<std::uint32_t>(current_state.start)
               ? static_cast<std::uint32_t>(current_state.end) - static_cast<std::uint32_t>(current_state.start)
               : static_cast<std::uint32_t>(
                     static_cast<std::uint32_t>(size_) -
                     (static_cast<std::uint32_t>(current_state.start) - static_cast<std::uint32_t>(current_state.end)));
}

template <template <class> class IndirectorType>
Result<Blank> ShmRingBufferImpl<IndirectorType>::Open()
{
    if ((nullptr == memory_resource_) || (nullptr == memory_resource_->getUsableBaseAddress()))
    {
        return MakeUnexpected(ErrorCode::kRingBufferInvalidMemoryResourceRecoverable);
    }
    // clang-format off
    // Usage of reinterpret cast is intended here and as per our use case, type safety is not violated
    // As the memory is accessible only within the usage of this library which ensures that this memory
    // would be always translated in the same way
    //  coverity[autosar_cpp14_a5_2_4_violation]
    //  coverity[autosar_cpp14_m5_2_8_violation]
    shm_data_ = reinterpret_cast<ShmDataSegment*>(memory_resource_->getUsableBaseAddress());  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast): Tolerated see above
    // clang-format on

    if (are_statistics_enabled_)
    {
        const auto statistics_result = CreateOrOpenStatistics();
        if (!statistics_result.has_value())
        {
            return MakeUnexpected<Blank>(statistics_result.error());
        }
    }
    // Cast is safe as maximum allowed size of vector must fit within 15-bit
    // No possibility for having data loss here
    // coverity[autosar_cpp14_a4_7_1_violation]
    size_ = static_cast<std::uint16_t>(shm_data_->vector_.size());
    initialized_ = true;
    shm_data_->use_count_++;
    return Blank{};
}

template <template <class> class IndirectorType>
Result<Blank> ShmRingBufferImpl<IndirectorType>::Create()
{
    if (number_of_elements_ > kMax15Bits)
    {
        return MakeUnexpected(ErrorCode::kRingBufferTooLargeRecoverable);
    }
    if ((nullptr == memory_resource_) || (nullptr == memory_resource_->getMemoryResourceProxy()))
    {
        return MakeUnexpected(ErrorCode::kRingBufferInvalidMemoryResourceRecoverable);
    }
    auto shm_data_raw_ptr = memory_resource_->allocate(sizeof(ShmDataSegment), alignof(std::max_align_t));
    // placement "new" is intended here, to allocate the created data segment in the specified
    // address in shared memory instead of heap
    // NOLINTBEGIN(score-no-dynamic-raw-memory): Tolerated see above
    // coverity[autosar_cpp14_a15_1_4_violation]
    // coverity[autosar_cpp14_a15_0_2_violation]
    // coverity[autosar_cpp14_a18_5_10_violation]
    shm_data_ = new (shm_data_raw_ptr) ShmDataSegment{number_of_elements_, memory_resource_->getMemoryResourceProxy()};
    // NOLINTEND(score-no-dynamic-raw-memory): Tolerated see above
    if (are_statistics_enabled_)
    {
        const auto statistics_result = CreateOrOpenStatistics();
        if (!statistics_result.has_value())
        {
            return MakeUnexpected<Blank>(statistics_result.error());
        }
    }
    // Cast is safe as maximum allowed size of vector must fit within 15-bit
    // No possibility for having data loss here
    // coverity[autosar_cpp14_a4_7_1_violation]
    size_ = static_cast<std::uint16_t>(shm_data_->vector_.size());
    initialized_ = true;
    creator_ = true;
    shm_data_->use_count_++;
    return Blank{};
}

template <template <class> class IndirectorType>
Result<Blank> ShmRingBufferImpl<IndirectorType>::CreateOrOpenStatistics() noexcept
{
    memory::shared::SharedMemoryFactory::WorldWritable permissions{};
    memory_resource_statistics_ = memory::shared::SharedMemoryFactory::Open(kShmRingBufferStatisticsPath, true);
    if (memory_resource_statistics_ != nullptr)
    {
        if (memory_resource_statistics_->getUsableBaseAddress() == nullptr)
        {
            return MakeUnexpected(ErrorCode::kRingBufferInvalidMemoryResourceRecoverable);
        }

        // clang-format off
        // Usage of reinterpret cast is intended here and as per our use case, type safety is not violated
        // As the memory is accessible only within the usage of this library which ensures that this memory
        // would be always translated in the same way
        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast): Tolerated see above
        // coverity[autosar_cpp14_a5_2_4_violation]
        // coverity[autosar_cpp14_m5_2_8_violation]
        statistics_ = reinterpret_cast<ShmRingBufferAtomicStatistics*>(memory_resource_statistics_->getUsableBaseAddress());
        // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast): Tolerated see above
        // clang-format on
    }
    else
    {
        memory::shared::SharedMemoryFactory::Remove(kShmRingBufferStatisticsPath);
        memory_resource_statistics_.reset();
        memory_resource_statistics_ = memory::shared::SharedMemoryFactory::Create(
            kShmRingBufferStatisticsPath, [](auto&&) {}, sizeof(ShmRingBufferAtomicStatistics), {permissions});
        if (memory_resource_statistics_ == nullptr)
        {
            return MakeUnexpected(ErrorCode::kRingBufferInvalidMemoryResourceRecoverable);
        }

        // clang-format off
        // coverity[autosar_cpp14_a15_1_4_violation]
        // coverity[autosar_cpp14_a15_0_2_violation]
        // coverity[autosar_cpp14_a15_4_2_violation]
        auto statistics_raw_ptr = memory_resource_statistics_->allocate(sizeof(ShmRingBufferAtomicStatistics), alignof(std::max_align_t));
        // clang-format on

        // placement "new" is intended here, to allocate the created data segment in the specified
        // address in shared memory instead of heap
        // NOLINTBEGIN(score-no-dynamic-raw-memory): Tolerated see above
        // coverity[autosar_cpp14_a15_1_4_violation]
        // coverity[autosar_cpp14_a15_0_2_violation]
        // coverity[autosar_cpp14_a18_5_10_violation]
        statistics_ = new (statistics_raw_ptr) ShmRingBufferAtomicStatistics{};
        // NOLINTEND(score-no-dynamic-raw-memory): Tolerated see above
    }
    return {};
}

template <template <class> class IndirectorType>
ElementReferenceType ShmRingBufferImpl<IndirectorType>::GetEmptyElement()
{
    if (!initialized_)
    {
        return score::MakeUnexpected(ErrorCode::kRingBufferNotInitializedRecoverable);
    }

    if (are_statistics_enabled_)
    {
        IncrementMetric(statistics_->producer_.call_count_, 1U);
    }

    std::uint64_t local_cas_trials = 0U;
    std::uint64_t local_cas_failures = 0U;
    for (std::uint8_t retry_counter = 0U; retry_counter < kMaxGetElementRetries; ++retry_counter)
    {
        auto current_state = IndirectorUnderlying<decltype(shm_data_->state_), IndirectorType>::load(
            shm_data_->state_, std::memory_order_acquire);
        if (!IsValidState(current_state))
        {
            if (are_statistics_enabled_)
            {
                IncrementMetric(statistics_->producer_.call_failure_count_, 1U);
                IncrementMetric(statistics_->producer_.cas_trials_, local_cas_trials);
                IncrementMetric(statistics_->producer_.cas_failures_, local_cas_failures);
            }
            return score::MakeUnexpected(ErrorCode::kRingBufferInvalidStateRecoverable);
        }

        if (detail::IsBufferFull(current_state))
        {
            if (are_statistics_enabled_)
            {
                IncrementMetric(statistics_->producer_.buffer_full_count_, 1U);
                IncrementMetric(statistics_->producer_.cas_trials_, local_cas_trials);
                IncrementMetric(statistics_->producer_.cas_failures_, local_cas_failures);
            }
            return score::MakeUnexpected(ErrorCode::kRingBufferFullRecoverable);
        }

        const std::uint16_t previous_end_index = current_state.end;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index): bounds already checked
        auto& element = shm_data_->vector_[static_cast<std::size_t>(previous_end_index)];
        if (!IsElementEmpty(element))  // LCOV_EXCL_BR_LINE false positive as decision true is taken (tool issue).
        {
            continue;
        }

        const auto new_end_index = static_cast<std::uint16_t>((previous_end_index + 1U) % size_);
        const ShmRingBufferState new_state{0U,
                                           static_cast<std::uint16_t>(current_state.start & kMax15Bits),
                                           static_cast<std::uint16_t>(new_end_index & kMax15Bits)};

        if (are_statistics_enabled_)
        {
            // No possibility for having data loss here
            // coverity[autosar_cpp14_a4_7_1_violation]
            ++local_cas_trials;
        }
        if (!TryUpdateStateAtomically(current_state, new_state))
        {
            if (are_statistics_enabled_)
            {
                // No possibility for having data loss here
                // coverity[autosar_cpp14_a4_7_1_violation]
                ++local_cas_failures;
            }
            continue;
        }

        IndirectorUnderlying<decltype(element.status_), IndirectorType>::store(
            element.status_, TraceJobStatus::kAllocated, std::memory_order_release);
        if (are_statistics_enabled_)
        {
            IncrementMetric(statistics_->producer_.cas_trials_, local_cas_trials);
            IncrementMetric(statistics_->producer_.cas_failures_, local_cas_failures);
        }
        return element;
    }

    if (are_statistics_enabled_)
    {
        IncrementMetric(statistics_->producer_.call_failure_count_, 1U);
        IncrementMetric(statistics_->producer_.cas_trials_, local_cas_trials);
        IncrementMetric(statistics_->producer_.cas_failures_, local_cas_failures);
    }
    return score::MakeUnexpected(ErrorCode::kRingBufferNoEmptyElementRecoverable);
}

template <template <class> class IndirectorType>
ElementReferenceType ShmRingBufferImpl<IndirectorType>::GetReadyElement()
{
    if (!initialized_)
    {
        return score::MakeUnexpected(ErrorCode::kRingBufferNotInitializedRecoverable);
    }
    return TryFetchElement();
}

// fetching an element
template <template <class> class IndirectorType>
ElementReferenceType ShmRingBufferImpl<IndirectorType>::TryFetchElement()
{
    if (!initialized_)
    {
        return score::MakeUnexpected(ErrorCode::kRingBufferNotInitializedRecoverable);
    }

    if (are_statistics_enabled_)
    {
        // No possibility for having data loss here
        // coverity[autosar_cpp14_a4_7_1_violation]
        ++statistics_->consumer_.call_count_;
    }
    std::uint64_t local_cas_trials = 0U;
    std::uint64_t local_cas_failures = 0U;
    for (std::uint8_t retry_counter = 0U; retry_counter < kMaxGetElementRetries; ++retry_counter)
    {
        auto current_state = IndirectorUnderlying<decltype(shm_data_->state_), IndirectorType>::load(
            shm_data_->state_, std::memory_order_acquire);
        if (!IsValidState(current_state))
        {
            if (are_statistics_enabled_)
            {
                // No possibility for having data loss here
                // coverity[autosar_cpp14_a4_7_1_violation]
                ++statistics_->consumer_.call_failure_count_;
                // No possibility for having data loss here
                // coverity[autosar_cpp14_a4_7_1_violation]
                statistics_->consumer_.cas_trials_ += local_cas_trials;
                // No possibility for having data loss here
                // coverity[autosar_cpp14_a4_7_1_violation]
                statistics_->consumer_.cas_failures_ += local_cas_failures;
            }
            return score::MakeUnexpected(ErrorCode::kRingBufferInvalidStateRecoverable);
        }

        if (detail::IsBufferEmpty(current_state))
        {
            if (are_statistics_enabled_)
            {
                // No possibility for having data loss here
                // coverity[autosar_cpp14_a4_7_1_violation]
                ++statistics_->consumer_.buffer_empty_count_;
                // No possibility for having data loss here
                // coverity[autosar_cpp14_a4_7_1_violation]
                statistics_->consumer_.cas_trials_ += local_cas_trials;
                // No possibility for having data loss here
                // coverity[autosar_cpp14_a4_7_1_violation]
                statistics_->consumer_.cas_failures_ += local_cas_failures;
            }
            return score::MakeUnexpected(ErrorCode::kRingBufferEmptyRecoverable);
        }

        const auto start_index = current_state.start;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index): bounds already checked
        auto& element = shm_data_->vector_[static_cast<std::size_t>(start_index)];
        const ShmRingBufferState new_state = CreateNewState(current_state, start_index);
        if (IsElementReady(element))
        {
            if (are_statistics_enabled_)
            {
                // No possibility for having data loss here
                // coverity[autosar_cpp14_a4_7_1_violation]
                ++local_cas_trials;
            }
            if (!TryUpdateStateAtomically(current_state, new_state))
            {
                if (are_statistics_enabled_)
                {
                    // No possibility for having data loss here
                    // coverity[autosar_cpp14_a4_7_1_violation]
                    ++local_cas_failures;
                }
                continue;
            }

            if (are_statistics_enabled_)
            {
                // No possibility for having data loss here
                // coverity[autosar_cpp14_a4_7_1_violation]
                statistics_->consumer_.cas_trials_ += local_cas_trials;
                // No possibility for having data loss here
                // coverity[autosar_cpp14_a4_7_1_violation]
                statistics_->consumer_.cas_failures_ += local_cas_failures;
            }
            return element;
        }
        else if (IsElementInvalid(element))
        {
            if (are_statistics_enabled_)
            {
                // No possibility for having data loss here
                // coverity[autosar_cpp14_a4_7_1_violation]
                ++local_cas_trials;
            }
            if (!TryUpdateStateAtomically(current_state, new_state))
            {
                if (are_statistics_enabled_)
                {
                    // No possibility for having data loss here
                    // coverity[autosar_cpp14_a4_7_1_violation]
                    ++local_cas_failures;
                }
                continue;
            }
            element.status_.store(TraceJobStatus::kEmpty, std::memory_order_release);
        }
    }

    if (are_statistics_enabled_)
    {
        // No possibility for having data loss here
        // coverity[autosar_cpp14_a4_7_1_violation]
        ++statistics_->consumer_.call_failure_count_;
        // No possibility for having data loss here
        // coverity[autosar_cpp14_a4_7_1_violation]
        statistics_->consumer_.cas_trials_ += local_cas_trials;
        // No possibility for having data loss here
        // coverity[autosar_cpp14_a4_7_1_violation]
        statistics_->consumer_.cas_failures_ += local_cas_failures;
    }
    return score::MakeUnexpected(ErrorCode::kRingBufferNoReadyElementRecoverable);
}

// Helper function to validate state
template <template <class> class IndirectorType>
bool ShmRingBufferImpl<IndirectorType>::IsValidState(const ShmRingBufferState& state) const
{
    return (state.start < size_) && (state.end < size_);
}

// Helper function to create a new state based on the current state and index
template <template <class> class IndirectorType>
ShmRingBufferState ShmRingBufferImpl<IndirectorType>::CreateNewState(const ShmRingBufferState& current_state,
                                                                     std::uint16_t start_index) const
{
    const auto new_start_index = static_cast<std::uint16_t>((start_index + 1U) % size_);
    // Casting is intended here and will not increase the size of the underlying type of a cvalue expression.
    //  coverity[autosar_cpp14_m5_0_8_violation]
    return {static_cast<std::uint8_t>((new_start_index & kMax15Bits) == (current_state.end & kMax15Bits)),
            static_cast<std::uint16_t>(new_start_index & kMax15Bits),
            static_cast<std::uint16_t>(current_state.end & kMax15Bits)};
}

// Helper function to attempt a state update
template <template <class> class IndirectorType>
bool ShmRingBufferImpl<IndirectorType>::TryUpdateStateAtomically(ShmRingBufferState& current_state,
                                                                 const ShmRingBufferState& new_state) const
{
    return IndirectorUnderlying<decltype(shm_data_->state_), IndirectorType>::compare_exchange_weak(
        shm_data_->state_, current_state, new_state, std::memory_order_acq_rel);
}

template <template <class> class IndirectorType>
bool ShmRingBufferImpl<IndirectorType>::IsBufferEmpty() const
{
    auto buffer_state = IndirectorUnderlying<decltype(shm_data_->state_), IndirectorType>::load(
        shm_data_->state_, std::memory_order_acquire);
    return (buffer_state.start == buffer_state.end) && (buffer_state.empty == 1);
}

template <template <class> class IndirectorType>
Result<ShmRingBufferStatistics> ShmRingBufferImpl<IndirectorType>::GetStatistics() const
{
    if (!are_statistics_enabled_)
    {
        return score::MakeUnexpected(ErrorCode::kGenericErrorRecoverable);
    }

    if (!initialized_)
    {
        return score::MakeUnexpected(ErrorCode::kRingBufferNotInitializedRecoverable);
    }

    // False positive as we here use initialization list.
    // coverity[autosar_cpp14_m8_5_2_violation]
    ProducerStatistics<std::uint64_t> producer{};
    producer.cas_trials_ = IndirectorUnderlying<decltype(statistics_->producer_.cas_trials_), IndirectorType>::load(
        statistics_->producer_.cas_trials_, std::memory_order_acquire);
    producer.cas_failures_ = IndirectorUnderlying<decltype(statistics_->producer_.cas_failures_), IndirectorType>::load(
        statistics_->producer_.cas_failures_, std::memory_order_acquire);
    producer.call_count_ = IndirectorUnderlying<decltype(statistics_->producer_.call_count_), IndirectorType>::load(
        statistics_->producer_.call_count_, std::memory_order_acquire);
    producer.call_failure_count_ =
        IndirectorUnderlying<decltype(statistics_->producer_.call_failure_count_), IndirectorType>::load(
            statistics_->producer_.call_failure_count_, std::memory_order_acquire);
    producer.buffer_full_count_ =
        IndirectorUnderlying<decltype(statistics_->producer_.buffer_full_count_), IndirectorType>::load(
            statistics_->producer_.buffer_full_count_, std::memory_order_acquire);

    // False positive as we here use initialization list.
    // coverity[autosar_cpp14_m8_5_2_violation]
    ConsumerStatistics<std::uint64_t> consumer{};
    consumer.cas_trials_ = statistics_->consumer_.cas_trials_;
    consumer.cas_failures_ = statistics_->consumer_.cas_failures_;
    consumer.call_count_ = statistics_->consumer_.call_count_;
    consumer.call_failure_count_ = statistics_->consumer_.call_failure_count_;
    consumer.buffer_empty_count_ = statistics_->consumer_.buffer_empty_count_;

    return ShmRingBufferStatistics(producer, consumer);
}

template <template <class> class IndirectorType>
void ShmRingBufferImpl<IndirectorType>::ResetStatistics()
{
    if (statistics_ != nullptr)
    {
        AtomicReset(statistics_->producer_.cas_trials_);
        AtomicReset(statistics_->producer_.cas_failures_);
        AtomicReset(statistics_->producer_.call_count_);
        AtomicReset(statistics_->producer_.call_failure_count_);
        AtomicReset(statistics_->producer_.buffer_full_count_);
        statistics_->consumer_.cas_trials_ = 0;
        statistics_->consumer_.cas_failures_ = 0;
        statistics_->consumer_.call_count_ = 0;
        statistics_->consumer_.call_failure_count_ = 0;
        statistics_->consumer_.buffer_empty_count_ = 0;
    }
}
template <template <class> class IndirectorType>
void ShmRingBufferImpl<IndirectorType>::IncrementMetric(std::atomic<std::uint64_t>& atomic_metric,
                                                        const std::uint64_t update)
{
    std::ignore = IndirectorUnderlying<decltype(atomic_metric), IndirectorType>::fetch_add(
        atomic_metric, update, std::memory_order_relaxed);
}

template <template <class> class IndirectorType>
void ShmRingBufferImpl<IndirectorType>::AtomicReset(std::atomic<std::uint64_t>& atomic_metric)
{
    IndirectorUnderlying<decltype(atomic_metric), IndirectorType>::store(atomic_metric, 0u, std::memory_order_relaxed);
}

template <template <class> class IndirectorType>
static bool IsElementReady(const ShmRingBufferElement& element)
{
    return IndirectorUnderlying<decltype(element.status_), IndirectorType>::load(
               element.status_, std::memory_order_acquire) == TraceJobStatus::kReady;
}

template <template <class> class IndirectorType>
static bool IsElementInvalid(const ShmRingBufferElement& element)
{
    return IndirectorUnderlying<decltype(element.status_), IndirectorType>::load(
               element.status_, std::memory_order_acquire) == TraceJobStatus::kInvalid;
}

template <template <class> class IndirectorType>
static bool IsElementEmpty(const ShmRingBufferElement& element)
{
    return IndirectorUnderlying<decltype(element.status_), IndirectorType>::load(
               element.status_, std::memory_order_acquire) == TraceJobStatus::kEmpty;
}

template class ShmRingBufferImpl<score::memory::shared::AtomicIndirectorReal>;
template class ShmRingBufferImpl<score::memory::shared::AtomicIndirectorMock>;

}  // namespace detail
}  // namespace tracing
}  // namespace analysis
}  // namespace score
