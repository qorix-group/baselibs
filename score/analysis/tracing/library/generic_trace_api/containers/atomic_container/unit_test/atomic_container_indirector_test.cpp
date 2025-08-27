///
/// @file atomic_container_indirector_test.cpp
/// @copyright Copyright (C) 2025, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief AtomicContainer tests with mocked atomic operations source file
///

#include "score/analysis/tracing/library/generic_trace_api/containers/atomic_container/unit_test/common_atomic_container_test.h"
#include "score/memory/shared/atomic_mock.h"

class AtomicContainerMockTest : public AtomicContainerTest
{
  protected:
    void SetUp() override
    {
        atomic_mock_ = std::make_unique<AtomicMock<std::size_t>>();
        atomic_bool_mock_ = std::make_unique<AtomicMock<bool>>();

        EXPECT_EQ(AtomicIndirectorMock<std::size_t>::GetMockObject(), nullptr);
        AtomicIndirectorMock<std::size_t>::SetMockObject(atomic_mock_.get());
        EXPECT_EQ(AtomicIndirectorMock<std::size_t>::GetMockObject(), atomic_mock_.get());

        EXPECT_EQ(AtomicIndirectorMock<bool>::GetMockObject(), nullptr);
        AtomicIndirectorMock<bool>::SetMockObject(atomic_bool_mock_.get());
        EXPECT_EQ(AtomicIndirectorMock<bool>::GetMockObject(), atomic_bool_mock_.get());
    }

    void TearDown() override
    {
        AtomicIndirectorMock<std::size_t>::SetMockObject(nullptr);
        EXPECT_EQ(AtomicIndirectorMock<std::size_t>::GetMockObject(), nullptr);

        AtomicIndirectorMock<bool>::SetMockObject(nullptr);
        EXPECT_EQ(AtomicIndirectorMock<bool>::GetMockObject(), nullptr);
    }

    using MockAtomicContainer = AtomicContainer<TestData, kContainerSize, AtomicIndirectorMock>;
    MockAtomicContainer container;

    std::unique_ptr<AtomicMock<std::size_t>> atomic_mock_;
    std::unique_ptr<AtomicMock<bool>> atomic_bool_mock_;
};

TEST_F(AtomicContainerMockTest, AcquireEmptyElementsLoadFails)
{
    EXPECT_CALL(*atomic_mock_, load(_)).WillOnce(Return(0U));

    auto element = container.Acquire();
    EXPECT_FALSE(element.has_value());
}

TEST_F(AtomicContainerMockTest, AcquireCompareExchangeWeakFails)
{
    {
        InSequence sequence;
        EXPECT_CALL(*atomic_mock_, load(_)).Times(1).WillOnce(Return(1U));
        EXPECT_CALL(*atomic_mock_, load(_)).Times(1).WillOnce(Return(0U));
        EXPECT_CALL(*atomic_bool_mock_, compare_exchange_weak(_, _, _, _))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(false));
    }

    auto element = container.Acquire();
    EXPECT_FALSE(element.has_value());
}

TEST_F(AtomicContainerMockTest, ReleaseElementCompareExchangeStrongFails)
{
    {
        InSequence sequence;
        EXPECT_CALL(*atomic_mock_, load(_)).Times(1).WillOnce(Return(1U));
        EXPECT_CALL(*atomic_mock_, load(_)).Times(1).WillOnce(Return(0U));
        EXPECT_CALL(*atomic_bool_mock_, compare_exchange_weak(_, _, _, _)).Times(AtLeast(1)).WillOnce(Return(true));
        EXPECT_CALL(*atomic_mock_, fetch_sub(_, _)).Times(1).WillOnce(Return(kContainerSize - 1));
        EXPECT_CALL(*atomic_mock_, store(_, _)).Times(1);
        EXPECT_CALL(*atomic_bool_mock_, compare_exchange_strong(_, _, _, _))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(false));
    }

    auto element = container.Acquire();
    EXPECT_TRUE(element.has_value());
    container.Release(element.value());
}

