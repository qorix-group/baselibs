///
/// @file test_app.cpp
/// @copyright Copyright (C) 2025, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#include "score/analysis/tracing/common/interface_types/shared_memory_location_helpers.h"
#include "score/analysis/tracing/library/interface/generic_trace_api.h"
#include "score/language/safecpp/scoped_function/scope.h"
#include "score/memory/shared/shared_memory_factory.h"
#include "score/os/unistd.h"

#include <sys/mman.h>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>

using namespace score::analysis::tracing;

static constexpr auto kTraceRetrySleep{std::chrono::milliseconds{100}};
static score::memory::shared::MemoryResourceRegistry& instance =
    score::memory::shared::MemoryResourceRegistry::getInstance();
std::shared_ptr<score::memory::shared::ISharedMemoryResource> memory_resource{nullptr};
ShmObjectHandle shm_handle;
constexpr int kError{-1};
bool shm_unregister_before_callback = false;

const TracePointType trace_point_type{TracePointType::SKEL_EVENT_SND};
ServiceInstanceElement service_instance_element{
    0xA,
    0xA,
    0xA,
    0xA,
    ServiceInstanceElement::VariantType{score::cpp::in_place_type<ServiceInstanceElement::EventIdType>,
                                        ServiceInstanceElement::EventIdType{0U}}};
const AraComProperties::TracePointDataId trace_point_data_id{0xBB};
const AraComProperties properties{trace_point_type, service_instance_element, trace_point_data_id};
const AraComMetaInfo meta_info{properties};

int trace_shm_job(const TraceClientId trace_client_id, char* chunk_data, const std::uint16_t chunk_size)
{
    static std::condition_variable trace_completed_cond;
    static std::mutex mtx;
    static bool callback_called = false;

    const score::safecpp::Scope<> event_receive_handler_scope{};
    score::analysis::tracing::TraceDoneCallBackType trace_done_callback{event_receive_handler_scope,
                                                                      [](TraceContextId) noexcept {
                                                                          std::lock_guard<std::mutex> lock(mtx);
                                                                          callback_called = true;
                                                                          trace_completed_cond.notify_one();
                                                                      }};

    const auto register_callback_result =
        GenericTraceAPI::RegisterTraceDoneCB(trace_client_id, std::move(trace_done_callback));
    if (!register_callback_result.has_value())
    {
        std::cout << "debug(\"TRC\"): Could not register trace done callback" << std::endl;
        return kError;
    }

    void* data_pointer = memory_resource->getMemoryResourceProxy()->allocate(chunk_size, alignof(std::max_align_t));
    if (nullptr == data_pointer)
    {
        std::cout << "debug(\"TRC\"): ERROR: Failed to allocate chunk memory" << std::endl;
        return kError;
    }

    off64_t phys_addr;
    if (mem_offset64(data_pointer, NOFD, 1, &phys_addr, NULL) != -1)
    {
        std::cout << "debug(\"TRC\"): phys_addr: " << phys_addr << std::endl;
    }

    memcpy(static_cast<std::uint8_t*>(data_pointer), chunk_data, chunk_size);
    SharedMemoryChunk shm_chunk{};
    shm_chunk.size_ = chunk_size;
    shm_chunk.start_.shm_object_handle_ = shm_handle;
    shm_chunk.start_.offset_ = GetOffsetFromPointer(data_pointer, memory_resource).value();

    ShmDataChunkList shm_data_chunk_list{shm_chunk};
    const TraceContextId trace_context_id{0x8765};
    constexpr auto kRetryMax{3U};
    TraceResult trace_result;
    std::size_t retry_cntr = 0;
    int retVal = 0;

    while (++retry_cntr < kRetryMax)
    {
        trace_result = GenericTraceAPI::Trace(trace_client_id, meta_info, shm_data_chunk_list, trace_context_id);
        if (trace_result.has_value())
        {
            break;
        }
        std::cout << "debug(\"TRC\"): Trace() retry #" << retry_cntr << std::endl;
        std::this_thread::sleep_for(kTraceRetrySleep);
    }

    if (!trace_result.has_value())
    {
        std::cout << "debug(\"TRC\"): ERROR: Failed to trace: " << trace_result.error() << std::endl;
        retVal = kError;
    }
    else
    {
        if (shm_unregister_before_callback)
        {
            auto unregister_result = GenericTraceAPI::UnregisterShmObject(trace_client_id, shm_handle);
            if (!unregister_result.has_value())
            {
                retVal = kError;
                std::cout << "debug(\"TRC\"): ERROR: Failed to UnregisterShmObject: " << unregister_result.error()
                          << std::endl;
            }
        }
        std::unique_lock<std::mutex> lock(mtx);
        trace_completed_cond.wait(lock, [] {
            return callback_called;
        });
    }

    memory_resource->getMemoryResourceProxy()->deallocate(data_pointer, chunk_size);
    shm_data_chunk_list.Clear();

    std::cout << "debug(\"TRC\"): Shm Trace completed with result: " << retVal << std::endl;
    return retVal;
}

