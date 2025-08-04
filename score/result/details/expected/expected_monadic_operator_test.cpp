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
#include "score/result/details/expected/expected.h"

#include "score/result/details/expected/test_types.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score::details
{
namespace
{

TEST(ExpectedTest, AndThenLValueRefWillCallFunctionIfHasValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<expected<CopyableType, ErrorType>(CopyableType&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(wrapped)).WillOnce(::testing::Return(expected<CopyableType, ErrorType>{monad_value}));

    // When calling and_then
    const auto result = unit.and_then(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, monad_value);
}

TEST(ExpectedTest, AndThenLValueRefWillReturnReboundErrorIfHasNoValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<expected<CopyableType, CompatibleCopyableType>(ValueType&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling and_then
    const auto result = unit.and_then(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().inner_.value_, value);
}

TEST(ExpectedTest, AndThenLValueConstRefWillCallFunctionIfHasValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<expected<CopyableType, ErrorType>(const CopyableType&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(wrapped)).WillOnce(::testing::Return(expected<CopyableType, ErrorType>{monad_value}));

    // When calling and_then
    const auto result = unit.and_then(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, monad_value);
}

TEST(ExpectedTest, AndThenLValueConstRefWillReturnReboundErrorIfHasNoValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<expected<CopyableType, CompatibleCopyableType>(const ValueType&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling and_then
    const auto result = unit.and_then(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().inner_.value_, value);
}

TEST(ExpectedTest, AndThenRValueRefWillCallFunctionIfHasValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<expected<CopyableType, ErrorType>(CopyableType&&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(std::move(wrapped)))
        .WillOnce(::testing::Return(expected<CopyableType, ErrorType>{monad_value}));

    // When calling and_then
    const auto result = std::move(unit).and_then(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, monad_value);
}

TEST(ExpectedTest, AndThenRValueRefWillReturnReboundErrorIfHasNoValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<expected<CopyableType, CompatibleCopyableType>(ValueType&&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling and_then
    const auto result = std::move(unit).and_then(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().inner_.value_, value);
}

TEST(ExpectedTest, AndThenRValueConstRefWillCallFunctionIfHasValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<expected<CopyableType, ErrorType>(const CopyableType&&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(std::move(wrapped)))
        .WillOnce(::testing::Return(expected<CopyableType, ErrorType>{monad_value}));

    // When calling and_then
    const auto result = std::move(unit).and_then(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, monad_value);
}

TEST(ExpectedTest, AndThenRValueConstRefWillReturnReboundErrorIfHasNoValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<expected<CopyableType, CompatibleCopyableType>(const ValueType&&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling and_then
    const auto result = std::move(unit).and_then(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().inner_.value_, value);
}

TEST(ExpectedTest, OrElseLValueRefWillCallFunctionIfHasNoValue)
{
    // Given an expected with an error
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<expected<ValueType, CopyableType>(CopyableType&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(wrapped))
        .WillOnce(::testing::Return(expected<ValueType, CopyableType>{unexpect, monad_value}));

    // When calling or_else
    const auto result = unit.or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, monad_value);
}

TEST(ExpectedTest, OrElseLValueRefWillReturnReboundValueIfHasValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<expected<CompatibleCopyableType, ErrorType>(ErrorType&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling or_else
    const auto result = unit.or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->inner_.value_, value);
}

TEST(ExpectedTest, OrElseConstLValueRefWillCallFunctionIfHasNoValue)
{
    // Given an expected with an error
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<expected<ValueType, CopyableType>(const CopyableType&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(wrapped))
        .WillOnce(::testing::Return(expected<ValueType, CopyableType>{unexpect, monad_value}));

    // When calling or_else
    const auto result = unit.or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, monad_value);
}

TEST(ExpectedTest, OrElseConstLValueRefWillReturnReboundValueIfHasValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<expected<CompatibleCopyableType, ErrorType>(const ErrorType&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling or_else
    const auto result = unit.or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->inner_.value_, value);
}

TEST(ExpectedTest, OrElseRValueRefWillCallFunctionIfHasNoValue)
{
    // Given an expected with an error
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<expected<ValueType, CopyableType>(CopyableType&&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(std::move(wrapped)))
        .WillOnce(::testing::Return(expected<ValueType, CopyableType>{unexpect, monad_value}));

    // When calling or_else
    const auto result = std::move(unit).or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, monad_value);
}

TEST(ExpectedTest, OrElseRValueRefWillReturnReboundValueIfHasValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<expected<CompatibleCopyableType, ErrorType>(ErrorType&&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling or_else
    const auto result = std::move(unit).or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->inner_.value_, value);
}

TEST(ExpectedTest, OrElseConstRValueRefWillCallFunctionIfHasNoValue)
{
    // Given an expected with an error
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<expected<ValueType, CopyableType>(const CopyableType&&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(std::move(wrapped)))
        .WillOnce(::testing::Return(expected<ValueType, CopyableType>{unexpect, monad_value}));

    // When calling or_else
    const auto result = std::move(unit).or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, monad_value);
}

TEST(ExpectedTest, OrElseConstRValueRefWillReturnReboundValueIfHasValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<expected<CompatibleCopyableType, ErrorType>(const ErrorType&&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling or_else
    const auto result = std::move(unit).or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->inner_.value_, value);
}

TEST(ExpectedTest, TransformLValueRefWillCallFunctionIfHasValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<CompatibleCopyableType(CopyableType&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(wrapped)).WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_value}}));

    // When calling transform
    const auto result = unit.transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->inner_.value_, monad_value);
}

