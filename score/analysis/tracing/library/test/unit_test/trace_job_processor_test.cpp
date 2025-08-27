///
/// @file daemon_communicator_test.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Generic Trace API daemon communicator test source file
///

#include "score/analysis/tracing/library/generic_trace_api/trace_job_processor/trace_job_processor.h"
#include "score/analysis/tracing/common/interface_types/types.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"
#include "score/analysis/tracing/library/test/unit_test/mocks/mock_trace_job_container.h"
#include "score/language/safecpp/scoped_function/scope.h"
#include "score/memory/shared/atomic_mock.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <atomic>
#include <thread>

using ::testing::_;
using ::testing::StrictMock;
using namespace score::analysis::tracing;
using ::testing::AtLeast;
using ::testing::Return;
using namespace score::memory::shared;
class TraceJobProcessorFixture : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        container_ = std::make_shared<TraceJobContainer>();
        mock_container_ = std::make_shared<MockTraceJobContainer>();
    }

    void TearDown() override
    {
        processor_.reset();
        container_.reset();
        stop_source_.request_stop();
    }
    bool AddTraceJob(const TraceJobType job_type, const TraceClientId trace_client_id);
    void AddMatchingTraceJob(const TraceJobType job_type);
    bool AddTraceJobInThread(ShmRingBufferElement& element, const TraceClientId trace_client_id);

    std::shared_ptr<TraceJobContainer> container_;
    std::shared_ptr<MockTraceJobContainer> mock_container_;
    std::unique_ptr<ITraceJobProcessor> processor_;
    ClientIdContainer client_id_container_;
    ShmRingBufferElement ring_buffer_element_{};
    ShmRingBufferElement ring_buffer_element_2_{};
    static constexpr TraceClientId client_id_{1};
    static constexpr TraceClientId jobs_to_process_bundle_size{17};
    static constexpr AppIdType app_id_{"LOLA"};
    std::atomic<std::uint16_t> processed_jobs{0};
    std::atomic<std::uint16_t> added_jobs{0};
    static constexpr std::size_t thread_count = 10;
    static constexpr std::size_t container_overflow_count = 40;
    ShmRingBufferElement* ring_elements;
    score::cpp::stop_source stop_source_{};
    static constexpr TraceClientId client_id_2_{2};
    static constexpr ShmObjectHandle object_handle_{1};
};
bool TraceJobProcessorFixture::AddTraceJob(const TraceJobType job_type,
                                           const TraceClientId trace_client_id = client_id_)
{
    static constexpr TraceContextId context_id_{2};
    static constexpr TraceContextId finished_context_id_{1};
    static constexpr std::size_t offset_{0};
    GlobalTraceContextId global_trace_context_id{trace_client_id, context_id_};
    SharedMemoryLocation chunk_list{object_handle_, offset_};
    ring_buffer_element_.global_context_id_ = {client_id_, finished_context_id_};
    ring_buffer_element_.chunk_list_ = chunk_list;
    ring_buffer_element_.status_ = TraceJobStatus::kEmpty;
    return container_->Add({std::reference_wrapper<ShmRingBufferElement>(ring_buffer_element_),
                            global_trace_context_id,
                            job_type,
                            chunk_list});
}

void TraceJobProcessorFixture::AddMatchingTraceJob(const TraceJobType job_type)
{
    static constexpr TraceContextId context_id_{1};
    static constexpr TraceContextId finished_context_id_{1};
    static constexpr std::size_t offset_{0};

    GlobalTraceContextId global_trace_context_id{client_id_, context_id_};
    SharedMemoryLocation chunk_list{object_handle_, offset_};
    ring_buffer_element_2_.global_context_id_ = {client_id_, finished_context_id_};
    ring_buffer_element_2_.chunk_list_ = chunk_list;
    ring_buffer_element_2_.status_ = TraceJobStatus::kEmpty;

    container_->Add({std::reference_wrapper<ShmRingBufferElement>(ring_buffer_element_2_),
                     global_trace_context_id,
                     job_type,
                     chunk_list});
}