int trace_local_job(const std::string& app_instance_identifier)
{
    constexpr std::size_t iteration_count = 5U;
    constexpr std::size_t chunk_count = 1U;
    constexpr std::size_t chunk_size = 2000U;

    auto trace_client_local_id_result = GenericTraceAPI::RegisterClient(BindingType::kVector, app_instance_identifier);

    if (!trace_client_local_id_result.has_value())
    {
        std::cout << "debug(\"LIB\"): Failed to register local client" << std::endl;
        return kError;
    }
    const TraceClientId trace_client_id = trace_client_local_id_result.value();

    LocalDataChunkList local_data_chunk_list{};
    std::list<void*> local_list{};
    static std::uint8_t byte = 0xFF;
    for (std::size_t i = 0; i < iteration_count; i++)
    {
        ++byte;
        for (std::size_t j = 0; j < chunk_count; j++)
        {
            LocalDataChunk local_chunk{};
            void* data_pointer = std::malloc(chunk_size);
            memset(data_pointer, trace_client_id, sizeof(TraceClientId));
            memset(static_cast<std::uint8_t*>(data_pointer) + 1, byte, chunk_size - 1);
            local_chunk.size_ = chunk_size;
            local_chunk.start_ = data_pointer;
            local_data_chunk_list.Append(local_chunk);
            local_list.push_back(data_pointer);
        }
        auto trace_result = GenericTraceAPI::Trace(trace_client_id, meta_info, local_data_chunk_list);
        std::list<void*>::iterator it = local_list.begin();
        static const std::uint32_t kMaxRetries = 100;
        std::size_t current_try = 0;
        while (!trace_result.has_value() && current_try < kMaxRetries &&
               trace_result.error() == ErrorCode::kDaemonNotConnectedRecoverable)
        {
            trace_result = GenericTraceAPI::Trace(trace_client_id, meta_info, local_data_chunk_list);
            if (trace_result.has_value())
            {
                break;
            }
            ++current_try;
            std::cout << "debug(\"LIB\"): Daemon is not connected yet. Try " << current_try << " out of " << kMaxRetries
                      << std::endl;
            std::this_thread::sleep_for(kTraceRetrySleep);
        }
        if (!trace_result.has_value())
        {
            std::cout << "debug(\"TRC\"): Trace error: " << trace_result.error() << std::endl;
        }

        while (it != local_list.end())
        {
            std::free(*it);
            local_list.erase(it++);
        }
        local_data_chunk_list.Clear();
        std::this_thread::sleep_for(std::chrono::milliseconds{50});
    }
    std::cout << "debug(\"TRC\"): Local Trace completed with success" << std::endl;
    return 0;
}

int LoadDataFromBinFile(const char* path, char* out_buff, const std::uint16_t buff_size) noexcept
{
    std::cout << "debug(\"TRC\"): Trying to open: " << path << std::endl;
    std::ifstream inputFile(path, std::ios_base::binary);
    if (inputFile.is_open() && (nullptr != out_buff))
    {
        inputFile.unsetf(std::ios::skipws);
        inputFile.seekg(0, std::ios::end);
        std::streampos fileSize = inputFile.tellg();
        if (fileSize != buff_size)
        {
            std::cout << "debug(\"TRC\"): File size: " << fileSize << " not equal to expected: " << buff_size
                      << std::endl;
            return kError;
        }
        inputFile.seekg(0, std::ios::beg);
        inputFile.read(out_buff, fileSize);
        return 0;
    }
    return kError;
}