TEST_F(AtomicContainerMockTest, ReleaseIndexCompareExchangeStrongFails)
{
    {
        InSequence sequence;
        EXPECT_CALL(*atomic_mock_, load(_)).Times(1).WillOnce(Return(1U));
        EXPECT_CALL(*atomic_mock_, load(_)).Times(1).WillOnce(Return(0U));
        EXPECT_CALL(*atomic_bool_mock_, compare_exchange_weak(_, _, _, _)).Times(AtLeast(1)).WillOnce(Return(true));
        EXPECT_CALL(*atomic_mock_, fetch_sub(_, _)).Times(1).WillOnce(Return(kContainerSize));
        EXPECT_CALL(*atomic_mock_, store(_, _)).Times(1);
        EXPECT_CALL(*atomic_bool_mock_, compare_exchange_strong(_, _, _, _))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(false));
    }

    auto element = container.Acquire();
    EXPECT_TRUE(element.has_value());

    std::size_t index = 0;
    container.Release(index);
}

TEST_F(AtomicContainerMockTest, FindIfNotFound)
{
    TestData item1{42};

    {
        InSequence sequence;
        EXPECT_CALL(*atomic_mock_, load(_)).Times(1).WillOnce(Return(1U));
        EXPECT_CALL(*atomic_mock_, load(_)).Times(1).WillOnce(Return(0U));
        EXPECT_CALL(*atomic_bool_mock_, compare_exchange_weak(_, _, _, _)).Times(AtLeast(1)).WillOnce(Return(true));
        EXPECT_CALL(*atomic_mock_, fetch_sub(_, _)).Times(1).WillOnce(Return(kContainerSize));
        EXPECT_CALL(*atomic_mock_, store(_, _)).Times(1);
        EXPECT_CALL(*atomic_bool_mock_, load(_)).Times(AtLeast(1)).WillRepeatedly(Return(false));
    }

    auto acquired_element = container.Acquire();
    EXPECT_TRUE(acquired_element.has_value());
    acquired_element.value().get().value = item1.value;
    const auto found = container.FindIf([](const TestData& data) {
        return data.value == 42;
    });
    EXPECT_FALSE(found.has_value());
}

TEST_F(AtomicContainerMockTest, FindIfConstLoadFails)
{
    const TestData item1{42};

    {
        InSequence sequence;
        EXPECT_CALL(*atomic_mock_, load(_)).Times(1).WillOnce(Return(1U));
        EXPECT_CALL(*atomic_mock_, load(_)).Times(1).WillOnce(Return(0U));
        EXPECT_CALL(*atomic_bool_mock_, compare_exchange_weak(_, _, _, _)).Times(AtLeast(1)).WillOnce(Return(true));
        EXPECT_CALL(*atomic_mock_, fetch_sub(_, _)).Times(1).WillOnce(Return(kContainerSize));
        EXPECT_CALL(*atomic_mock_, store(_, _)).Times(1);
        EXPECT_CALL(*atomic_bool_mock_, load(_)).Times(AtLeast(1)).WillRepeatedly(Return(false));
    }

    const auto& const_container = container;
    auto acquired_element = container.Acquire();
    EXPECT_TRUE(acquired_element.has_value());
    acquired_element.value().get().value = item1.value;
    const auto found = const_container.FindIf([](const TestData& data) {
        return data.value == 42;
    });
    EXPECT_FALSE(found.has_value());
}

TEST_F(AtomicContainerMockTest, IsFreeAtIndexIsOccupied)
{
    {
        InSequence sequence;
        EXPECT_CALL(*atomic_mock_, load(_)).Times(1).WillOnce(Return(1U));
        EXPECT_CALL(*atomic_mock_, load(_)).Times(1).WillOnce(Return(0U));
        EXPECT_CALL(*atomic_bool_mock_, compare_exchange_weak(_, _, _, _))
            .Times(AtLeast(1))
            .WillRepeatedly([](bool& expected, bool, std::memory_order, std::memory_order) {
                expected = true;
                return true;
            });
        EXPECT_CALL(*atomic_mock_, fetch_sub(_, _)).Times(1).WillOnce(Return(kContainerSize));
        EXPECT_CALL(*atomic_mock_, store(_, _)).Times(1);
        EXPECT_CALL(*atomic_bool_mock_, load(_)).Times(1).WillOnce(Return(true));
    }

    auto acquired = container.Acquire();
    EXPECT_TRUE(acquired.has_value());
    EXPECT_FALSE(container.IsFreeAt(static_cast<std::size_t>(0U)));
}
