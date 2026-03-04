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
#include "score/concurrency/locked_ptr.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "score/concurrency/test_types.h"
#include "score/concurrency/unlock_guard.h"

#include <functional>
#include <mutex>
#include <shared_mutex>
#include <type_traits>
#include <utility>

using score::concurrency::LockedPtr;
using score::concurrency::UnlockGuard;

namespace test
{

namespace
{
struct IntWrapper
{
    int value;
};
}  // namespace

TEST(LockedPtrTest, ConstructionWithTypes)
{
    EXPECT_TRUE((std::is_constructible_v<LockedPtr<int, BasicLockableArchetype>, int*, BasicLockableArchetype>))
        << "LockedPtr should be constructible with BasicLockableArchetype";
    EXPECT_TRUE(
        (std::is_constructible_v<LockedPtr<const int, BasicLockableArchetype>, const int*, BasicLockableArchetype>))
        << "LockedPtr should be constructible with BasicLockableArchetype";
    EXPECT_TRUE(
        (std::is_constructible_v<LockedPtr<int, std::unique_lock<std::mutex>>, int*, std::unique_lock<std::mutex>>))
        << "LockedPtr should be constructible with std::unique_lock<std::mutex>";
    EXPECT_TRUE(
        (std::is_constructible_v<LockedPtr<int, std::shared_lock<std::mutex>>, int*, std::shared_lock<std::mutex>>))
        << "LockedPtr should be constructible with std::shared_lock<std::mutex>";

    EXPECT_TRUE((!std::is_copy_constructible_v<LockedPtr<int, BasicLockableArchetype>>))
        << "LockedPtr should not be copy-constructible";
    EXPECT_TRUE((!std::is_copy_assignable_v<LockedPtr<int, BasicLockableArchetype>>))
        << "LockedPtr should not be copy-assignable";
    EXPECT_TRUE((std::is_move_constructible_v<LockedPtr<int, BasicLockableArchetype>>))
        << "LockedPtr should be move-constructible";
    EXPECT_TRUE((std::is_move_assignable_v<LockedPtr<int, BasicLockableArchetype>>))
        << "LockedPtr should be move-assignable";
}

TEST(LockedPtrTest, SwappingWithTypes)
{
    EXPECT_TRUE((std::is_swappable_v<LockedPtr<int, BasicLockableArchetype>>))
        << "LockedPtr with BasicLockableArchetype should be swappable";
    EXPECT_TRUE((std::is_swappable_v<LockedPtr<int, std::unique_lock<std::mutex>>>))
        << "LockedPtr with unique_lock should be swappable";
    EXPECT_TRUE((std::is_swappable_v<LockedPtr<int, std::shared_lock<std::shared_mutex>>>))
        << "LockedPtr with shared_lock should be swappable";
}

TEST(LockedPtrTest, TFunctionsWithTypes)
{
    // Functions working with T
    EXPECT_TRUE((std::is_same_v<std::invoke_result_t<decltype(&LockedPtr<int, BasicLockableArchetype>::operator*),
                                                     LockedPtr<int, BasicLockableArchetype>*>,
                                int&>))
        << "LockedPtr::operator*() should return int&";
    EXPECT_TRUE((std::is_same_v<std::invoke_result_t<decltype(&LockedPtr<const int, BasicLockableArchetype>::operator*),
                                                     LockedPtr<const int, BasicLockableArchetype>*>,
                                const int&>))
        << "LockedPtr::operator*() should return const int&";
    EXPECT_TRUE((std::is_same_v<std::invoke_result_t<decltype(&LockedPtr<int, BasicLockableArchetype>::operator->),
                                                     LockedPtr<int, BasicLockableArchetype>*>,
                                int*>))
        << "LockedPtr::operator->() should return int*";
    EXPECT_TRUE(
        (std::is_same_v<std::invoke_result_t<decltype(&LockedPtr<const int, BasicLockableArchetype>::operator->),
                                             LockedPtr<const int, BasicLockableArchetype>*>,
                        const int*>))
        << "LockedPtr::operator->() should return const int*";
    EXPECT_TRUE((std::is_same_v<std::invoke_result_t<decltype(&LockedPtr<int, BasicLockableArchetype>::get),
                                                     LockedPtr<int, BasicLockableArchetype>*>,
                                int*>))
        << "LockedPtr::get() should return int*";
    EXPECT_TRUE((std::is_same_v<std::invoke_result_t<decltype(&LockedPtr<const int, BasicLockableArchetype>::get),
                                                     LockedPtr<const int, BasicLockableArchetype>*>,
                                const int*>))
        << "LockedPtr::get() should return const int*";
}

TEST(LockedPtrTest, LockFunctionsWithTypes)
{
    // Functions working with Lock
    EXPECT_TRUE((std::is_same_v<std::invoke_result_t<decltype(&LockedPtr<int, BasicLockableArchetype>::unlock_guard),
                                                     LockedPtr<int, BasicLockableArchetype>*>,
                                UnlockGuard<BasicLockableArchetype>>))
        << "LockedPtr::unlock_guard() should return UnlockGuard<BasicLockableArchetype>";
    EXPECT_TRUE(
        (std::is_same_v<std::invoke_result_t<decltype(&LockedPtr<int, std::unique_lock<std::mutex>>::unlock_guard),
                                             LockedPtr<int, std::unique_lock<std::mutex>>*>,
                        UnlockGuard<std::unique_lock<std::mutex>>>))
        << "LockedPtr::unlock_guard() should return UnlockGuard<unique_lock>";
    EXPECT_TRUE((std::is_same_v<
                 std::invoke_result_t<decltype(&LockedPtr<int, std::shared_lock<std::shared_mutex>>::unlock_guard),
                                      LockedPtr<int, std::shared_lock<std::shared_mutex>>*>,
                 UnlockGuard<std::shared_lock<std::shared_mutex>>>))
        << "LockedPtr::unlock_guard() should return UnlockGuard<shared_lock>";
}

TEST(LockedPtrTest, NonConstWithUniqueLock)
{
    IntWrapper obj{42};
    std::mutex mut;

    {
        EXPECT_EQ(LockedPtr(&obj, std::unique_lock{mut}), &obj);
    }

    {
        EXPECT_EQ(LockedPtr(&obj, std::unique_lock{mut}).get(), &obj);
    }

    {
        EXPECT_EQ(&(*LockedPtr(&obj, std::unique_lock{mut})).value, &obj.value);
    }

    {
        EXPECT_TRUE(static_cast<bool>(LockedPtr(&obj, std::unique_lock{mut})));
    }

    {
        constexpr IntWrapper* nullp = nullptr;
        EXPECT_FALSE(static_cast<bool>(LockedPtr(nullp, std::unique_lock{mut})));
    }
}

TEST(LockedPtrTest, ConstWithUniqueLock)
{
    const IntWrapper obj{42};
    std::mutex mut;

    {
        EXPECT_EQ(LockedPtr(&obj, std::unique_lock{mut}), &obj);
    }

    {
        EXPECT_EQ(LockedPtr(&obj, std::unique_lock{mut}).get(), &obj);
    }

    {
        EXPECT_EQ(&(*LockedPtr(&obj, std::unique_lock{mut})).value, &obj.value);
    }

    {
        EXPECT_TRUE(static_cast<bool>(LockedPtr(&obj, std::unique_lock{mut})));
    }

    {
        constexpr IntWrapper* nullp = nullptr;
        EXPECT_FALSE(static_cast<bool>(LockedPtr(nullp, std::unique_lock{mut})));
    }
}

TEST(LockedPtrTest, NonConstWithSharedLock)
{
    IntWrapper obj{42};
    std::shared_mutex mut;

    {
        EXPECT_EQ(LockedPtr(&obj, std::shared_lock{mut}), &obj);
    }

    {
        EXPECT_EQ(LockedPtr(&obj, std::shared_lock{mut}).get(), &obj);
    }

    {
        EXPECT_EQ(&(*LockedPtr(&obj, std::shared_lock{mut})).value, &obj.value);
    }

    {
        EXPECT_TRUE(static_cast<bool>(LockedPtr(&obj, std::shared_lock{mut})));
    }

    {
        constexpr IntWrapper* nullp = nullptr;
        EXPECT_FALSE(static_cast<bool>(LockedPtr(nullp, std::shared_lock{mut})));
    }
}

TEST(LockedPtrTest, ConstWithSharedLock)
{
    const IntWrapper obj{42};
    std::shared_mutex mut;

    {
        EXPECT_EQ(LockedPtr(&obj, std::shared_lock{mut}), &obj);
    }

    {
        EXPECT_EQ(LockedPtr(&obj, std::shared_lock{mut}).get(), &obj);
    }

    {
        EXPECT_EQ(&(*LockedPtr(&obj, std::shared_lock{mut})).value, &obj.value);
    }

    {
        EXPECT_TRUE(static_cast<bool>(LockedPtr(&obj, std::shared_lock{mut})));
    }

    {
        constexpr IntWrapper* nullp = nullptr;
        EXPECT_FALSE(static_cast<bool>(LockedPtr(nullp, std::shared_lock{mut})));
    }
}

TEST(LockedPtrTest, Swap)
{
    {
        IntWrapper obj1{11};
        IntWrapper obj2{22};
        MockMutex mut1;
        MockMutex mut2;

        auto lp1 = LockedPtr(&obj1, std::unique_lock{mut1});
        auto lp2 = LockedPtr(&obj2, std::unique_lock{mut2});
        EXPECT_THAT(std::pair(lp1.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj1, true)));
        EXPECT_THAT(std::pair(lp2.get(), mut2.is_locked()), ::testing::Eq(std::pair(&obj2, true)));

        lp1.swap(lp2);
        EXPECT_THAT(std::pair(lp1.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj2, true)));
        EXPECT_THAT(std::pair(lp2.get(), mut2.is_locked()), ::testing::Eq(std::pair(&obj1, true)));
    }

    {
        const IntWrapper obj1{11};
        const IntWrapper obj2{22};
        MockMutex mut1;
        MockMutex mut2;

        auto clp1 = LockedPtr(&obj1, std::unique_lock{mut1});
        auto clp2 = LockedPtr(&obj2, std::unique_lock{mut2});
        EXPECT_THAT(std::pair(clp1.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj1, true)));
        EXPECT_THAT(std::pair(clp2.get(), mut2.is_locked()), ::testing::Eq(std::pair(&obj2, true)));

        clp1.swap(clp2);
        EXPECT_THAT(std::pair(clp1.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj2, true)));
        EXPECT_THAT(std::pair(clp2.get(), mut2.is_locked()), ::testing::Eq(std::pair(&obj1, true)));
    }
}