bool TraceJobProcessorFixture::AddTraceJobInThread(ShmRingBufferElement& element, const TraceClientId trace_client_id)
{
    static constexpr TraceContextId context_id_{1};
    static constexpr TraceContextId finished_context_id_{0};
    static constexpr std::size_t offset_{0};

    GlobalTraceContextId global_trace_context_id{trace_client_id, context_id_};
    SharedMemoryLocation chunk_list{object_handle_, offset_};
    element.global_context_id_ = {client_id_, finished_context_id_};
    element.chunk_list_ = chunk_list;
    element.status_ = TraceJobStatus::kEmpty;

    return container_->Add({std::reference_wrapper<ShmRingBufferElement>(element),
                            global_trace_context_id,
                            TraceJobType::kShmJob,
                            chunk_list});
}

TEST_F(TraceJobProcessorFixture, SaveCallbackSuccess)
{
    processor_ = std::make_unique<TraceJobProcessor<AtomicIndirectorReal>>(
        client_id_container_, container_, nullptr, stop_source_.get_token());
    const auto save_callback_result = processor_->SaveCallback(
        client_id_,
        score::analysis::tracing::TraceDoneCallBackType{score::safecpp::Scope<>{}, [](TraceContextId) noexcept {}});
    EXPECT_TRUE(save_callback_result.has_value());
}

TEST_F(TraceJobProcessorFixture, SaveCallbackNull)
{
    // empty callback
    score::safecpp::MoveOnlyScopedFunction<void(uint32_t)> function;

    processor_ = std::make_unique<TraceJobProcessor<AtomicIndirectorReal>>(
        client_id_container_, container_, nullptr, stop_source_.get_token());
    const auto save_callback_result = processor_->SaveCallback(client_id_, std::move(function));
    EXPECT_FALSE(save_callback_result.has_value());
}

TEST_F(TraceJobProcessorFixture, SaveCallbackClientAlreadyExists)
{
    processor_ = std::make_unique<TraceJobProcessor<AtomicIndirectorReal>>(
        client_id_container_, container_, nullptr, stop_source_.get_token());
    auto save_callback_result = processor_->SaveCallback(
        client_id_,
        score::analysis::tracing::TraceDoneCallBackType{score::safecpp::Scope<>{}, [](TraceContextId) noexcept {}});
    EXPECT_TRUE(save_callback_result.has_value());

    save_callback_result = processor_->SaveCallback(
        client_id_,
        score::analysis::tracing::TraceDoneCallBackType{score::safecpp::Scope<>{}, [](TraceContextId) noexcept {}});
    EXPECT_FALSE(save_callback_result.has_value());
    EXPECT_EQ(save_callback_result.error(), ErrorCode::kCallbackAlreadyRegisteredRecoverable);
}

TEST_F(TraceJobProcessorFixture, NoFreeSlotToSaveCallback)
{
    processor_ = std::make_unique<TraceJobProcessor<AtomicIndirectorReal>>(
        client_id_container_, container_, nullptr, stop_source_.get_token());

    for (std::uint8_t i = 0U; i < kCallbackMaxNumber; i++)
    {
        auto save_callback_result = processor_->SaveCallback(
            static_cast<TraceClientId>(client_id_ + i),
            score::analysis::tracing::TraceDoneCallBackType{score::safecpp::Scope<>{}, [](TraceContextId) noexcept {}});
        EXPECT_TRUE(save_callback_result.has_value());
    }

    auto result = processor_->SaveCallback(
        static_cast<TraceClientId>(client_id_ + kCallbackMaxNumber),
        score::analysis::tracing::TraceDoneCallBackType{score::safecpp::Scope<>{}, [](TraceContextId) noexcept {}});
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kNoFreeSlotToSaveTheCallbackRecoverable);
}

