///
/// @file shm_ring_buffer_consumer.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"
#include "score/analysis/tracing/shm_ring_buffer/shm_ring_buffer.h"
#include "score/memory/shared/shared_memory_factory.h"
#include <unistd.h>
#include <iostream>

using namespace score::analysis::tracing;

constexpr char buffer_shm_path[] = "/tmd_buffer";
constexpr std::uint32_t elements_count = 90u;

int main(int argc, char* argv[])
{
    std::uint32_t i = 0u, break_cnt = 0u;
    std::cout << "debug(\"TRC\"): Consumer started" << std::endl;

    auto tmd_pool_ = std::make_shared<ShmRingBuffer>(buffer_shm_path, 10);
    auto result = tmd_pool_->CreateOrOpen();
    if (!result.has_value())
    {
        std::cout << "debug(\"TRC\"): Failed to create TMD" << std::endl;
    }

    sleep(1);

    do
    {
        auto element = tmd_pool_->GetReadyElement();
        if (element.has_value())
        {
            break_cnt = 0;
            element.value().get().status_ = TraceJobStatus::kEmpty;
            std::cout << "debug(\"TRC\"): Read: " << element.value().get().global_context_id_.context_id_ << std::endl;
            i++;
        }
        else
        {
            std::cout << "debug(\"TRC\"): No ready element: " << element.error() << std::endl;
            break_cnt++;
        }
        if (break_cnt == 5)
        {
            break;
        }
        usleep(50000);
    } while (i != elements_count);

    // Verify if buffer is empty
    auto element = tmd_pool_->GetReadyElement();
    if (element.has_value())
    {
        std::cout << "debug(\"TRC\"): Unexpected ready element!" << std::endl;
    }
    else if (element.error() != ErrorCode::kRingBufferEmptyRecoverable)
    {
        std::cout << "debug(\"TRC\"): Unexpected error while checking for empty buffer!" << element.error().Message()
                  << std::endl;
    }

    const auto statistics = tmd_pool_->GetStatistics();
    if (statistics.has_value())
    {
        std::cout << std::endl << "###### STATISTICS ######" << std::endl;
        std::cout << "###### producer cas_trials_:\t\t" << statistics.value().producer_.cas_trials_ << std::endl;
        std::cout << "###### producer cas_failures_:\t\t" << statistics.value().producer_.cas_failures_ << std::endl;
        std::cout << "###### producer call_count_:\t\t" << statistics.value().producer_.call_count_ << std::endl;
        std::cout << "###### producer call_failure_count_:\t" << statistics.value().producer_.call_failure_count_
                  << std::endl;
        std::cout << "###### producer buffer_full_count_:\t" << statistics.value().producer_.buffer_full_count_
                  << std::endl;

        std::cout << "###### consumer cas_trials_:\t\t" << statistics.value().consumer_.cas_trials_ << std::endl;
        std::cout << "###### consumer cas_failures_:\t\t" << statistics.value().consumer_.cas_failures_ << std::endl;
        std::cout << "###### consumer call_count_:\t\t" << statistics.value().consumer_.call_count_ << std::endl;
        std::cout << "###### consumer call_failure_count_:\t" << statistics.value().consumer_.call_failure_count_
                  << std::endl;
        std::cout << "###### consumer buffer_empty_count_:\t" << statistics.value().consumer_.buffer_empty_count_
                  << std::endl;
        std::cout << "###### STATISTICS ######" << std::endl << std::endl;
    }

    score::memory::shared::SharedMemoryFactory::Remove(buffer_shm_path);

    std::cout << "debug(\"TRC\"): Finished!" << std::endl;

    return 0;
}
