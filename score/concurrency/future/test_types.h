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
#ifndef BASELIBS_SCORE_CONCURRENCY_FUTURE_TEST_TYPES_H
#define BASELIBS_SCORE_CONCURRENCY_FUTURE_TEST_TYPES_H

namespace score
{
namespace concurrency
{
namespace testing
{

class CopyAndMovableType
{
  public:
    explicit CopyAndMovableType(int value);

    int GetValue() const noexcept;

  private:
    int value_;
};

class CopyOnlyType : public CopyAndMovableType
{
  public:
    using CopyAndMovableType::CopyAndMovableType;
    CopyOnlyType(const CopyOnlyType&) noexcept = default;
    CopyOnlyType& operator=(const CopyOnlyType&) noexcept = default;
    CopyOnlyType(CopyOnlyType&&) noexcept = delete;
    CopyOnlyType& operator=(CopyOnlyType&&) noexcept = delete;

    ~CopyOnlyType() noexcept = default;
};

class MoveOnlyType : public CopyAndMovableType
{
  public:
    using CopyAndMovableType::CopyAndMovableType;
    MoveOnlyType(const MoveOnlyType&) noexcept = delete;
    MoveOnlyType& operator=(const MoveOnlyType&) noexcept = delete;
    MoveOnlyType(MoveOnlyType&&) noexcept = default;
    MoveOnlyType& operator=(MoveOnlyType&&) noexcept = default;

    ~MoveOnlyType() noexcept = default;
};

}  // namespace testing
}  // namespace concurrency
}  // namespace score

#endif  // BASELIBS_SCORE_CONCURRENCY_FUTURE_TEST_TYPES_H