TEST_F(TraceJobProcessorFixture, ProcessJobsNoDeallocatorRegistered)
{
    processor_ = std::make_unique<TraceJobProcessor<AtomicIndirectorReal>>(
        client_id_container_, container_, nullptr, stop_source_.get_token());
    const auto process_jobs_result = processor_->ProcessJobs();
    EXPECT_FALSE(process_jobs_result.has_value());
    EXPECT_EQ(process_jobs_result.error(), ErrorCode::kNoDeallocatorCallbackRegisteredFatal);
}

TEST_F(TraceJobProcessorFixture, ProcessShmTracedJobNoCallbackRegistered)
{
    EXPECT_TRUE(AddTraceJob(TraceJobType::kShmJob));
    processor_ = std::make_unique<TraceJobProcessor<AtomicIndirectorReal>>(
        client_id_container_,
        container_,
        [](SharedMemoryLocation, TraceJobType) noexcept -> score::ResultBlank {
            return {};
        },
        stop_source_.get_token());

    const auto size_before_processing = container_->GetEmptyElementsCount();
    const auto process_jobs_result = processor_->ProcessJobs();
    EXPECT_TRUE(process_jobs_result.has_value());
    const auto size_after_processing = container_->GetEmptyElementsCount();
    EXPECT_NE(size_after_processing, size_before_processing);
}

TEST_F(TraceJobProcessorFixture, TraceJobAllocatorShallStopWhenStopRequestedWhileReleasingElementsFromContainer)
{
    EXPECT_TRUE(AddTraceJob(TraceJobType::kLocalJob));
    auto element = container_->GetReadyElement();  // Get the element from the real container
    auto mock_container_raw = mock_container_.get();

    processor_ = std::make_unique<TraceJobProcessor<AtomicIndirectorReal>>(
        client_id_container_,
        std::move(mock_container_),
        [](SharedMemoryLocation, TraceJobType) noexcept -> score::ResultBlank {
            return {};
        },
        stop_source_.get_token());

    ON_CALL(*mock_container_raw, GetReadyElement()).WillByDefault(Return(element.value()));
    ON_CALL(*mock_container_raw, ReleaseReadyElement()).WillByDefault(Return(false));

    std::thread delayedThread([this]() {
        // Pause for 1 milliseconds
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        this->stop_source_.request_stop();
    });

    std::ignore = processor_->ProcessJobs();
    delayedThread.join();
}

TEST_F(TraceJobProcessorFixture, ReleaseFromEmptyContainer)
{
    processor_ = std::make_unique<TraceJobProcessor<AtomicIndirectorReal>>(
        client_id_container_,
        container_,
        [](SharedMemoryLocation, TraceJobType) noexcept -> score::ResultBlank {
            return {};
        },
        stop_source_.get_token());
    const auto size_before_processing = container_->GetEmptyElementsCount();
    container_->ReleaseReadyElement();
    const auto size_after_processing = container_->GetEmptyElementsCount();
    EXPECT_EQ(size_after_processing, size_before_processing);
}

TEST_F(TraceJobProcessorFixture, ProcessLocalJobSuccess)
{
    EXPECT_TRUE(AddTraceJob(TraceJobType::kLocalJob));
    processor_ = std::make_unique<TraceJobProcessor<AtomicIndirectorReal>>(
        client_id_container_,
        container_,
        [](SharedMemoryLocation, TraceJobType) noexcept -> score::ResultBlank {
            return {};
        },
        stop_source_.get_token());

    const auto size_before_processing = container_->GetEmptyElementsCount();

    const auto process_jobs_result = processor_->ProcessJobs();
    const auto size_after_processing = container_->GetEmptyElementsCount();
    EXPECT_TRUE(process_jobs_result.has_value());
    EXPECT_NE(size_after_processing, size_before_processing);
}

TEST_F(TraceJobProcessorFixture, ProcessLocalJobSuccessNoElements)
{
    processor_ = std::make_unique<TraceJobProcessor<AtomicIndirectorReal>>(
        client_id_container_,
        container_,
        [](SharedMemoryLocation, TraceJobType) noexcept -> score::ResultBlank {
            return {};
        },
        stop_source_.get_token());

    const auto process_jobs_result = processor_->ProcessJobs();
    EXPECT_TRUE(process_jobs_result.has_value());
}

