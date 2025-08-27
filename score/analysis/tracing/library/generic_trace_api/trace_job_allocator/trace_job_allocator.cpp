///
/// @file trace_job_allocator.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#include "score/analysis/tracing/library/generic_trace_api/trace_job_allocator/trace_job_allocator.h"
#include "score/analysis/tracing/common/interface_types/shared_memory_location_helpers.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_container/trace_job_container.h"
#include "score/analysis/tracing/plugin/ipc_trace_plugin/interface/ara_com_meta_info_trace_format.h"
#include "platform/aas/mw/time/HWLoggerTime/receiver/plp_serializer.h"
#include "platform/aas/mw/time/HighPrecisionLocalSteadyClock/high_precision_local_steady_clock.h"
#include <netinet/in.h>
#include <utility>

static_assert(sizeof(score::mw::time::HighPrecisionLocalSteadyClock::time_point) == 8, "Wrong timestamp size");

namespace
{
bool FillRingBufferElement(score::analysis::tracing::detail::ElementReferenceType element,
                           score::analysis::tracing::TraceClientId client_id,
                           score::analysis::tracing::TraceContextId context_id,
                           score::analysis::tracing::TraceJobType type,
                           score::analysis::tracing::SharedMemoryLocation chunk_list,
                           std::shared_ptr<score::analysis::tracing::TraceJobContainer> container)
{
    element.value().get().global_context_id_ = {client_id, context_id};
    element.value().get().chunk_list_ = chunk_list;
    element.value().get().status_ = score::analysis::tracing::TraceJobStatus::kReady;

    return container->Add({element.value(), {client_id, context_id}, type, element.value().get().chunk_list_});
}
}  // namespace

