

///
/// @file local_memory_resource.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#include "score/analysis/tracing/library/test/unit_test/local_memory_resource.h"
#include "score/memory/shared/memory_resource_registry.h"

namespace score
{
namespace analysis
{
namespace tracing
{

std::uint64_t LocalMemoryResource::instance_id_ = 0U;
const std::string kPath = "/dev/path";

LocalMemoryResource::LocalMemoryResource(std::pair<void*, void*> memory_range) noexcept
    : base_address_{memory_range.first},
      usable_base_address_{memory_range.first},
      end_address_{memory_range.second},
      is_allocation_possible_{true},
      save_first_alloc_as_usable_base_{true},
      allocated_memory_{0U},
      memory_resource_id_{instance_id_++},
      manager_{memory_resource_id_}
{
}

const score::memory::shared::MemoryResourceProxy* LocalMemoryResource::getMemoryResourceProxy() noexcept
{
    score::memory::shared::MemoryResourceRegistry::getInstance().clear();
    score::memory::shared::MemoryResourceRegistry::getInstance().insert_resource({memory_resource_id_, this});
    return &this->manager_;
}

void* LocalMemoryResource::getBaseAddress() const noexcept
{
    return base_address_;
}

void* LocalMemoryResource::getUsableBaseAddress() const noexcept
{
    return usable_base_address_;
}

const void* LocalMemoryResource::getEndAddress() const noexcept
{
    return end_address_;
}

// TODO (piotr.chodorowski@partner.bmw.de): Investigate and fix tsan detected problems with usage of aligned_alloc and
// change malloc to aligned_alloc
void* LocalMemoryResource::do_allocate(std::size_t number_of_bytes, std::size_t)
{
    void* result = nullptr;
    if (is_allocation_possible_)
    {
        result = std::malloc(number_of_bytes);
        if (nullptr != result)
        {
            allocated_memory_ += number_of_bytes;
            if (save_first_alloc_as_usable_base_)
            {
                usable_base_address_ = result;
                save_first_alloc_as_usable_base_ = false;
            }
        }
    }
    return result;
}

void LocalMemoryResource::do_deallocate(void* memory, std::size_t number_of_bytes, std::size_t)
{
    allocated_memory_ -= number_of_bytes;
    std::free(memory);
}

bool LocalMemoryResource::do_is_equal(const memory_resource& other) const noexcept
{
    return &other == this;
}

std::size_t LocalMemoryResource::GetUserAllocatedBytes() const noexcept
{
    return allocated_memory_;
}
const std::string* LocalMemoryResource::getPath() const noexcept
{
    return &kPath;
}
void LocalMemoryResource::UnlinkFilesystemEntry() const noexcept
{
    return;
}
std::int32_t LocalMemoryResource::GetFileDescriptor() const noexcept
{
    return 0;
}
bool LocalMemoryResource::IsShmInTypedMemory() const noexcept
{
    return true;
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