TEST_F(TraceJobProcessorFixture, ProcessTraceJobsDeallocationFailedCallbackShallReleaseElementFromRingBuffer)
{
    EXPECT_TRUE(AddTraceJob(TraceJobType::kShmJob));
    processor_ = std::make_unique<TraceJobProcessor<AtomicIndirectorReal>>(
        client_id_container_,
        container_,
        [](SharedMemoryLocation, TraceJobType) noexcept -> score::ResultBlank {
            return score::MakeUnexpected(ErrorCode::kInvalidArgumentFatal);
        },
        stop_source_.get_token());

    const auto size_before_processing = container_->GetEmptyElementsCount();

    const auto process_jobs_result = processor_->ProcessJobs();
    const auto size_after_processing = container_->GetEmptyElementsCount();
    EXPECT_FALSE(process_jobs_result.has_value());
    EXPECT_EQ(process_jobs_result.error(), ErrorCode::kInvalidArgumentFatal);
    EXPECT_NE(size_after_processing, size_before_processing);
}

TEST_F(TraceJobProcessorFixture, ProcessLocalJobMultipleJobs)
{
    AddMatchingTraceJob(TraceJobType::kShmJob);
    EXPECT_TRUE(AddTraceJob(TraceJobType::kShmJob));
    processor_ = std::make_unique<TraceJobProcessor<AtomicIndirectorReal>>(
        client_id_container_,
        container_,
        [](SharedMemoryLocation, TraceJobType) noexcept -> score::ResultBlank {
            return {};
        },
        stop_source_.get_token());

    score::cpp::ignore = processor_->SaveCallback(
        client_id_2_,
        score::analysis::tracing::TraceDoneCallBackType{score::safecpp::Scope<>{}, [](TraceContextId) noexcept {}});
    score::cpp::ignore = processor_->SaveCallback(
        client_id_,
        score::analysis::tracing::TraceDoneCallBackType{score::safecpp::Scope<>{}, [](TraceContextId) noexcept {}});

    const auto size_before_processing = container_->GetEmptyElementsCount();

    const auto process_jobs_result = processor_->ProcessJobs();
    const auto size_after_processing = container_->GetEmptyElementsCount();
    EXPECT_TRUE(process_jobs_result.has_value());
    EXPECT_EQ(size_after_processing, size_before_processing);
}

TEST_F(TraceJobProcessorFixture, CleanPendingJobsDeallocationFailedCallback)
{
    processor_ = std::make_unique<TraceJobProcessor<AtomicIndirectorReal>>(
        client_id_container_, container_, nullptr, stop_source_.get_token());

    const auto process_jobs_result = processor_->CleanPendingJobs();
    EXPECT_FALSE(process_jobs_result.has_value());
    EXPECT_EQ(process_jobs_result.error(), ErrorCode::kNoDeallocatorCallbackRegisteredFatal);
}

TEST_F(TraceJobProcessorFixture, CleanPendingJobs)
{
    processor_ = std::make_unique<TraceJobProcessor<AtomicIndirectorReal>>(
        client_id_container_,
        container_,
        [](SharedMemoryLocation, TraceJobType) noexcept -> score::ResultBlank {
            return {};
        },
        stop_source_.get_token());
    auto number_of_added_jobs = 0U;
    score::cpp::ignore = processor_->SaveCallback(
        client_id_2_,
        score::analysis::tracing::TraceDoneCallBackType{score::safecpp::Scope<>{}, [](TraceContextId) noexcept {}});
    score::cpp::ignore = processor_->SaveCallback(
        client_id_,
        score::analysis::tracing::TraceDoneCallBackType{score::safecpp::Scope<>{}, [](TraceContextId) noexcept {}});

    AddMatchingTraceJob(TraceJobType::kShmJob);
    number_of_added_jobs++;
    EXPECT_TRUE(AddTraceJob(TraceJobType::kShmJob));
    number_of_added_jobs++;
    const auto size_before_processing = container_->GetEmptyElementsCount();

    const auto process_jobs_result = processor_->CleanPendingJobs();
    const auto size_after_processing = container_->GetEmptyElementsCount();
    EXPECT_TRUE(process_jobs_result.has_value());
    EXPECT_EQ(size_after_processing, size_before_processing + number_of_added_jobs);
}

