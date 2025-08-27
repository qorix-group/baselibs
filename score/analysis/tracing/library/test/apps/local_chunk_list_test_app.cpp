///
/// @file chunk_list_test_app.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#include "score/analysis/tracing/common/flexible_circular_allocator/lockless_flexible_circular_allocator.h"
#include "score/analysis/tracing/common/interface_types/shared_memory_location_helpers.h"
#include "score/analysis/tracing/library/generic_trace_api/chunk_list/local_data_chunk_list.h"
#include "score/analysis/tracing/library/generic_trace_api/chunk_list/shm_data_chunk_list.h"
#include "score/memory/shared/shared_memory_factory.h"
#include <unistd.h>
#include <cstring>
#include <iostream>

using namespace score::analysis::tracing;
using namespace score::memory::shared;
constexpr char buffer_shm_path[] = "/chunk_list_shm";
constexpr std::size_t allocated_mem_size = 65535u;

void print_bytes(void* input, std::size_t size)
{
    const unsigned char* p = reinterpret_cast<const unsigned char*>(input);
    std::cout << std::hex << std::showbase;
    std::cout << "[";
    for (std::size_t i = 0; i < size; i++)
    {
        std::cout << static_cast<int>(*(p++)) << " ";
    }
    std::cout << "]" << std::endl;
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    std::cout << "debug(\"TRC\"): Local chunk list test app started" << std::endl;

    auto shm =
        score::memory::shared::SharedMemoryFactory::CreateOrOpen(buffer_shm_path, [](auto&&) {}, allocated_mem_size);
    if ((nullptr == shm) || (nullptr == shm->getBaseAddress()))
    {
        std::cout << "debug(\"TRC\"): Failed to create shm" << std::endl;
        return 0;
    }

    std::uint8_t* memory_block_ptr =
        static_cast<std::uint8_t*>(shm->allocate(allocated_mem_size, alignof(std::max_align_t)));
    auto flexible_allocator_ =
        std::make_shared<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>>(memory_block_ptr, allocated_mem_size);

    LocalDataChunk local_chunk;
    LocalDataChunkList local_data_chunk_list;
    std::size_t total_size = 0u;
    for (std::size_t i = 0; i < 10; i++)
    {
        total_size += i + 10u;
    }
    std::uint8_t* data = static_cast<std::uint8_t*>(malloc(total_size));
    if (nullptr == data)
    {
        std::cout << "debug(\"TRC\"): Malloc failed" << std::endl;
        return 0;
    }
    std::uint8_t* pointer = data;
    for (std::uint8_t i = 0; i < 10; i++)
    {
        std::size_t size = i + 10u;
        memset(pointer, i, size);
        local_chunk.size_ = size;
        local_chunk.start_ = pointer;
        local_data_chunk_list.Append(local_chunk);
        pointer += size;
    }

    if (local_data_chunk_list.Size() != 10)
    {
        std::cout << "debug(\"TRC\"): Wrong list size" << std::endl;
        return 0;
    }

    std::cout << "debug(\"TRC\"): Created list" << std::endl;

    auto result = local_data_chunk_list.SaveToSharedMemory(shm, 1, flexible_allocator_);

    if (local_data_chunk_list.Size() != 10)
    {
        std::cout << "debug(\"TRC\"): Wrong list size" << std::endl;
        return 0;
    }
    local_data_chunk_list.Clear();

    std::cout << "debug(\"TRC\"): List saved to shared memory" << std::endl;

    // Just to make sure there is no evidence of local data
    memset(data, 0xff, total_size);
    free(data);

    std::cout << "debug(\"TRC\"): Local memory overwrite and free" << std::endl;

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
            void* data_ptr = GetPointerFromLocation<void>(el.start_, shm);
            print_bytes(data_ptr, el.size_);
            shm->getMemoryResourceProxy()->deallocate(data_ptr, el.size_);
        }
        vector->clear();
        shm->getMemoryResourceProxy()->deallocate(vector, sizeof(ShmChunkVector));
    }
    else
    {
        std::cout << result.error() << std::endl;
    }

    std::cout << "debug(\"TRC\"): Test finished" << std::endl;

    return 0;
}
