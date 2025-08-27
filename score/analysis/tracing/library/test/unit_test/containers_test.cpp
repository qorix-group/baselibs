///
/// @file shm_object_handle_container_test.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API shm_object_handle_container test source
///
#include "score/analysis/tracing/library/generic_trace_api/containers/client_id/client_id_container.h"
#include "score/analysis/tracing/library/generic_trace_api/containers/shm_object_handle/shm_object_handle_container.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using testing::An;
using testing::AtLeast;
using testing::ByMove;
using testing::Eq;
using ::testing::WithArgs;

namespace score
{
namespace analysis
{
namespace tracing
{

TEST(Containers, ShmObjectHandleContainerTestConstFindIf)
{
    auto const_container = std::make_unique<const ShmObjectHandleContainer>();
    auto res = const_container->FindIf([](const auto&) {
        return false;
    });
    EXPECT_FALSE(res.has_value());

    auto elem = (*const_container)[0].get();
    EXPECT_EQ(elem.local_handle_, kInvalidSharedObjectIndex);
}

TEST(Containers, ClientIdContainerTestoperators)
{
    auto const_container = std::make_unique<const ClientIdContainer>();
    auto elem = (*const_container)[0];
    EXPECT_EQ(elem.get().client_id_, kInvalidTraceClientId);
}

TEST(Containers, ClientIdContainerTestAcquiringOverSize)
{
    auto _container = std::make_unique<AtomicContainer<std::string, 20>>();
    for (int i = 0; i < 20; i++)
    {
        auto res = _container->Acquire();
        EXPECT_TRUE(res.has_value());
    }
    auto res = _container->Acquire();
    EXPECT_FALSE(res.has_value());
}

TEST(Containers, ClientIdContainerSetAndResetErrorValidClientId)
{
    auto const_container = std::make_unique<ClientIdContainer>();
    static constexpr AppIdType app_id{"app_id"};
    auto trace_client_id = const_container->RegisterLocalTraceClient(BindingType::kFirst, app_id);
    EXPECT_TRUE(trace_client_id.has_value());
    auto client = const_container->GetTraceClientById(trace_client_id.value().get().local_client_id_);
    EXPECT_TRUE(client.has_value());
    const_container->SetClientError(ErrorCode::kGenericErrorRecoverable,
                                    trace_client_id.value().get().local_client_id_);
    const_container->ResetClientError(trace_client_id.value().get().local_client_id_);
    client = const_container->GetTraceClientById(trace_client_id.value().get().local_client_id_);
    EXPECT_TRUE(client.has_value());
}

TEST(Containers, ClientIdContainerSetAndResetErrorInvalidClientId)
{
    auto const_container = std::make_unique<ClientIdContainer>();
    const TraceClientId trace_client_id{1};
    auto client = const_container->GetTraceClientById(trace_client_id);
    EXPECT_FALSE(client.has_value());
    const_container->SetClientError(ErrorCode::kGenericErrorRecoverable, trace_client_id);
    const_container->ResetClientError(trace_client_id);
    client = const_container->GetTraceClientById(trace_client_id);
    EXPECT_FALSE(client.has_value());
}

TEST(Containers, AtomicContainerTestReleae)
{
    auto atomicContainer = std::make_unique<AtomicContainer<std::string, 20>>();
    atomicContainer->Release("temp");
    auto spot = atomicContainer->Acquire();
    spot.value().get() = "temp";
    atomicContainer->Release(1000);
    atomicContainer->Release(0);
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
