///
/// \file
/// \copyright Copyright (C) 2019-2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.Pmr.MemoryResource component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_MEMORY_RESOURCE_HPP
#define SCORE_LANGUAGE_FUTURECPP_MEMORY_RESOURCE_HPP

#include <score/private/memory_resource/memory_resource.hpp>              // IWYU pragma: export
#include <score/private/memory_resource/monotonic_buffer_resource.hpp>    // IWYU pragma: export
#include <score/private/memory_resource/polymorphic_allocator.hpp>        // IWYU pragma: export
#include <score/private/memory_resource/pool_options.hpp>                 // IWYU pragma: export
#include <score/private/memory_resource/resource_adaptor.hpp>             // IWYU pragma: export
#include <score/private/memory_resource/unsynchronized_pool_resource.hpp> // IWYU pragma: export

#include <score/algorithm.hpp> // to be removed but removing them may break downstream

#endif // SCORE_LANGUAGE_FUTURECPP_MEMORY_RESOURCE_HPP
