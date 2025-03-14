///
/// \file
/// \copyright Copyright (C) 2016-2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/static_vector.hpp>
#include <score/static_vector.hpp> // test include guard

#include <score/type_traits.hpp>

#include <gtest/gtest.h>

namespace
{

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(static_vector_test, backward_compatible)
{
    static_assert(std::is_same<score::cpp::static_vector<bool, 1>, score::cpp::inplace_vector<bool, 1>>::value, "failed");
    static_assert(score::cpp::is_inplace_vector<score::cpp::static_vector<bool, 1>>::value, "failed");
    static_assert(score::cpp::is_inplace_vector<score::cpp::inplace_vector<bool, 1>>::value, "failed");
    static_assert(score::cpp::is_static_vector<score::cpp::static_vector<bool, 1>>::value, "failed");
    static_assert(score::cpp::is_static_vector<score::cpp::inplace_vector<bool, 1>>::value, "failed");
}

} // namespace
