/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "score/utils/pimpl_ptr.h"
#include <type_traits>

class TestClassAssert
{
  public:
    TestClassAssert()
    {
        call_assert();
    }

    ~TestClassAssert()
    {
        call_assert();
    }

    void call_assert()
    {
        ASSERT_TRUE(false);
    }
};

class TestClassAssert2
{
  public:
    TestClassAssert2()
    {
        ++constructed;
    }

    ~TestClassAssert2()
    {
        destructed();
    }

    MOCK_METHOD(void, destructed, ());

    unsigned constructed = 0;
};

class TestClass1
{
};

class TestClass2
{
  public:
    TestClass2(int a_in, double b_in) : a(a_in), b(b_in) {}
    int a;
    double b;
};

using namespace score::utils;

TEST(pimplptrTest, does_not_construct_and_destruct_class)
{
    pimpl_ptr_16<TestClassAssert> pp1;
}

TEST(pimplptrTest, does_construct_and_destruct_class)
{
    {
        auto pp1 = make_pimpl_256<TestClassAssert2>();
        ASSERT_TRUE(pp1->constructed);
        EXPECT_CALL(*pp1.get(), destructed());
    }
}

TEST(pimplptrTest, does_construct_and_destruct_class_once)
{
    {
        auto pp1 = make_pimpl_256<TestClassAssert2>();
        auto pp2 = std::move(pp1);
        ASSERT_EQ(1u, pp2->constructed);
        EXPECT_CALL(*pp2.get(), destructed());
    }
}

TEST(pimplptrTest, destructs_moved_to_object)
{
    {
        auto pp1 = make_pimpl_256<TestClassAssert2>();
        auto pp2 = make_pimpl_256<TestClassAssert2>();

        ASSERT_EQ(1u, pp1->constructed);
        ASSERT_EQ(1u, pp2->constructed);

        EXPECT_CALL(*pp2.get(), destructed());
        pp2 = std::move(pp1);
        EXPECT_CALL(*pp2.get(), destructed());
    }
}

TEST(pimplptrTest, constructs_class_with_arguments)
{
    auto pp2 = make_pimpl_16<TestClass2>(1, 1.0);
}

TEST(pimplptrTest, gives_access_to_members)
{
    auto pp2 = make_pimpl_16<TestClass2>(1, 1.0);
    ASSERT_EQ(1, pp2->a);
    ASSERT_DOUBLE_EQ(1.0, pp2->b);
}

TEST(pimplptrTest, gives_access_to_members_after_move)
{
    auto pp1 = make_pimpl_16<TestClass2>(12, 165.0);
    auto pp2 = std::move(pp1);
    ASSERT_EQ(12, pp2->a);
    ASSERT_DOUBLE_EQ(165.0, pp2->b);
}