TEST(ExpectedTest, TransformLValueRefWillReturnReboundErrorIfHasNoValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<CopyableType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType(CopyableType&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling transform
    const auto result = unit.transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, value);
}

TEST(ExpectedTest, TransformConstLValueRefWillCallFunctionIfHasValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<CompatibleCopyableType(const CopyableType&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(wrapped)).WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_value}}));

    // When calling transform
    const auto result = unit.transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->inner_.value_, monad_value);
}

TEST(ExpectedTest, TransformConstLValueRefWillReturnReboundErrorIfHasNoValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<CopyableType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType(const CopyableType&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling transform
    const auto result = unit.transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, value);
}

TEST(ExpectedTest, TransformRValueRefWillCallFunctionIfHasValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<CompatibleCopyableType(CopyableType&&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(std::move(wrapped)))
        .WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_value}}));

    // When calling transform
    const auto result = std::move(unit).transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->inner_.value_, monad_value);
}

TEST(ExpectedTest, TransformRValueRefWillReturnReboundErrorIfHasNoValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<CopyableType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType(CopyableType&&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling transform
    const auto result = std::move(unit).transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, value);
}

TEST(ExpectedTest, TransformConstRValueRefWillCallFunctionIfHasValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<CompatibleCopyableType(const CopyableType&&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(std::move(wrapped)))
        .WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_value}}));

    // When calling transform
    const auto result = std::move(unit).transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->inner_.value_, monad_value);
}

TEST(ExpectedTest, TransformConstRValueRefWillReturnReboundErrorIfHasNoValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<CopyableType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType(const CopyableType&&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling transform
    const auto result = std::move(unit).transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, value);
}

TEST(ExpectedTest, TransformErrorLValueRefWillCallFunctionIfHasValue)
{
    // Given an expected with an error
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<CompatibleCopyableType(CopyableType&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(wrapped)).WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_value}}));

    // When calling transform_error
    const auto result = unit.transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().inner_.value_, monad_value);
}

TEST(ExpectedTest, TransformErrorLValueRefWillReturnReboundValueIfHasValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<CopyableType, CopyableType> unit{wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType(CopyableType&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling transform_error
    const auto result = unit.transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, value);
}

TEST(ExpectedTest, TransformErrorConstLValueRefWillCallFunctionIfHasValue)
{
    // Given an expected with an error
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<CompatibleCopyableType(const CopyableType&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(wrapped)).WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_value}}));

    // When calling transform_error
    const auto result = unit.transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().inner_.value_, monad_value);
}

TEST(ExpectedTest, TransformErrorConstLValueRefWillReturnReboundValueIfHasValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<CopyableType, CopyableType> unit{wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType(const CopyableType&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling transform_error
    const auto result = unit.transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, value);
}

TEST(ExpectedTest, TransformErrorRValueRefWillCallFunctionIfHasValue)
{
    // Given an expected with an error
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<CompatibleCopyableType(CopyableType&&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(std::move(wrapped)))
        .WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_value}}));

    // When calling transform_error
    const auto result = std::move(unit).transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().inner_.value_, monad_value);
}

TEST(ExpectedTest, TransformErrorRValueRefWillReturnReboundValueIfHasValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<CopyableType, CopyableType> unit{wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType(CopyableType&&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling transform_error
    const auto result = std::move(unit).transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, value);
}

TEST(ExpectedTest, TransformErrorConstRValueRefWillCallFunctionIfHasValue)
{
    // Given an expected with an error
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<CompatibleCopyableType(const CopyableType&&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(std::move(wrapped)))
        .WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_value}}));

    // When calling transform_error
    const auto result = std::move(unit).transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().inner_.value_, monad_value);
}

TEST(ExpectedTest, TransformErrorConstRValueRefWillReturnReboundValueIfHasValue)
{
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<CopyableType, CopyableType> unit{wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType(const CopyableType&&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling transform_error
    const auto result = std::move(unit).transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, value);
}

}  // namespace
}  // namespace score::details