TEST(LockedPtrTest, Move)
{
    {
        IntWrapper obj1{11};
        IntWrapper obj2{22};
        MockMutex mut1;
        MockMutex mut2;

        auto lp1 = LockedPtr(&obj1, std::unique_lock{mut1});
        auto lp2 = LockedPtr(&obj2, std::unique_lock{mut2});
        EXPECT_THAT(std::pair(lp1.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj1, true)));
        EXPECT_THAT(std::pair(lp2.get(), mut2.is_locked()), ::testing::Eq(std::pair(&obj2, true)));

        lp1 = std::move(lp2);
        EXPECT_THAT(std::pair(lp1.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj2, false)));
        EXPECT_FALSE(lp2);
        EXPECT_TRUE(mut2.is_locked());
    }

    {
        const IntWrapper obj1{11};
        const IntWrapper obj2{22};
        MockMutex mut1;
        MockMutex mut2;

        auto clp1 = LockedPtr(&obj1, std::unique_lock{mut1});
        auto clp2 = LockedPtr(&obj2, std::unique_lock{mut2});
        EXPECT_THAT(std::pair(clp1.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj1, true)));
        EXPECT_THAT(std::pair(clp2.get(), mut2.is_locked()), ::testing::Eq(std::pair(&obj2, true)));

        clp1 = std::move(clp2);
        EXPECT_THAT(std::pair(clp1.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj2, false)));
        EXPECT_FALSE(clp2);
        EXPECT_TRUE(mut2.is_locked());
    }

    {
        IntWrapper obj1{11};
        MockMutex mut1;

        auto lp1 = LockedPtr(&obj1, std::unique_lock{mut1});
        EXPECT_THAT(std::pair(lp1.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj1, true)));

        auto lp2(std::move(lp1));
        EXPECT_FALSE(lp1);
        EXPECT_THAT(std::pair(lp2.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj1, true)));
    }

    {
        const IntWrapper obj1{11};
        MockMutex mut1;

        auto clp1 = LockedPtr(&obj1, std::unique_lock{mut1});
        EXPECT_THAT(std::pair(clp1.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj1, true)));

        auto clp2 = std::move(clp1);
        EXPECT_FALSE(clp1);
        EXPECT_THAT(std::pair(clp2.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj1, true)));
    }
}

TEST(LockedPtrTest, UnlockGuard)
{
    IntWrapper obj{42};
    MockMutex mut;

    auto lp = LockedPtr(&obj, std::unique_lock{mut});
    EXPECT_TRUE(mut.is_locked());

    {
        auto ug = lp.unlock_guard();
        EXPECT_FALSE(mut.is_locked());
    }

    EXPECT_TRUE(mut.is_locked());
}

}  // namespace test
