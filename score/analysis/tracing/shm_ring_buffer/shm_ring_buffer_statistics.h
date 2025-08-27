///
/// @file shm_ring_buffer_statistics.h
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Shared memory ring buffer statistics header file
///

#ifndef SCORE_ANALYSIS_TRACING_SHM_RING_BUFFER_SHM_RING_BUFFER_STATISTICS_H
#define SCORE_ANALYSIS_TRACING_SHM_RING_BUFFER_SHM_RING_BUFFER_STATISTICS_H

#include <cstdint>

namespace score
{
namespace analysis
{
namespace tracing
{

template <typename T>
// No need to define the rule of five for this struct
// No harm to define struct in that format
// coverity[autosar_cpp14_a12_8_6_violation]
// coverity[autosar_cpp14_a11_0_2_violation]
struct BasicStatistics  // NOLINT(score-struct-usage-compliance): Intended struct semantic
{
    //  In this case the name would be too long to fully specify, so that's why we choose CAS = C(ompare)-A(nd)-S(wap)
    //  operation.
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    T cas_trials_{};
    //  In this case the name would be too long to fully specify, so that's why we choose CAS = C(ompare)-A(nd)-S(wap)
    //  operation.
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    T cas_failures_{};
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    T call_count_{};
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    T call_failure_count_{};
};

template <typename T>
// No harm to define struct in that format
// coverity[autosar_cpp14_a11_0_2_violation]
struct ConsumerStatistics : public BasicStatistics<T>  // NOLINT(score-struct-usage-compliance): Intended struct semantic
{
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    T buffer_empty_count_{};
};

template <typename T>
// No harm to define struct in that format
// coverity[autosar_cpp14_a11_0_2_violation]
struct ProducerStatistics : public BasicStatistics<T>  // NOLINT(score-struct-usage-compliance): Intended struct semantic
{
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    T buffer_full_count_{};
};

template <typename T>
/// @brief ShmRingBufferStatistics class
///
/// Class used to store Shm ring buffer producer and consumer statistics
// No harm to define struct in that format
// coverity[autosar_cpp14_a11_0_2_violation]
struct ShmRingBufferStatisticsTemplate  // NOLINT(score-struct-usage-compliance): Intended struct semantic
{
    ShmRingBufferStatisticsTemplate() = default;
    ShmRingBufferStatisticsTemplate(const ProducerStatistics<T>& producer,
                                    const ConsumerStatistics<std::uint64_t>& consumer)
        : producer_{producer}, consumer_{consumer}
    {
    }

    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    ProducerStatistics<T> producer_;
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    ConsumerStatistics<std::uint64_t> consumer_;
};

// Non-atomic version
using ShmRingBufferStatistics = ShmRingBufferStatisticsTemplate<std::uint64_t>;

// Atomic version
using ShmRingBufferAtomicStatistics = ShmRingBufferStatisticsTemplate<std::atomic<std::uint64_t>>;

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_SHM_RING_BUFFER_SHM_RING_BUFFER_STATISTICS_H
