///
/// @file i_shm_ring_buffer.h
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#ifndef SCORE_ANALYSIS_TRACING_SHM_RING_BUFFER_I_SHM_RING_BUFFER
#define SCORE_ANALYSIS_TRACING_SHM_RING_BUFFER_I_SHM_RING_BUFFER

#include "score/analysis/tracing/shm_ring_buffer/shm_data_segment.h"
#include "score/analysis/tracing/shm_ring_buffer/shm_ring_buffer_element.h"
#include "score/analysis/tracing/shm_ring_buffer/shm_ring_buffer_statistics.h"
#include "score/memory/shared/atomic_indirector.h"
#include "score/memory/shared/managed_memory_resource.h"
#include "score/result/result.h"

#include <atomic>
#include <cstdint>
#include <memory>
#include <optional>

namespace score
{
namespace analysis
{
namespace tracing
{

using ElementReferenceType = score::Result<std::reference_wrapper<ShmRingBufferElement>>;

/// @brief ShmRingBuffer class
///
/// Ring buffer that keeps data in shared-memory and allows to read and write elements from different processes.
class IShmRingBuffer
{
  public:
    /// @brief Default constructor. Destroys a ShmRingBuffer object.
    IShmRingBuffer() = default;

    /// @brief Default copy op. Destroys a ShmRingBuffer object.
    IShmRingBuffer(const IShmRingBuffer&) = delete;

    /// @brief Default copy op. Destroys a ShmRingBuffer object.
    IShmRingBuffer& operator=(const IShmRingBuffer&) = delete;

    /// @brief Default move op. Destroys a ShmRingBuffer object.
    IShmRingBuffer(IShmRingBuffer&&) = delete;

    /// @brief Default move op. Destroys a ShmRingBuffer object.
    IShmRingBuffer& operator=(IShmRingBuffer&&) = delete;

    /// @brief Default destructor. Destroys a ShmRingBuffer object.
    virtual ~IShmRingBuffer() = default;

    /// @brief Open ShmRingBuffer that was previously created
    ///
    /// @return Error code in the case of error
    virtual score::Result<Blank> Open() = 0;

    /// @brief Create ShmRingBuffer in shared memory
    ///
    /// @param number_of_elements Number of elements in ring buffer
    ///
    /// @return Error code in the case of error
    virtual score::Result<Blank> Create() = 0;

    /// @brief Create or open shared memory allocated for ring buffer then create or open the ring buffer itself
    ///
    /// @param path shared memory path
    /// @param number_of_elements Number of elements in ring buffer
    /// @param is_owner flag to indicate if owner is doing the request or not
    ///
    /// @return Error code in the case of error
    // This is intended by design
    // NOLINTBEGIN(google-default-arguments) see comment above
    virtual score::Result<Blank> CreateOrOpen(bool is_owner = false) = 0;
    // NOLINTEND(google-default-arguments) see comment above

    /// @brief Get empty element to write data
    ///
    /// @return Reference to element that is available to be written to or error code if queue is full or element
    /// couldn't be obtained.
    ///
    /// @details Method return element that can be used to store data. If the ring buffer is full it will return error
    /// code. If operation fails a preset number of times then error code will be returned.
    virtual ElementReferenceType GetEmptyElement() = 0;

    /// @brief Get ready to process element
    ///
    /// @return Reference to first element in the queue that is ready to process or error code if queue is empty or
    /// element couldn't be obtained.
    ///
    /// @details Method return previously stored element that can be processed. If the ring buffer is empty it will
    /// return error code. In the case that element is already stored but not yet ready to process then or operation of
    /// atomic access fails then it retries to get access a preset number of times before returning an error.
    virtual ElementReferenceType GetReadyElement() = 0;

    /// @brief Get count of used circular buffer entries
    ///
    /// @return Count of used circular buffer entries.
    ///
    /// @details Method atomically fetch ShmDataSegment statistics and calculates it's usage.
    virtual score::Result<std::uint32_t> GetUseCount() = 0;

    /// @brief Helper function to atomically fetch the current CAS trials and failures upon the
    /// GetReadyElement(comsumer-side) and GetEmptyElement(producer-side) operations
    ///
    /// @return the trials and failures of the GetReadyElement and GetEmptyElement operations
    virtual Result<ShmRingBufferStatistics> GetStatistics() const = 0;

    /// @brief Helper function to atomically reset statistics values to zero.
    ///
    /// @return void
    virtual void ResetStatistics() = 0;

    /// @brief Get the size of the circular ring buffer
    ///
    /// @return size of the ring buffer, if the ring buffer is initialized, an error otherwise
    virtual Result<std::uint16_t> GetSize() const = 0;

    virtual bool IsBufferEmpty() const = 0;

    /// @brief Helper function reset ring buffer members and clean all elements also removing the mapped memory.
    ///
    /// @return void
    virtual void Close() = 0;

    /// @brief Mark all ring buffer Elements as empty and reset the status of the ring buffer.
    virtual void Reset() = 0;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_SHM_RING_BUFFER_I_SHM_RING_BUFFER
