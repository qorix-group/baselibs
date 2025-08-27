///
/// @file chunk_list_test_app.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#include "score/analysis/tracing/common/flexible_circular_allocator/lockless_flexible_circular_allocator.h"
#include "score/analysis/tracing/common/interface_types/shared_memory_location_helpers.h"
#include "score/analysis/tracing/library/generic_trace_api/chunk_list/shm_data_chunk_list.h"
#include "score/memory/shared/shared_memory_factory.h"
#include <unistd.h>
#include <iostream>

using namespace score::analysis::tracing;
using namespace score::memory::shared;

constexpr char buffer_shm_path[] = "/chunk_list_shm";
constexpr std::size_t allocated_mem_size = 65535u;

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    std::cout << "debug(\"TRC\"): Shm chunk list test app started" << std::endl;

    auto shm =
        score::memory::shared::SharedMemoryFactory::CreateOrOpen(buffer_shm_path, [](auto&&) {}, allocated_mem_size);
    if ((shm == nullptr) || (shm->getBaseAddress() == nullptr))
    {
        std::cout << "debug(\"TRC\"): Failed to create shm" << std::endl;
        return 0;
    }

    std::uint8_t* memory_block_ptr =
        static_cast<std::uint8_t*>(shm->allocate(allocated_mem_size, alignof(std::max_align_t)));
    auto flexible_allocator_ =
        std::make_shared<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>>(memory_block_ptr, allocated_mem_size);

    SharedMemoryLocation shm_offset_ptr{0, 0};
    SharedMemoryChunk shm_chunk;
    ShmDataChunkList shm_data_chunk_list;
    for (std::size_t i = 0; i < 10; i++)
    {
        shm_offset_ptr.offset_ = i;
        shm_offset_ptr.shm_object_handle_ = static_cast<std::int32_t>(i);
        shm_chunk.size_ = i;
        shm_chunk.start_ = shm_offset_ptr;
        shm_data_chunk_list.Append(shm_chunk);
    }

    if (shm_data_chunk_list.Size() != 10)
    {
        std::cout << "debug(\"TRC\"): Wrong list size" << std::endl;
        return 0;
    }

    std::cout << "debug(\"TRC\"): Created list" << std::endl;

    auto result = shm_data_chunk_list.SaveToSharedMemory(shm, 1, flexible_allocator_);

    if (shm_data_chunk_list.Size() != 10)
    {
        std::cout << "debug(\"TRC\"): Wrong list size" << std::endl;
        return 0;
    }
    shm_data_chunk_list.Clear();

    std::cout << "debug(\"TRC\"): List saved to shared memory" << std::endl;

    if (result.has_value())
    {
        ShmChunkVector* vector = GetPointerFromLocation<ShmChunkVector>(result.value(), shm);

        if (nullptr == vector)
        {
            std::cout << "debug(\"TRC\"): vector == nulptr" << std::endl;
            return 0;
        }

        for (std::size_t i = 0; i < vector->size(); i++)
        {
            auto el = vector->at(i).value();
            std::cout << i << ": " << el.size_ << " " << el.start_.offset_ << " " << el.start_.shm_object_handle_
                      << std::endl;
        }
        vector->~List();
        shm->getMemoryResourceProxy()->deallocate(vector, sizeof(ShmChunkVector));
    }
    else
    {
        std::cout << result.error() << std::endl;
    }

    std::cout << "debug(\"TRC\"): Test finished" << std::endl;

    return 0;
}
