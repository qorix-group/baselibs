///
/// \file
/// \copyright Copyright (C) 2020, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/chrono.hpp>
#include <score/chrono.hpp> // include guard test

#include <chrono>

#include <gtest/gtest.h>

namespace
{

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7990907
TEST(ChronoTest, AbsDurationPositive)
{
    const std::chrono::seconds duration{123};

    ASSERT_EQ(score::cpp::chrono::abs(duration), std::chrono::seconds{123});
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7990907
TEST(ChronoTest, AbsDurationNegative)
{
    const std::chrono::seconds duration{-123};

    ASSERT_EQ(score::cpp::chrono::abs(duration), std::chrono::seconds{123});
}

} // namespace
