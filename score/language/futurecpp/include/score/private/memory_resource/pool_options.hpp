///
/// \file
/// \copyright Copyright (C) 2019-2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.Pmr.MemoryResource component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_POOL_OPTIONS_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_POOL_OPTIONS_HPP

#include <cstddef>

namespace score::cpp
{
namespace pmr
{

/// \brief The members of pool_options comprise a set of constructor options for pool resources.
///
/// \see https://en.cppreference.com/w/cpp/memory/pool_options
struct pool_options
{
    /// The maximum number of blocks that will be allocated at once from the upstream memory resource to replenish a
    /// pool. If the value of max_blocks_per_chunk is zero or is greater than an implementation-defined limit, that
    /// limit is used instead. The implementation may choose to use a smaller value than is specified in this field and
    /// may use different values for different pools.
    std::size_t max_blocks_per_chunk;
    /// The largest allocation size that is required to be fulfilled using the pooling mechanism. Attempts to allocate a
    /// single block larger than this threshold will be allocated directly from the upstream memory resource. If
    /// largest_required_pool_block is zero or is greater than an implementation-defined limit, that limit is used
    /// instead. The implementation may choose a pass-through threshold larger than specified in this field.
    std::size_t largest_required_pool_block;
};

} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_POOL_OPTIONS_HPP