namespace score
{
namespace analysis
{
namespace tracing
{

constexpr std::size_t locally_created_elements = 2u;

// No harm from define that varaible here a3-3-1,a3-3-2
//  coverity[autosar_cpp14_a3_3_2_violation]
//  coverity[autosar_cpp14_a3_3_1_violation]
std::shared_ptr<TraceJobContainer> globalContainer = std::make_shared<TraceJobContainer>();
// No harm from intializing the constructor in that way
//  coverity[autosar_cpp14_a12_1_5_violation]
TraceJobAllocator::TraceJobAllocator()
    : ITraceJobAllocator(),
      container_{globalContainer},
      trace_metadata_memory_resource_{nullptr},
      trace_metadata_memory_handle_{0},
      ring_buffer_{nullptr},
      job_id_{0U},
      flexible_allocator_{nullptr},
      logger_time_{nullptr},
      logger_time_available_{false}
{
}

TraceJobAllocator::TraceJobAllocator(std::shared_ptr<TraceJobContainer> container,
                                     ResourcePointer tmd_memory_resource,
                                     std::shared_ptr<IFlexibleCircularAllocator> flexible_allocator,
                                     std::unique_ptr<IShmRingBuffer> ring_buffer,
                                     LoggerTime logger_time)
    : ITraceJobAllocator(),
      container_{container},
      trace_metadata_memory_resource_{tmd_memory_resource},
      trace_metadata_memory_handle_{0},
      ring_buffer_{std::move(ring_buffer)},
      job_id_{0U},
      flexible_allocator_{flexible_allocator},
      logger_time_{logger_time},
      logger_time_available_{logger_time_.has_value()}

{
    if (((nullptr == trace_metadata_memory_resource_) || (nullptr == ring_buffer_)) ||
        (-1 == trace_metadata_memory_handle_))
    {
        return;
    }
}

void TraceJobAllocator::SetTraceMetaDataShmObjectHandle(ShmObjectHandle handle)
{
    trace_metadata_memory_handle_ = handle;
}

TraceResult TraceJobAllocator::AllocateShmJob(const TraceClientId client,
                                              const MetaInfoVariants::type& meta_info,
                                              const BindingType binding_type,
                                              const AppIdType& app_instance_id,
                                              ShmDataChunkList& data,
                                              TraceContextId context_id)
{
    const auto element = ring_buffer_->GetEmptyElement();
    if (!element.has_value())
    {
        return Unexpected{element.error()};
    }

    // No harm to declare void pointer
    //  coverity[autosar_cpp14_a0_1_1_violation]
    void* meta_info_trace_format_ptr = nullptr;
    if (score::cpp::holds_alternative<AraComMetaInfo>(meta_info))
    {
        // No harm from casting void pointer
        //  coverity[autosar_cpp14_m5_2_8_violation]
        meta_info_trace_format_ptr = static_cast<AraComMetaInfoTraceFormat*>(
            flexible_allocator_->Allocate(sizeof(AraComMetaInfoTraceFormat), alignof(std::max_align_t)));
        if (nullptr == meta_info_trace_format_ptr)
        {
            element.value().get().status_ = score::analysis::tracing::TraceJobStatus::kInvalid;
            return score::MakeUnexpected(ErrorCode::kNotEnoughMemoryRecoverable);
        }

        // NOLINTBEGIN(score-no-dynamic-raw-memory) Usage of placement new is intended here to allocate vector with
        // custom allocator correctely in the shared memory.
        const auto meta_info_trace_format = new (meta_info_trace_format_ptr)
            AraComMetaInfoTraceFormat{score::cpp::get<AraComMetaInfo>(meta_info), binding_type, app_instance_id};
        // NOLINTEND(score-no-dynamic-raw-memory) Usage of placement new is intended here to allocate vector with
        //  custom allocator correctely in the shared memory.
        const auto trace_meta_data_offset =
            GetOffsetFromPointer(meta_info_trace_format, trace_metadata_memory_resource_);
        if (trace_meta_data_offset.has_value())
        {
            SharedMemoryChunk trace_meta_data_chunk{{trace_metadata_memory_handle_, trace_meta_data_offset.value()},
                                                    sizeof(AraComMetaInfoTraceFormat)};
            data.AppendFront(trace_meta_data_chunk);
        }
        else
        {
            element.value().get().status_ = score::analysis::tracing::TraceJobStatus::kInvalid;
            return score::MakeUnexpected(ErrorCode::kNotEnoughMemoryRecoverable);
        }
    }
    else
    {
        return score::MakeUnexpected(ErrorCode::kNoMetaInfoProvidedRecoverable);
    }
    // clang-format off
    // No harm from casting void pointer
    //  coverity[autosar_cpp14_m5_2_8_violation]
    auto timestamp_ptr =static_cast<std::uint64_t*>(flexible_allocator_->Allocate(sizeof(std::uint64_t), alignof(std::max_align_t)));
    // clang-format on
    if (nullptr == timestamp_ptr)
    {
        element.value().get().status_ = score::analysis::tracing::TraceJobStatus::kInvalid;
        return score::MakeUnexpected(ErrorCode::kNotEnoughMemoryRecoverable);
    }

    // update the timestamp
    *timestamp_ptr = GetTimeStamp();
    const auto offset = GetOffsetFromPointer(timestamp_ptr, trace_metadata_memory_resource_);
    if (offset.has_value())
    {
        SharedMemoryChunk timestamp{{trace_metadata_memory_handle_, offset.value()},
                                    sizeof(score::mw::time::HighPrecisionLocalSteadyClock::time_point)};
        data.AppendFront(timestamp);
    }
    else
    {
        element.value().get().status_ = score::analysis::tracing::TraceJobStatus::kInvalid;
        return score::MakeUnexpected(ErrorCode::kNotEnoughMemoryRecoverable);
    }
    auto chunk_list_result =
        data.SaveToSharedMemory(trace_metadata_memory_resource_, trace_metadata_memory_handle_, flexible_allocator_);
    if (!chunk_list_result.has_value())
    {
        element.value().get().status_ = score::analysis::tracing::TraceJobStatus::kInvalid;
        return Unexpected{chunk_list_result.error()};
    }

    if (!FillRingBufferElement(
            element, client, context_id, TraceJobType::kShmJob, chunk_list_result.value(), container_))
    {
        auto vector =
            GetPointerFromLocation<ShmChunkVector>(chunk_list_result.value(), trace_metadata_memory_resource_);
        // No need to check against nullptr in the following line due to the check against the size
        // otherwise, it yields to unreachable code
        // coverity[autosar_cpp14_a5_3_2_violation] See above
        vector->clear();
        element.value().get().status_ = score::analysis::tracing::TraceJobStatus::kInvalid;
        return score::MakeUnexpected(ErrorCode::kNotEnoughMemoryRecoverable);
    }

    return score::cpp::blank{};
}

TraceResult TraceJobAllocator::AllocateLocalJob(const TraceClientId client,
                                                const MetaInfoVariants::type& meta_info,
                                                const BindingType binding_type,
                                                const AppIdType& app_instance_id,
                                                LocalDataChunkList& data)
{
    const auto element = ring_buffer_->GetEmptyElement();
    if (!element.has_value())
    {
        return Unexpected{element.error()};
    }

    if (!score::cpp::holds_alternative<AraComMetaInfo>(meta_info))
    {
        return score::MakeUnexpected(ErrorCode::kNoMetaInfoProvidedRecoverable);
    }

    AraComMetaInfoTraceFormat meta_info_trace_format =
        AraComMetaInfoTraceFormat{score::cpp::get<AraComMetaInfo>(meta_info), binding_type, app_instance_id};
    LocalDataChunk trace_meta_data_chunk{&meta_info_trace_format, sizeof(AraComMetaInfoTraceFormat)};
    data.AppendFront(trace_meta_data_chunk);

    // update the timestamp
    std::uint64_t timestamp_value = GetTimeStamp();

    LocalDataChunk timestamp{&timestamp_value, sizeof(score::mw::time::HighPrecisionLocalSteadyClock::time_point)};
    data.AppendFront(timestamp);

    auto chunk_list_result =
        data.SaveToSharedMemory(trace_metadata_memory_resource_, trace_metadata_memory_handle_, flexible_allocator_);
    if (!chunk_list_result.has_value())
    {
        element.value().get().status_ = score::analysis::tracing::TraceJobStatus::kInvalid;
        return Unexpected{chunk_list_result.error()};
    }

    if (!FillRingBufferElement(
            element, client, job_id_, TraceJobType::kLocalJob, chunk_list_result.value(), container_))
    {
        auto vector =
            GetPointerFromLocation<ShmChunkVector>(chunk_list_result.value(), trace_metadata_memory_resource_);
        // No need to check against nullptr in the following line due to the check against the size
        // otherwise, it yields to unreachable code
        // coverity[autosar_cpp14_a5_3_2_violation] See above
        vector->clear();
        element.value().get().status_ = score::analysis::tracing::TraceJobStatus::kInvalid;
        return score::MakeUnexpected(ErrorCode::kNotEnoughMemoryRecoverable);
    }
    job_id_++;
    return score::cpp::blank{};
}

TraceResult TraceJobAllocator::DeallocateJob(const SharedMemoryLocation chunk_list, const TraceJobType job_type)
{
    if (chunk_list.shm_object_handle_ != trace_metadata_memory_handle_)
    {
        return score::MakeUnexpected(ErrorCode::kWrongHandleRecoverable);
    }
    auto vector = GetPointerFromLocation<ShmChunkVector>(chunk_list, trace_metadata_memory_resource_);
    if (nullptr == vector)
    {
        return score::MakeUnexpected(ErrorCode::kInvalidArgumentFatal);
    }
    const std::size_t elements_to_deallocate =
        job_type == TraceJobType::kShmJob ? locally_created_elements : vector->size();

    for (std::size_t i = 0U; i < elements_to_deallocate; i++)
    {
        auto el = vector->at(i);
        // False decision can not be covered by UTs because conditions above guard vector index to exceed vector size
        if (el.has_value())  // LCOV_EXCL_BR_LINE not testable see comment above.
        {
            void* data_ptr = GetPointerFromLocation<void>(el.value().start_, trace_metadata_memory_resource_);
            // No harm from ignorning the returned value here
            // coverity[autosar_cpp14_a0_1_2_violation]
            flexible_allocator_->Deallocate(data_ptr, el.value().size_);
        }
    }

    vector->clear();
    // No harm from ignorning the returned value here
    // coverity[autosar_cpp14_a0_1_2_violation]
    flexible_allocator_->Deallocate(vector, sizeof(vector));
    return score::cpp::blank{};
}

std::uint64_t TraceJobAllocator::GetTimeStamp()
{
    // get the Time Point in ePLP format only if the HW Timer is initialized successfully
    if (logger_time_.has_value())
    {
        const auto plp_time_point = logger_time_.value()->Now();
        return score::mw::time::hwloggertime::serialize2plp(plp_time_point);
    }
    else
    {
        // In case the timer is not available ie. the logger_time_.init() fails at the startup
        // the timestamp field should be all set with zeroes.
        // No harm to do the return in that way
        // coverity[autosar_cpp14_a5_2_2_violation]
        return std::uint64_t{0};
    }
}
bool TraceJobAllocator::IsLoggerTimeAvailable()
{
    return logger_time_available_;
}

void TraceJobAllocator::CloseRingBuffer() noexcept
{
    ring_buffer_->Close();
}

void TraceJobAllocator::ResetRingBuffer() noexcept
{
    ring_buffer_->Reset();
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