TEST_F(TraceJobProcessorFixture, ProcessJobsContainerVerify)
{
    processor_ = std::make_unique<TraceJobProcessor<AtomicIndirectorReal>>(
        client_id_container_,
        container_,
        [](SharedMemoryLocation, TraceJobType) noexcept -> score::ResultBlank {
            return {};
        },
        stop_source_.get_token());

    auto client_id_element = client_id_container_.RegisterLocalTraceClient(BindingType::kLoLa, app_id_);
    client_id_element.value().get().local_client_id_ = client_id_;

    const score::safecpp::Scope<> event_receive_handler_scope{};
    TraceDoneCallBackType trace_done_callback{event_receive_handler_scope,
                                              [this](TraceContextId trace_context_id) noexcept {
                                                  score::cpp::ignore = trace_context_id;
                                                  processed_jobs++;
                                              }};
    score::cpp::ignore = processor_->SaveCallback(client_id_, std::move(trace_done_callback));

    // we simulate LTPM Daemon client registration
    client_id_element.value().get().client_id_ = client_id_ + 100;

    EXPECT_EQ(container_->GetEmptyElementsCount(), container_->Size());
    EXPECT_EQ(processed_jobs, 0);
    for (std::size_t i = 0; i < container_->Size(); i++)
    {
        std::uint16_t processed_jobs_before = processed_jobs;
        for (std::size_t j = 0; j < jobs_to_process_bundle_size; j++)
        {
            EXPECT_TRUE(AddTraceJob(TraceJobType::kShmJob, client_id_element.value().get().client_id_));
        }

        auto process_jobs_result = processor_->ProcessJobs();
        std::uint16_t processed_jobs_after = processed_jobs;
        EXPECT_EQ(processed_jobs_before + jobs_to_process_bundle_size, processed_jobs_after);
        process_jobs_result = processor_->ProcessJobs();
        EXPECT_EQ(processed_jobs_after, processed_jobs);
    }
}

TEST_F(TraceJobProcessorFixture, ProcessValidElementsWithNoCallback)
{
    processor_ = std::make_unique<TraceJobProcessor<AtomicIndirectorReal>>(
        client_id_container_,
        container_,
        [](SharedMemoryLocation, TraceJobType) noexcept -> score::ResultBlank {
            return {};
        },
        stop_source_.get_token());

    auto client_id_element = client_id_container_.RegisterLocalTraceClient(BindingType::kLoLa, app_id_);
    client_id_element.value().get().local_client_id_ = client_id_;

    // we simulate LTPM Daemon client registration
    client_id_element.value().get().client_id_ = client_id_ + 100;

    EXPECT_EQ(container_->GetEmptyElementsCount(), container_->Size());

    for (std::size_t j = 0; j < jobs_to_process_bundle_size; j++)
    {
        EXPECT_TRUE(AddTraceJob(TraceJobType::kShmJob, client_id_element.value().get().client_id_));
    }

    auto process_jobs_result = processor_->ProcessJobs();
    EXPECT_TRUE(process_jobs_result.has_value());
}

