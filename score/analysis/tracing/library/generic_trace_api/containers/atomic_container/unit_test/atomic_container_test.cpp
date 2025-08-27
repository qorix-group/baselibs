///
/// @file atomic_container_test.cpp
/// @copyright Copyright (C) 2025, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief AtomicContainer main functionality tests source file
///

#include "score/analysis/tracing/library/generic_trace_api/containers/atomic_container/unit_test/common_atomic_container_test.h"

TEST_F(AtomicContainerTest, Initialization)
{
    EXPECT_EQ(container.GetEmptyElementsCount(), kContainerSize);
}

TEST_F(AtomicContainerTest, AcquireElement)
{
    auto element = container.Acquire();
    ASSERT_TRUE(element.has_value());
    EXPECT_EQ(container.GetEmptyElementsCount(), kContainerSize - 1);
}

TEST_F(AtomicContainerTest, AcquireMultipleElements)
{
    auto first = container.Acquire();
    auto second = container.Acquire();
    ASSERT_TRUE(first.has_value());
    ASSERT_TRUE(second.has_value());
    EXPECT_EQ(container.GetEmptyElementsCount(), kContainerSize - 2);
}

TEST_F(AtomicContainerTest, AcquireWhenFull)
{
    for (int i = 0; i < 100; ++i)
    {
        auto res = container.Acquire();
        ASSERT_TRUE(res.has_value());
    }

    auto res = container.Acquire();
    EXPECT_FALSE(res.has_value());
}

TEST_F(AtomicContainerTest, ReleaseElement)
{
    auto element = container.Acquire();
    ASSERT_TRUE(element.has_value());

    container.Release(element.value());
    EXPECT_EQ(container.GetEmptyElementsCount(), kContainerSize);
}

TEST_F(AtomicContainerTest, ReleaseIndex)
{
    auto element = container.Acquire();
    ASSERT_TRUE(element.has_value());

    std::size_t index = 0;
    container.Release(index);
    EXPECT_TRUE(container.IsFreeAt(index));
    EXPECT_EQ(container.GetEmptyElementsCount(), kContainerSize);
}

TEST_F(AtomicContainerTest, FindIfElementExists)
{
    TestData item1{42};
    TestData item2{99};

    auto acquired1 = container.Acquire();
    auto acquired2 = container.Acquire();

    ASSERT_TRUE(acquired1.has_value());
    ASSERT_TRUE(acquired2.has_value());

    acquired1.value().get().value = item1.value;
    acquired2.value().get().value = item2.value;

    auto found = container.FindIf([](const TestData& data) {
        return data.value == 42;
    });
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found.value().get().value, 42);

    const auto& const_container = container;
    auto const_found = const_container.FindIf([](const TestData& data) {
        return data.value == 42;
    });
    ASSERT_TRUE(const_found.has_value());
    EXPECT_EQ(const_found.value().get().value, 42);
}

TEST_F(AtomicContainerTest, FindIElementDoesNotExist)
{
    auto found = container.FindIf([](const TestData& data) {
        return data.value == 999;
    });
    EXPECT_FALSE(found.has_value());

    const auto& const_container = container;
    auto const_found = const_container.FindIf([](const TestData& data) {
        return data.value == 999;
    });
    EXPECT_FALSE(const_found.has_value());
}

TEST_F(AtomicContainerTest, IsFreeAtIndexOutOfBounds)
{
    EXPECT_FALSE(container.IsFreeAt(static_cast<std::size_t>(kContainerSize + 1)));
}

TEST_F(AtomicContainerTest, IsFreeAtIndexSuccess)
{
    auto acquired = container.Acquire();
    ASSERT_TRUE(acquired.has_value());

    EXPECT_FALSE(container.IsFreeAt(static_cast<std::size_t>(0U)));

    container.Release(static_cast<std::size_t>(0U));
    EXPECT_TRUE(container.IsFreeAt(static_cast<std::size_t>(0U)));
}

TEST_F(AtomicContainerTest, EmptyElementsCount)
{
    EXPECT_EQ(container.GetEmptyElementsCount(), kContainerSize);

    auto element = container.Acquire();
    EXPECT_EQ(container.GetEmptyElementsCount(), kContainerSize - 1);

    container.Release(element.value());
    EXPECT_EQ(container.GetEmptyElementsCount(), kContainerSize);
}

TEST_F(AtomicContainerTest, OperatorAccess)
{
    auto element = container.Acquire();
    ASSERT_TRUE(element.has_value());

    std::size_t index = 0;
    container[index].value = 55;
    EXPECT_EQ(container[index].value, 55);
}

TEST_F(AtomicContainerTest, AccessInvalidIndex)
{
    EXPECT_DEATH_IF_SUPPORTED(container[101], "");
}

TEST_F(AtomicContainerTest, ConstAccessInvalidIndex)
{
    const auto& const_container = container;
    EXPECT_DEATH_IF_SUPPORTED(const_container[101], "");
}
