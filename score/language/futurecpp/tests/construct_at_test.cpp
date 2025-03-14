///
/// \file
/// \copyright Copyright (C) 2020-2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/private/memory/construct_at.hpp>
#include <score/private/memory/construct_at.hpp> // check include guard

#include <type_traits>

#include <gtest/gtest.h>

namespace
{

TEST(ConstructAtTest, ConstructAtDefault)
{
    int storage{23};
    const int* const result{score::cpp::detail::construct_at(&storage)};
    EXPECT_EQ(result, &storage);
    EXPECT_EQ(*result, 0);
}

TEST(ConstructAtTest, ConstructAtWithArgument)
{
    int storage{23};
    const int argument{42};
    const int* const result{score::cpp::detail::construct_at(&storage, argument)};
    EXPECT_EQ(*result, argument);
}

TEST(ConstructAtTest, ConstructAtConst)
{
    int storage{23};
    const int* const const_pointer{&storage};
    const auto result{score::cpp::detail::construct_at(const_pointer)};
    EXPECT_TRUE((std::is_same<decltype(result), const int* const>::value));
    EXPECT_EQ(*result, 0);
}

} // namespace
