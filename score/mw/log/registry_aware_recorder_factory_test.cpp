/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
#include "score/mw/log/registry_aware_recorder_factory.h"

#include "score/mw/log/backend_table.h"
#include "score/mw/log/detail/empty_recorder.h"
#include "score/mw/log/detail/text_recorder/text_recorder.h"
#include "score/mw/log/irecorder_factory.h"

#include "gtest/gtest.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
namespace
{

template <typename ConcreteRecorder>
bool IsRecorderOfType(const std::unique_ptr<Recorder>& recorder) noexcept
{
    static_assert(std::is_base_of<Recorder, ConcreteRecorder>::value,
                  "Concrete recorder shall be derived from Recorder base class");
    return dynamic_cast<const ConcreteRecorder*>(recorder.get()) != nullptr;
}

class RegistryAwareRecorderFactoryTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // Save original table state so console_registrant's static registration persists.
        saved_table_ = gBackendCreators;
    }

    void TearDown() override
    {
        // Restore the original table state.
        gBackendCreators = saved_table_;
    }

    std::array<RecorderCreatorFn, kMaxBackendSlots> saved_table_{};
};

TEST(CreateRecorderFactoryTest, CreateRecorderFactoryReturnsNonNullFactory)
{
    RecordProperty("Description", "CreateRecorderFactory shall return a valid IRecorderFactory pointer.");
    RecordProperty("TestingTechnique", "Interface-based test");

    auto factory = CreateRecorderFactory();

    EXPECT_NE(factory, nullptr);
}

TEST(CreateRecorderFactoryTest, CreateStubReturnsEmptyRecorder)
{
    RecordProperty("Description", "CreateRecorderFactory()->CreateStub() shall return an EmptyRecorder.");
    RecordProperty("TestingTechnique", "Interface-based test");

    auto factory = CreateRecorderFactory();
    auto recorder = factory->CreateStub();

    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RegistryAwareRecorderFactoryTest, CreateWithConsoleLoggingOnlyReturnsRecorder)
{
    RecordProperty("Description",
                   "CreateWithConsoleLoggingOnly shall return a valid Recorder when console backend is registered.");
    RecordProperty("TestingTechnique", "Requirements-based test");

    // Console backend is registered via the static BackendRegistrant in console_registrant.cpp
    // which is linked as part of the :minimal target.
    RegistryAwareRecorderFactory factory;
    auto recorder = factory.CreateWithConsoleLoggingOnly(score::cpp::pmr::get_default_resource());

    EXPECT_NE(recorder, nullptr);
    EXPECT_TRUE(IsRecorderOfType<TextRecorder>(recorder));
}

TEST_F(RegistryAwareRecorderFactoryTest, CreateWithConsoleLoggingOnlyWithNullResourceFallsBackToDefault)
{
    RecordProperty("Description", "CreateWithConsoleLoggingOnly shall use default memory resource when given nullptr.");
    RecordProperty("TestingTechnique", "Requirements-based test");

    RegistryAwareRecorderFactory factory;
    auto recorder = factory.CreateWithConsoleLoggingOnly(nullptr);

    EXPECT_NE(recorder, nullptr);
    EXPECT_TRUE(IsRecorderOfType<TextRecorder>(recorder));
}

TEST_F(RegistryAwareRecorderFactoryTest, CreateWithConsoleLoggingOnlyFallsBackToEmptyIfConsoleNotRegistered)
{
    RecordProperty("Description",
                   "CreateWithConsoleLoggingOnly shall return EmptyRecorder if console backend is not registered.");
    RecordProperty("TestingTechnique", "Boundary test");

    // Clear the backend table to simulate no console backend.
    gBackendCreators.fill(nullptr);

    RegistryAwareRecorderFactory factory;
    auto recorder = factory.CreateWithConsoleLoggingOnly(score::cpp::pmr::get_default_resource());

    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RegistryAwareRecorderFactoryTest, CreateStubReturnsEmptyRecorder)
{
    RecordProperty("Description", "CreateStub shall return an EmptyRecorder.");
    RecordProperty("TestingTechnique", "Interface-based test");

    RegistryAwareRecorderFactory factory;
    auto recorder = factory.CreateStub();

    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RegistryAwareRecorderFactoryTest, CreateFromConfigurationReturnsNullptrWhenMemoryResourceIsNull)
{
    RecordProperty("Description", "CreateFromConfiguration shall return nullptr when given a null memory resource.");
    RecordProperty("TestingTechnique", "Boundary test");

    RegistryAwareRecorderFactory factory;
    auto recorder = factory.CreateFromConfiguration(nullptr);

    EXPECT_EQ(recorder, nullptr);
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