TEST_F(TraceJobProcessorFixture, DeallocateElementShallDoNothingWhenAtomicUpdateFailsForMaxRetries)
{
    std::unique_ptr<AtomicMock<bool>> atomic_bool_mock_;
    auto mock_processor = std::make_unique<TraceJobProcessor<AtomicIndirectorMock>>(
        client_id_container_,
        container_,
        [](SharedMemoryLocation, TraceJobType) noexcept -> score::ResultBlank {
            return {};
        },
        stop_source_.get_token());
    ;
    atomic_bool_mock_ = std::make_unique<AtomicMock<bool>>();

    EXPECT_EQ(AtomicIndirectorMock<bool>::GetMockObject(), nullptr);
    AtomicIndirectorMock<bool>::SetMockObject(atomic_bool_mock_.get());
    EXPECT_EQ(AtomicIndirectorMock<bool>::GetMockObject(), atomic_bool_mock_.get());

    EXPECT_CALL(*atomic_bool_mock_, compare_exchange_weak(_, _, _, _)).WillRepeatedly(Return(false));

    auto client_id_element = client_id_container_.RegisterLocalTraceClient(BindingType::kLoLa, app_id_);
    client_id_element.value().get().local_client_id_ = client_id_;

    // we simulate LTPM Daemon client registration
    client_id_element.value().get().client_id_ = client_id_ + 100;

    EXPECT_EQ(container_->GetEmptyElementsCount(), container_->Size());

    for (std::size_t j = 0; j < 1; j++)
    {
        EXPECT_TRUE(AddTraceJob(TraceJobType::kShmJob, client_id_element.value().get().client_id_));
    }
    std::thread delaying_thread([this]() {
        // Pause for 1 milliseconds to be able to make atomic update fail with max retries.
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        this->stop_source_.request_stop();
    });
    auto process_jobs_result = mock_processor->ProcessJobs();
    EXPECT_TRUE(process_jobs_result.has_value());
    AtomicIndirectorMock<bool>::SetMockObject(nullptr);
    EXPECT_EQ(AtomicIndirectorMock<bool>::GetMockObject(), nullptr);
    delaying_thread.join();
}

TEST_F(TraceJobProcessorFixture, ProcessJobsMultithread)
{
    processor_ = std::make_unique<TraceJobProcessor<AtomicIndirectorReal>>(
        client_id_container_,
        container_,
        [](SharedMemoryLocation, TraceJobType) noexcept -> score::ResultBlank {
            return {};
        },
        stop_source_.get_token());

    auto client_id_element = client_id_container_.RegisterLocalTraceClient(BindingType::kLoLa, app_id_);
    client_id_element.value().get().local_client_id_ = client_id_;

    const score::safecpp::Scope<> event_receive_handler_scope{};
    TraceDoneCallBackType trace_done_callback{event_receive_handler_scope,
                                              [this](TraceContextId trace_context_id) noexcept {
                                                  score::cpp::ignore = trace_context_id;
                                                  processed_jobs++;
                                              }};
    std::thread producer_threads[thread_count], consumer_thread;
    std::size_t elements_count = container_->Size() * container_overflow_count;
    std::size_t elements_count_per_thread = elements_count / thread_count;
    ring_elements = new ShmRingBufferElement[elements_count];

    score::cpp::ignore = processor_->SaveCallback(client_id_, std::move(trace_done_callback));

    // we simulate LTPM Daemon client registration
    client_id_element.value().get().client_id_ = client_id_ + 100;

    EXPECT_EQ(processed_jobs, 0);
    EXPECT_EQ(added_jobs, 0);
    EXPECT_EQ(container_->GetEmptyElementsCount(), container_->Size());

    auto producer = [this, elements_count_per_thread, client_id_element](std::size_t index) noexcept {
        for (std::size_t i = 0; i < elements_count_per_thread; i++)
        {
            while (AddTraceJobInThread(ring_elements[i + elements_count_per_thread * index],
                                       client_id_element.value().get().client_id_) != true)
                ;
            added_jobs++;
        }
    };
    auto consumer = [this, elements_count]() noexcept {
        while (processed_jobs != elements_count)
        {

            auto result = processor_->ProcessJobs();
            EXPECT_TRUE(result.has_value());
        }
    };
    for (std::size_t i = 0; i < thread_count; i++)
    {
        producer_threads[i] = std::thread{producer, i};
    }
    consumer_thread = std::thread{consumer};

    for (std::size_t i = 0; i < thread_count; i++)
    {
        producer_threads[i].join();
    }
    consumer_thread.join();

    EXPECT_EQ(processed_jobs, container_->Size() * container_overflow_count);
    EXPECT_EQ(added_jobs, container_->Size() * container_overflow_count);

    delete[] ring_elements;
}
