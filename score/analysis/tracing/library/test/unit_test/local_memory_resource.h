
///
/// @file local_memory_resource.h
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#ifndef GENERIC_TRACE_API_LOCAL_MEMORY_RESOURCE_H
#define GENERIC_TRACE_API_LOCAL_MEMORY_RESOURCE_H

#include "score/memory/shared/i_shared_memory_resource.h"
#include "score/memory/shared/memory_resource_proxy.h"

namespace score
{
namespace analysis
{
namespace tracing
{

class LocalMemoryResource : public score::memory::shared::ISharedMemoryResource
{
  public:
    LocalMemoryResource(std::pair<void*, void*> memory_range = {
                            reinterpret_cast<void*>(1),
                            reinterpret_cast<void*>(std::numeric_limits<std::uint64_t>::max())}) noexcept;
    const score::memory::shared::MemoryResourceProxy* getMemoryResourceProxy() noexcept override;
    void* getBaseAddress() const noexcept override;
    void* getUsableBaseAddress() const noexcept override;

    void* const base_address_;
    void* usable_base_address_;
    void* const end_address_;
    bool is_allocation_possible_;
    bool save_first_alloc_as_usable_base_;
    std::size_t allocated_memory_;
    const std::uint64_t memory_resource_id_;
    score::memory::shared::MemoryResourceProxy manager_;

  private:
    const void* getEndAddress() const noexcept override;
    void* do_allocate(std::size_t number_of_bytes, std::size_t) override;
    void do_deallocate(void* memory, std::size_t number_of_bytes, std::size_t) override;
    bool do_is_equal(const memory_resource& other) const noexcept override;
    std::size_t GetUserAllocatedBytes() const noexcept override;
    const std::string* getPath() const noexcept override;
    void UnlinkFilesystemEntry() const noexcept override;
    std::int32_t GetFileDescriptor() const noexcept override;
    bool IsShmInTypedMemory() const noexcept override;

    static std::uint64_t instance_id_;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // GENERIC_TRACE_API_LOCAL_MEMORY_RESOURCE_H
