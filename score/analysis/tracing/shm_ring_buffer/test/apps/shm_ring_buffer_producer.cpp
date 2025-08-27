///
/// @file shm_ring_buffer_producer.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#include "score/analysis/tracing/shm_ring_buffer/shm_ring_buffer.h"
#include "score/memory/shared/shared_memory_factory.h"
#include <unistd.h>
#include <iostream>

using namespace score::analysis::tracing;

constexpr char buffer_shm_path[] = "/tmd_buffer";
constexpr std::uint32_t elements_count = 30u;

int main(int argc, char* argv[])
{
    std::uint32_t i = 0u, break_cnt = 0u;
    std::cout << "debug(\"TRC\"): Producer started" << std::endl;

    auto tmd_pool_ = std::make_shared<ShmRingBuffer>(buffer_shm_path, 10);
    auto result = tmd_pool_->CreateOrOpen();
    if (!result.has_value())
    {
        std::cout << "debug(\"TRC\"): Failed to Open TMD" << std::endl;
        return 0;
    }

    do
    {
        auto element = tmd_pool_->GetEmptyElement();
        if (element.has_value())
        {
            element.value().get().global_context_id_ = {0, i};
            element.value().get().status_ = TraceJobStatus::kReady;
            std::cout << "debug(\"TRC\"): Enqueued: " << i << std::endl;
            i++;
            break_cnt = 0;
        }
        else
        {
            std::cout << "debug(\"TRC\"): Failed to enqueue: " << element.error() << std::endl;
            break_cnt++;
            if (break_cnt == 20)
            {
                break;
            }
        }
        usleep(100000);
    } while (i < elements_count);

    std::cout << "debug(\"TRC\"): Finished!" << std::endl;

    return 0;
}
