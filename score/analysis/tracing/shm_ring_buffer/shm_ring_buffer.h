///
/// @file shm_ring_buffer.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#ifndef GENERIC_TRACE_API_RING_BUFFER_H
#define GENERIC_TRACE_API_RING_BUFFER_H

#include "score/analysis/tracing/shm_ring_buffer/i_shm_ring_buffer.h"
#include "score/analysis/tracing/shm_ring_buffer/shm_data_segment.h"
#include "score/analysis/tracing/shm_ring_buffer/shm_ring_buffer_element.h"
#include "score/memory/shared/atomic_indirector.h"
#include "score/memory/shared/managed_memory_resource.h"
#include "score/result/result.h"
#include <atomic>
#include <cstdint>
#include <memory>

namespace score
{
namespace analysis
{
namespace tracing
{
namespace detail
{

using ElementReferenceType = score::Result<std::reference_wrapper<ShmRingBufferElement>>;

/// @brief ShmRingBuffer class
///
/// Ring buffer that keeps data in shared-memory and allows to read and write elements from different processes.
template <template <class> class IndirectorType = score::memory::shared::AtomicIndirectorReal>
class ShmRingBufferImpl : public IShmRingBuffer
{
  public:
    /// @brief Constructor. Create a new ShmRingBuffer object that will operate in shared memory.
    ///
    /// @param memory_resource shared ptr with shared-memory resource containing state and vector.
    explicit ShmRingBufferImpl(const std::string& path,
                               const std::size_t number_of_elements,
                               const bool are_statistics_enabled = true);

    /// @brief Default destructor. Destroys a ShmRingBuffer object.
    // No harm to define default destructor
    //  coverity[autosar_cpp14_a10_3_1_violation]
    ~ShmRingBufferImpl() = default;

    ShmRingBufferImpl(const ShmRingBufferImpl&) = delete;
    ShmRingBufferImpl& operator=(const ShmRingBufferImpl&) = delete;
    ShmRingBufferImpl(ShmRingBufferImpl&&) = delete;
    ShmRingBufferImpl& operator=(ShmRingBufferImpl&&) = delete;

    /// @brief Create or open shared memory allocated for ring buffer then create or open the ring buffer itself
    ///
    /// @param path shared memory path
    /// @param is_owner flag to indicate if owner is doing the request or not
    ///
    /// @return Error code in the case of error
    // This is intended by design
    // NOLINTBEGIN(google-default-arguments) see comment above
    score::Result<Blank> CreateOrOpen(bool is_owner = false) override;
    // NOLINTEND(google-default-arguments) see comment above

    /// @brief Get empty element to write data
    ///
    /// @return Reference to element that is available to be written to or error code if queue is full or element
    /// couldn't be obtained.
    ///
    /// @details Method return element that can be used to store data. If the ring buffer is full it will return error
    /// code. If operation fails a preset number of times then error code will be returned.
    ElementReferenceType GetEmptyElement() override;

    /// @brief Get ready to process element
    ///
    /// @return Reference to first element in the queue that is ready to process or error code if queue is empty or
    /// element couldn't be obtained.
    ///
    /// @details Method return previously stored element that can be processed. If the ring buffer is empty it will
    /// return error code. In the case that element is already stored but not yet ready to process then or operation of
    /// atomic access fails then it retries to get access a preset number of times before returning an error. If
    /// obtained element is marked as invalid (due to some unrecoverable problems during trace job allocation) then this
    /// method will omit this element, update indexes, mark it as empty and then retry to get next empty element.
    ElementReferenceType GetReadyElement() override;

    // Declares helper functions for reducing cyclomatic complexity
    //  function to fetch an element
    ElementReferenceType TryFetchElement();

    /// @brief Get count of used circular buffer entries
    ///
    /// @return Count of used circular buffer entries.
    ///
    /// @details Method atomically fetch ShmDataSegment statistics and calculates it's usage.
    score::Result<std::uint32_t> GetUseCount() override;

    // Helper function to validate state
    bool IsValidState(const ShmRingBufferState& state) const;

    // Helper function to create a new state based on the current state and index
    ShmRingBufferState CreateNewState(const ShmRingBufferState& current_state, std::uint16_t start_index) const;

    // Helper function to attempt a state update
    bool TryUpdateStateAtomically(ShmRingBufferState& current_state, const ShmRingBufferState& new_state) const;

    // Helper function to check if the SHM Ring Buffer is empty. Used for testing purposes
    bool IsBufferEmpty() const override;

    // Helper function to atomically fetch the current CAS and general trials and failures upon the
    // GetReadyElement(consumer-side) and GetEmptyElement(producer-side) operations
    Result<ShmRingBufferStatistics> GetStatistics() const override;

    // Helper function to atomically reset statistics values to zero.
    void ResetStatistics() override;

    /// @brief Get the size of the circular ring buffer
    Result<std::uint16_t> GetSize() const override;

    /// @brief Reset ShmRingBuffer Members to be able to OpenOrCreate it again with empty elements, but it will only
    // Reset RingBuffer elements unless shm is opened only by one instance
    void Close() override;

    /// @brief Mark all ring buffer Elements as empty.
    void Reset() override;

  private:
    /// @brief Open ShmRingBuffer that was previously created
    ///
    /// @return Error code in the case of error
    score::Result<Blank> Open() override;

    /// @brief Create ShmRingBuffer in shared memory
    ///
    /// @param number_of_elements Number of elements in ring buffer
    ///
    /// @return Error code in the case of error
    score::Result<Blank> Create() override;
    void IncrementMetric(std::atomic<std::uint64_t>& atomic_metric, const std::uint64_t update);
    void AtomicReset(std::atomic<std::uint64_t>& atomic_metric);
    score::Result<Blank> CreateOrOpenStatistics() noexcept;

    ShmDataSegment* shm_data_;         ///< Pointer to the shared-memory data segment
    ResourcePointer memory_resource_;  ///< Shared-memory resource where data segment is kept
    bool initialized_;                 ///< Initialization finished flag
    bool creator_;                     ///< Creator flag
    std::uint16_t size_;               ///< Size of ring buffer

    const bool are_statistics_enabled_;
    ResourcePointer memory_resource_statistics_;  ///< Shared-memory resource where statistics are kept
    ShmRingBufferAtomicStatistics* statistics_;   ///< Pointer to the shared-memory that stores the statistics
    std::string path_;
    std::size_t number_of_elements_;
};

}  // namespace detail

using ShmRingBuffer = detail::ShmRingBufferImpl<>;

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_RING_BUFFER_H
