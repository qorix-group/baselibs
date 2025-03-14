///
/// \file
/// \copyright Copyright (C) 2020, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/narrow.hpp>
#include <score/narrow.hpp> // test include guard

#include <cstdint>

#include <gtest/gtest.h>

namespace
{

///
/// \test Test if returned optional correctly holds value or not depending on validity of intended type conversion.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505831
TEST(narrow_test, narrow)
{
    const auto result_valid = score::cpp::narrow<std::uint8_t>(42);
    const auto result_invalid = score::cpp::narrow<std::uint8_t>(4242);

    ASSERT_TRUE(result_valid.has_value());
    EXPECT_EQ(result_valid.value(), 42);
    ASSERT_FALSE(result_invalid.has_value());
}

} // namespace
