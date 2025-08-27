///
/// @file common_atomic_container_test.h
/// @copyright Copyright (C) 2025, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief AtomicContainer tests common header file
///

#include "score/analysis/tracing/library/generic_trace_api/containers/atomic_container/atomic_container.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace score::analysis::tracing;
using namespace score::memory::shared;

using ::testing::_;
using ::testing::AtLeast;
using ::testing::InSequence;
using ::testing::Return;

constexpr std::size_t kContainerSize = 100;
constexpr std::size_t kThreadCount = 8;
constexpr std::size_t kOperationsPerThread = 10000;

struct TestData
{
    std::size_t value;
    bool operator==(const TestData& other) const
    {
        return value == other.value;
    }
};

class AtomicContainerTest : public ::testing::Test
{
  protected:
    AtomicContainer<TestData, kContainerSize> container;
};
