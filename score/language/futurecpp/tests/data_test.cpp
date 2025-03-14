///
/// \file
/// \copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/private/iterator/data.hpp>
#include <score/private/iterator/data.hpp> // check include guard

#include <array>
#include <cstdint>
#include <initializer_list>
#include <vector>

#include <gtest/gtest.h>

namespace
{

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22116168
TEST(data, data_on_c_array)
{
    std::int32_t a[1]{};
    EXPECT_EQ(a, score::cpp::data(a));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22116168
TEST(data, data_on_const_c_array)
{
    const std::int32_t a[1]{};
    EXPECT_EQ(a, score::cpp::data(a));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22116168
TEST(data, data_on_array)
{
    std::array<std::int32_t, 1> a{};
    EXPECT_EQ(a.data(), score::cpp::data(a));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22116168
TEST(data, data_on_const_array)
{
    const std::array<std::int32_t, 1> a{};
    EXPECT_EQ(a.data(), score::cpp::data(a));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22116168
TEST(data, data_on_vector)
{
    std::vector<std::int32_t> a{1};
    EXPECT_EQ(a.data(), score::cpp::data(a));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22116168
TEST(data, data_on_const_vector)
{
    const std::vector<std::int32_t> a{1};
    EXPECT_EQ(a.data(), score::cpp::data(a));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22116168
TEST(data, data_on_initializer_list)
{
    const std::initializer_list<std::int32_t> a{1};
    EXPECT_EQ(a.begin(), score::cpp::data(a));
}

} // namespace
