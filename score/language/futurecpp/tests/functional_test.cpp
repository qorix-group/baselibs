///
/// \file
/// \copyright Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/functional.hpp>
#include <score/functional.hpp> // test include guard

#include <type_traits>

#include <gtest/gtest.h>

namespace
{

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16079319
TEST(functional_test, identity)
{
    EXPECT_EQ(1, score::cpp::identity{}(1));

    int a{23};
    static_assert(std::is_same<decltype(score::cpp::identity{}(a)), int&>::value, "Mismatch.");
    EXPECT_EQ(23, score::cpp::identity{}(a));

    const int b{42};
    static_assert(std::is_same<decltype(score::cpp::identity{}(b)), const int&>::value, "Mismatch.");
    EXPECT_EQ(42, score::cpp::identity{}(b));
}

} // namespace