int main(int argc, char* argv[])
{
    const auto process_id = score::os::Unistd::instance().getpid();
    const std::string kTraceClientAppIdZeroCopy{"TstZCopy_"};
    const std::string shared_memory_path = "/dev_client_" + std::to_string(process_id);
    constexpr std::size_t kShmSize{256 * 1024U};
    constexpr std::size_t kBuffSize{1000U};
    static std::array<char, kBuffSize> buffer;

    if (argc > 1)
    {
        char first_arg_beggining = argv[1][0];
        char* bin_file_path = argv[1];
        if (first_arg_beggining == '-')
        {
            std::cout << "debug(\"TRC\"): Unregister ShmObject before callback test" << std::endl;
            shm_unregister_before_callback = true;
        }
        else if (LoadDataFromBinFile(bin_file_path, buffer.data(), buffer.size()) != 0)
        {
            std::cout << "debug(\"TRC\"): Filed to read data from file:" << argv[1] << " -> using constant payload"
                      << std::endl;
        }
        else
        {
            std::cout << "debug(\"TRC\"): Data loaded from: " << argv[1] << std::endl;
        }
    }

    score::memory::shared::SharedMemoryFactory::WorldWritable permissions{};
    score::memory::shared::SharedMemoryFactory::SetTypedMemoryProvider(score::memory::shared::TypedMemory::Default());
    memory_resource = score::memory::shared::SharedMemoryFactory::Create(
        shared_memory_path, [](auto&&) {}, kShmSize, {permissions}, true);

    if (nullptr == memory_resource)
    {
        std::cout << "debug(\"TRC\"): Failed to open memory resource" << std::endl;
        score::memory::shared::SharedMemoryFactory::Remove(shared_memory_path);
        return kError;
    }

    if (!memory_resource->IsShmInTypedMemory())
    {
        std::cout << "debug(\"TRC\"): Shared memory not in typed memory" << std::endl;
        score::memory::shared::SharedMemoryFactory::Remove(shared_memory_path);
        return kError;
    }

    auto trace_client_zerocopy_id_result =
        GenericTraceAPI::RegisterClient(BindingType::kLoLa, kTraceClientAppIdZeroCopy + std::to_string(process_id));

    if (!trace_client_zerocopy_id_result.has_value())
    {
        std::cout << "debug(\"TRC\"): Failed to register zero-copy client" << std::endl;
        score::memory::shared::SharedMemoryFactory::Remove(shared_memory_path);
        return kError;
    }
    auto trace_client_zerocopy_id = trace_client_zerocopy_id_result.value();

    auto shared_memory_object = GenericTraceAPI::RegisterShmObject(trace_client_zerocopy_id, shared_memory_path);
    if (!shared_memory_object.has_value())
    {
        std::cout << "debug(\"TRC\"): Failed to register shm object" << std::endl;
        score::memory::shared::SharedMemoryFactory::Remove(shared_memory_path);
        return kError;
    }
    shm_handle = shared_memory_object.value();

    if (trace_shm_job(trace_client_zerocopy_id, buffer.data(), buffer.size()) != 0)
    {
        std::cout << "debug(\"TRC\"): Failed to complete the shm tracing" << std::endl;
        score::memory::shared::SharedMemoryFactory::Remove(shared_memory_path);
        return kError;
    }

    if (argc == 1)  // no parameters
    {
        if (trace_local_job("TestCopy_" + std::to_string(process_id)) != 0)
        {
            std::cout << "debug(\"TRC\"): Failed to complete local tracing" << std::endl;
            score::memory::shared::SharedMemoryFactory::Remove(shared_memory_path);
            return kError;
        }
    }
    if (!shm_unregister_before_callback)
    {
        auto unregister_result = GenericTraceAPI::UnregisterShmObject(trace_client_zerocopy_id, shm_handle);
        if (!unregister_result.has_value())
        {
            std::cout << "debug(\"TRC\"): Failed to unregister shm object" << std::endl;
            score::memory::shared::SharedMemoryFactory::Remove(shared_memory_path);
            return kError;
        }
    }

    score::memory::shared::SharedMemoryFactory::Remove(shared_memory_path);

    return 0;
}
