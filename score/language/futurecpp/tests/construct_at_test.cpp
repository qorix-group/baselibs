///
/// \file
/// \copyright Copyright (C) 2020-2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/private/memory/construct_at.hpp>
#include <score/private/memory/construct_at.hpp> // check include guard

#include <gtest/gtest.h>

namespace
{

// NOTRACING
TEST(ConstructAtTest, ConstructAtDefault)
{
    int storage{23};
    const int* const result{score::cpp::detail::construct_at(&storage)};
    EXPECT_EQ(result, &storage);
    EXPECT_EQ(*result, 0);
}

// NOTRACING
TEST(ConstructAtTest, ConstructAtWithArgument)
{
    int storage{23};
    const int argument{42};
    const int* const result{score::cpp::detail::construct_at(&storage, argument)};
    EXPECT_EQ(*result, argument);
}

} // namespace
