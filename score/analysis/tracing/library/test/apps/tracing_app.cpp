///
/// @file tracing_app.cpp
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#include "score/analysis/tracing/common/interface_types/shared_memory_location_helpers.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"
#include "score/analysis/tracing/library/interface/generic_trace_api.h"
#include "score/language/safecpp/scoped_function/scope.h"
#include "score/memory/shared/shared_memory_factory.h"
#include "score/os/unistd.h"
#include "score/mw/log/logging.h"
#include <score/jthread.hpp>
#include <sys/syspage.h>
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <list>
#include <random>
#include <thread>

using namespace score::analysis::tracing;

static constexpr auto kTraceRetrySleep{std::chrono::milliseconds{100}};

static constexpr auto kMinimumViableSleepMillis = 5;
static constexpr auto kMaximumSleepMillis = 10;
const std::string kTraceClientAppIdCopy = "TstCopy";
const std::string kTraceClientAppIdZeroCopy = "TstZCop";
const auto kError = -1;

std::random_device seeder;
std::mt19937 rng(seeder());
std::uniform_int_distribution<int> gen(kMinimumViableSleepMillis, kMaximumSleepMillis);  // uniform, unbiased

constexpr std::size_t shared_memory_size = 3145728U;
std::uint32_t iteration_count = 30u;
constexpr std::size_t chunk_count = 1;
std::size_t chunk_size = 16000;

TracePointType trace_point_type{TracePointType::SKEL_EVENT_SND};
ServiceInstanceElement service_instance_element{
    0xA,
    0xA,
    0xA,
    0xA,
    ServiceInstanceElement::VariantType{score::cpp::in_place_type<ServiceInstanceElement::EventIdType>,
                                        ServiceInstanceElement::EventIdType{0U}}};
BindingType binding{static_cast<BindingType>(0xCC)};
AraComProperties::TracePointDataId trace_point_data_id{0xBB};
AraComProperties properties{trace_point_type, service_instance_element, trace_point_data_id};
AraComMetaInfo meta_info{properties};
unsigned long number_of_threads;

static score::memory::shared::MemoryResourceRegistry& instance =
    score::memory::shared::MemoryResourceRegistry::getInstance();

// holds the command line options
typedef struct
{
    // flag to created the upper layer shared memory in typed memory
    bool is_registered_memory_typed;
    bool is_reregister_test;
    bool is_concurrent_testing;
    bool is_integrity_testing;
    bool is_pregate_testing;

} options;

std::mutex memory_mutex;
int trace_shm_job(const TraceClientId trace_client_id,
                  const ShmObjectHandle& shm_handle,
                  std::shared_ptr<score::memory::shared::ISharedMemoryResource> memory_resource)
{
    TraceContextId trace_context_id{0x1234};

    ShmDataChunkList shm_data_chunk_list{};
    auto shm_list = std::make_shared<std::list<std::pair<TraceContextId, SharedMemoryChunk>>>();

    const score::safecpp::Scope<> event_receive_handler_scope{};
    score::analysis::tracing::TraceDoneCallBackType trace_done_callback{
        event_receive_handler_scope, [&shm_handle, &memory_resource, &shm_list](TraceContextId context_id) noexcept {
            std::lock_guard<std::mutex> lock{memory_mutex};
            std::list<std::pair<TraceContextId, SharedMemoryChunk>>::iterator i = shm_list->begin();
            while (i != shm_list->end())
            {
                if ((*i).first == context_id)
                {
                    if ((*i).second.start_.shm_object_handle_ != shm_handle)
                    {
                        std::cout << "debug(\"LIB\"): Invalid handle" << std::endl;
                    }
                    void* data_ptr = GetPointerFromLocation<void>((*i).second.start_, memory_resource);
                    memory_resource->getMemoryResourceProxy()->deallocate(data_ptr, (*i).second.size_);
                    shm_list->erase(i++);
                }
                else
                {
                    i++;
                }
            }
        }};
    const auto register_callback_result =
        GenericTraceAPI::RegisterTraceDoneCB(trace_client_id, std::move(trace_done_callback));
    if (!register_callback_result.has_value() &&
        ErrorCode::kCallbackAlreadyRegisteredRecoverable != register_callback_result.error())
    {
        std::cout << "debug(\"LIB\"): Could not register trace done callback with error: "
                  << register_callback_result.error() << std::endl;
        return 1;
    }

    static std::uint8_t byte = 0xFF;
    int result = 0;

    for (std::size_t i = 0; i < iteration_count; i++)
    {
        {
            std::lock_guard<std::mutex> lock{memory_mutex};
            ++byte;
            for (std::size_t j = 0; j < chunk_count; j++)
            {
                SharedMemoryChunk shm_chunk{};
                void* data_pointer =
                    memory_resource->getMemoryResourceProxy()->allocate(chunk_size, alignof(std::max_align_t));
                memset(data_pointer, trace_client_id, sizeof(TraceClientId));
                memset(static_cast<std::uint8_t*>(data_pointer) + 1, byte, chunk_size - 1);
                shm_chunk.size_ = chunk_size;
                shm_chunk.start_.shm_object_handle_ = shm_handle;
                shm_chunk.start_.offset_ = GetOffsetFromPointer(data_pointer, memory_resource).value();
                shm_data_chunk_list.Append(shm_chunk);
                shm_list->push_back({trace_context_id, shm_chunk});
            }
        }

        auto trace_result = GenericTraceAPI::Trace(trace_client_id, meta_info, shm_data_chunk_list, trace_context_id++);
        static const std::uint32_t kMaxRetries = 100;
        std::uint32_t current_try = 0;
        while (!trace_result.has_value() && current_try < kMaxRetries &&
               score::analysis::tracing::IsErrorRecoverable(
                   static_cast<score::analysis::tracing::ErrorCode>(*trace_result.error())))
        {
            trace_result = GenericTraceAPI::Trace(trace_client_id, meta_info, shm_data_chunk_list, trace_context_id++);
            if (trace_result.has_value())
            {
                break;
            }
            std::cout << "debug(\"LIB\"): Recoverable error detected: " << trace_result.error() << current_try + 1
                      << " out of " << kMaxRetries << std::endl;

            ++current_try;
            std::this_thread::sleep_for(kTraceRetrySleep);
        }
        if (!trace_result.has_value())
        {
            std::list<std::pair<TraceContextId, SharedMemoryChunk>>::iterator it = shm_list->begin();
            while (it != shm_list->end())
            {
                if ((*it).first == (trace_context_id - 1))
                {
                    if ((*it).second.start_.shm_object_handle_ != shm_handle)
                    {
                        std::cout << "debug(\"LIB\"): Invalid handle" << std::endl;
                    }
                    void* data_ptr = GetPointerFromLocation<void>((*it).second.start_, memory_resource);
                    memory_resource->getMemoryResourceProxy()->deallocate(data_ptr, (*it).second.size_);
                    shm_list->erase(it++);
                }
                ++it;
            }
            std::cout << "debug(\"LIB\"): Failed to trace: " << trace_result.error() << std::endl;
            result = -1;
        }

        shm_data_chunk_list.Clear();
        std::this_thread::sleep_for(std::chrono::milliseconds{gen(rng)});
    }

    return result;
}

int trace_local_job(const TraceClientId trace_client_id)
{
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
        std::uint32_t current_try = 0;
        while (!trace_result.has_value() && current_try < kMaxRetries &&
               score::analysis::tracing::IsErrorRecoverable(
                   static_cast<score::analysis::tracing::ErrorCode>(*trace_result.error())))
        {
            trace_result = GenericTraceAPI::Trace(trace_client_id, meta_info, local_data_chunk_list);
            if (trace_result.has_value())
            {
                break;
            }
            std::cout << "debug(\"LIB\"): Recoverable error detected: " << trace_result.error() << current_try + 1
                      << " out of " << kMaxRetries << std::endl;
            ++current_try;
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
        std::this_thread::sleep_for(std::chrono::milliseconds{gen(rng)});
    }

    return 0;
}
static int parse_options(int argc, char* argv[], options* result)
{
    int ch;
    memset(result, 0, sizeof(options));
    result->is_registered_memory_typed = true;
    result->is_reregister_test = false;
    result->is_concurrent_testing = false;
    result->is_integrity_testing = false;
    result->is_pregate_testing = false;
    number_of_threads = 0;
    do
    {
        ch = getopt(argc, argv, "isrcpt:");
        switch (ch)
        {
            case -1:
                break;
            case 'i':
                result->is_integrity_testing = true;
                break;
            case 's':
                result->is_registered_memory_typed = false;
                break;
            case 'r':
                result->is_reregister_test = true;
                break;
            case 'c':
                result->is_concurrent_testing = true;
                break;
            case 'p':
                result->is_pregate_testing = true;
                break;
            case 't':
                number_of_threads = std::stoul(optarg);  // Convert argument to integer
                if (number_of_threads > 9)
                {
                    std::cout << "Can't have more than 9 threads" << std::endl;
                    return -1;
                }
                break;
            default:
                return -1;
        }
    } while (ch != (-1));
    return 0;
}

int RegisterClients(unsigned long thread_id,
                    bool is_reregister_test,
                    TraceClientId& trace_local_client_id,
                    TraceClientId& trace_client_zerocopy_id,
                    ShmObjectHandle& shm_handle,
                    const std::string& shared_memory_path)
{
    auto trace_client_zerocopy_id_result = GenericTraceAPI::RegisterClient(
        BindingType::kVectorZeroCopy, kTraceClientAppIdZeroCopy + std::to_string(thread_id));

    if (is_reregister_test)
    {
        trace_client_zerocopy_id_result = GenericTraceAPI::RegisterClient(
            BindingType::kVectorZeroCopy, kTraceClientAppIdZeroCopy + std::to_string(thread_id));
    }

    if (!trace_client_zerocopy_id_result.has_value())
    {
        std::cout << "Thread #" << thread_id << "debug(\"LIB\"): Failed to register zero-copy client" << std::endl;
        score::memory::shared::SharedMemoryFactory::Remove(shared_memory_path);
        return -1;
    }
    trace_client_zerocopy_id = trace_client_zerocopy_id_result.value();

    auto trace_client_local_id_result =
        GenericTraceAPI::RegisterClient(BindingType::kVector, kTraceClientAppIdCopy + std::to_string(thread_id));

    if (!trace_client_local_id_result.has_value())
    {
        std::cout << "Thread #" << thread_id << "debug(\"LIB\"): Failed to register local client" << std::endl;
        score::memory::shared::SharedMemoryFactory::Remove(shared_memory_path);
        return -1;
    }
    trace_local_client_id = trace_client_local_id_result.value();

    auto shared_memory_object = GenericTraceAPI::RegisterShmObject(trace_client_zerocopy_id, shared_memory_path);
    if (!shared_memory_object.has_value())
    {
        std::cout << "Thread #" << thread_id << "debug(\"LIB\"): Failed to register shm object" << std::endl;
        score::memory::shared::SharedMemoryFactory::Remove(shared_memory_path);
        return 2;
    }
    shm_handle = shared_memory_object.value();
    return 0;
}

std::mutex mtx;
std::condition_variable trace_completed_cond;
bool callback_called = false;

int trace_integrity_check(const std::string& shared_memory_path,
                          std::shared_ptr<score::memory::shared::ISharedMemoryResource>& memory_resource)
{
    std::list<std::pair<TraceContextId, SharedMemoryChunk>> shm_list{};
    ShmDataChunkList shm_data_chunk_list{};
    SharedMemoryChunk shm_chunk{};
    const std::string kTraceClientAppId = "TstZCopy_";
    const TraceContextId trace_context_id{0x8765};
    auto process_id = getpid();

    auto trace_client_zerocopy_id_result =
        GenericTraceAPI::RegisterClient(BindingType::kVectorZeroCopy, kTraceClientAppId + std::to_string(process_id));

    if (!trace_client_zerocopy_id_result.has_value())
    {
        std::cout << "debug(\"LIB\"): Failed to register zero-copy client" << std::endl;
        return kError;
    }
    auto trace_client_id = trace_client_zerocopy_id_result.value();
    auto shared_memory_object = GenericTraceAPI::RegisterShmObject(trace_client_id, shared_memory_path);
    if (!shared_memory_object.has_value())
    {
        std::cout << "debug(\"LIB\"): Failed to register shm object" << std::endl;
        return kError;
    }
    auto shm_handle = shared_memory_object.value();

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
        std::cout << "debug(\"LIB\"): Could not register trace done callback" << std::endl;
        GenericTraceAPI::UnregisterShmObject(trace_client_id, shm_handle);
        return kError;
    }

    constexpr std::size_t kChunkCount = 2;
    constexpr std::size_t kChunkSize = 16000;
    constexpr std::uint8_t kPatternByte = 0xAE;

    for (std::size_t j = 0; j < kChunkCount; j++)
    {
        void* data_pointer = memory_resource->getMemoryResourceProxy()->allocate(kChunkSize, alignof(std::max_align_t));
        if (nullptr == data_pointer)
        {
            std::cout << "debug(\"TRC\"): ERROR: Failed to allocate chunk memory" << std::endl;
            GenericTraceAPI::UnregisterShmObject(trace_client_id, shm_handle);
            return kError;
        }
        memcpy(data_pointer, kTraceClientAppId.c_str(), kTraceClientAppId.length());
        memset(static_cast<std::uint8_t*>(data_pointer) + kTraceClientAppId.length(),
               kPatternByte,
               kChunkSize - kTraceClientAppId.length());
        shm_chunk.size_ = kChunkSize;
        shm_chunk.start_.shm_object_handle_ = shm_handle;
        shm_chunk.start_.offset_ = GetOffsetFromPointer(data_pointer, memory_resource).value();
        shm_data_chunk_list.Append(shm_chunk);
        shm_list.push_back({trace_context_id, shm_chunk});
        off64_t phys_addr;
        if (mem_offset64(data_pointer, NOFD, 1, &phys_addr, NULL) != -1)
        {
            std::cout << "debug(\"TRC\"): chunk #" << j << " phys_addr: " << phys_addr << std::endl;
        }
    }

    callback_called = false;
    constexpr std::uint8_t kRetryMax = 3;
    std::uint8_t retry_cntr = 0;
    TraceResult trace_result;

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
    }
    else
    {
        std::unique_lock<std::mutex> lock(mtx);
        trace_completed_cond.wait(lock, [] {
            return callback_called;
        });
    }

    std::cout << "debug(\"TRC\"): TRC app alive!" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    int retVal = 0;
    std::list<std::pair<TraceContextId, SharedMemoryChunk>>::iterator it = shm_list.begin();
    while (it != shm_list.end())
    {
        if ((*it).first == trace_context_id)
        {
            if ((*it).second.start_.shm_object_handle_ != shm_handle)
            {
                std::cout << "debug(\"LIB\"): Invalid handle" << std::endl;
            }
            std::uint8_t* data_ptr = GetPointerFromLocation<std::uint8_t>((*it).second.start_, memory_resource);

            if (retVal == 0)
            {
                if (0 != memcmp(data_ptr, kTraceClientAppId.c_str(), kTraceClientAppId.length()))
                {
                    // failure: memory changed after Trace () call !!!
                    retVal = kError;
                    continue;
                }
                for (std::size_t k = kTraceClientAppId.length(); k < kChunkSize; k++)
                {
                    if (data_ptr[k] != kPatternByte)
                    {
                        // failure: memory changed after Trace () call !!!
                        retVal = kError;
                        break;
                    }
                }
            }
            memory_resource->getMemoryResourceProxy()->deallocate(data_ptr, (*it).second.size_);
            shm_list.erase(it++);
        }
    }

    shm_data_chunk_list.Clear();

    std::this_thread::sleep_for(std::chrono::milliseconds{200});
    GenericTraceAPI::UnregisterShmObject(trace_client_id, shm_handle);

    return retVal;
}

int Trace(unsigned long thread_id,
          const TraceClientId& trace_local_client_id,
          const TraceClientId& trace_client_zerocopy_id,
          const ShmObjectHandle& shm_handle,
          const std::string& shared_memory_path,
          std::shared_ptr<score::memory::shared::ISharedMemoryResource> memory_resource)
{
    if (trace_shm_job(trace_client_zerocopy_id, shm_handle, memory_resource) != 0)
    {
        std::cout << "Thread #" << thread_id << "debug(\"LIB\"): Failed to complete the shm tracing" << std::endl;
        score::memory::shared::SharedMemoryFactory::Remove(shared_memory_path);
        return -1;
    }

    if (trace_local_job(trace_local_client_id) != 0)
    {
        std::cout << "Thread #" << thread_id << "debug(\"LIB\"): Failed to complete the local tracing" << std::endl;
        score::memory::shared::SharedMemoryFactory::Remove(shared_memory_path);
        return -1;
    }

    return 0;
}

int UnregisterSharedMemory(unsigned long thread_id,
                           const TraceClientId& trace_client_zerocopy_id,
                           const ShmObjectHandle& shm_handle)
{

    auto unregister_result = GenericTraceAPI::UnregisterShmObject(trace_client_zerocopy_id, shm_handle);
    if (!unregister_result.has_value())
    {
        std::cout << "Thread #" << thread_id << "debug(\"LIB\"): Failed to unregister shm object" << std::endl;
        return -1;
    }
    return 0;
}

int PrepareSharedMemory(bool is_registered_memory_typed,
                        const std::string& shared_memory_path,
                        std::shared_ptr<score::memory::shared::ISharedMemoryResource>& memory_resource)
{
    score::memory::shared::SharedMemoryFactory::WorldWritable permissions{};
    auto mem_size = shared_memory_size;
    if (!is_registered_memory_typed)
    {
        mem_size = 512U * 1024U * 1024U;  // expecting that this size exceeds TypedMem resources
    }
    std::cout << "debug(\"LIB\"): Allocating " << mem_size << " bytes in Typed Memory" << std::endl;
    score::memory::shared::SharedMemoryFactory::SetTypedMemoryProvider(score::memory::shared::TypedMemory::Default());
    memory_resource = score::memory::shared::SharedMemoryFactory::Create(
        shared_memory_path, [](auto&&) {}, mem_size, {permissions}, true);

    if (nullptr == memory_resource)
    {
        std::cout << "debug(\"LIB\"): Failed to open memory resource" << std::endl;
        score::memory::shared::SharedMemoryFactory::Remove(shared_memory_path);
        return -1;
    }

    if (!is_registered_memory_typed)
    {
        void* base_addr = memory_resource->getBaseAddress();
        std::cout << "base_addr: " << std::hex << base_addr << std::endl;
        off64_t phys_addr;
        if (mem_offset64(base_addr, NOFD, 1, &phys_addr, NULL) != -1)
        {
            std::cout << "phys_addr: " << std::hex << phys_addr << std::endl;
            uint64_t typedmem_addr = phys_addr;
            struct asinfo_entry* asinfo = SYSPAGE_ENTRY(asinfo);
            int num_entries = _syspage_ptr->asinfo.entry_size / sizeof(asinfo_entry);
            char* string_table = SYSPAGE_ENTRY(strings)->data;

            for (int i = 0; i < num_entries; ++i)
            {
                if ((typedmem_addr >= asinfo[i].start) && (typedmem_addr < asinfo[i].end))
                {
                    if (strcmp((string_table + asinfo[i].name), "sysram") == 0)
                    {
                        std::cout << "Memory allocated here:" << std::endl;
                        std::cout << "Start: " << std::hex << asinfo[i].start << ", End: " << std::hex << asinfo[i].end
                                  << ", Name: " << (string_table + asinfo[i].name) << std::endl;
                        return 0;
                    }
                }
            }
            std::cout << "ERROR: Memory allocated not in SYSRAM" << std::endl;
            score::memory::shared::SharedMemoryFactory::Remove(shared_memory_path);
            return -2;
        }
    }
    return 0;
}

int StartTracing(unsigned long tid,
                 const options& passed_options,
                 const std::string& shared_memory_path,
                 std::shared_ptr<score::memory::shared::ISharedMemoryResource> memory_resource)
{
    TraceClientId trace_local_client_id = static_cast<TraceClientId>(-1);
    TraceClientId trace_client_zerocopy_id = static_cast<TraceClientId>(-1);
    ShmObjectHandle shm_handle = -1;

    std::cout << "Thread #" << tid << " PrepareSharedMemory finsihed" << std::endl;
    auto result = RegisterClients(tid,
                                  passed_options.is_reregister_test,
                                  trace_local_client_id,
                                  trace_client_zerocopy_id,
                                  shm_handle,
                                  shared_memory_path);
    if (result != 0)
    {
        std::ignore = UnregisterSharedMemory(tid, trace_client_zerocopy_id, shm_handle);
        return result;
    }
    std::cout << "Thread #" << tid << " RegisterClients finsihed" << std::endl;

    const uint8_t trace_repeat = passed_options.is_pregate_testing ? 5U : 1U;
    for (auto i = 0; i < trace_repeat; i++)
    {
        Trace(tid, trace_local_client_id, trace_client_zerocopy_id, shm_handle, shared_memory_path, memory_resource);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "Thread #" << tid << " Trace finsihed" << std::endl;

    result = UnregisterSharedMemory(tid, trace_client_zerocopy_id, shm_handle);
    if (result != 0)
    {
        return result;
    }
    std::cout << "Thread #" << tid << " UnregisterSharedMemory finsihed" << std::endl;
    return 0;
}

int main(int argc, char* argv[])
{
    score::mw::log::LogDebug() << "Tracing app is running";

    options passed_options;

    int res = parse_options(argc, argv, &passed_options);
    if (res != 0)
    {
        std::cout << "Error while parsing arguments" << std::endl;
        return res;
    }

    if (passed_options.is_concurrent_testing && number_of_threads == 0)
    {
        number_of_threads = 2;
    }
    else if (!passed_options.is_concurrent_testing)
    {
        number_of_threads = 1;
    }

    std::cout << "debug(\"TRC\"): "
              << "Tracing started" << std::endl;

    // prepare the shared memory
    auto process_id = getpid();
    const std::string shared_memory_path = "/dev_client_" + std::to_string(process_id);
    std::shared_ptr<score::memory::shared::ISharedMemoryResource> memory_resource{nullptr};
    res = PrepareSharedMemory(passed_options.is_registered_memory_typed, shared_memory_path, memory_resource);
    if (res != 0)
    {
        return res;
    }

    if (passed_options.is_integrity_testing)
    {
        auto result = trace_integrity_check(shared_memory_path, memory_resource);
        // remove the shared memory
        score::memory::shared::SharedMemoryFactory::Remove(shared_memory_path);
        return result;
    }

    // delay tracing startup for ITF testing
    std::this_thread::sleep_for(std::chrono::milliseconds{500});
    if (passed_options.is_pregate_testing)
    {
        chunk_size = 400;
        iteration_count = 60;

        StartTracing(1, passed_options, shared_memory_path, memory_resource);

        // remove the shared memory
        score::memory::shared::SharedMemoryFactory::Remove(shared_memory_path);
        return 0;
    }

    // Work with concurrent number_of_threads threads (which can be 1)
    std::vector<std::unique_ptr<score::cpp::jthread>> threads(number_of_threads);
    std::vector<std::promise<int>> promise(number_of_threads);
    std::vector<std::future<int>> future(number_of_threads);
    for (unsigned long i = 0; i < number_of_threads; i++)
    {
        future[i] = promise[i].get_future();
    }

    for (unsigned long i = 0; i < number_of_threads; i++)
    {
        threads[i] = std::make_unique<score::cpp::jthread>(
            [&promise, i, &passed_options, &shared_memory_path, &memory_resource]() -> void {
                promise[i].set_value(StartTracing(i + 1, passed_options, shared_memory_path, memory_resource));
            });
    }
    for (unsigned long i = 0; i < number_of_threads; i++)
    {
        threads[i]->join();
    }

    // remove the shared memory
    score::memory::shared::SharedMemoryFactory::Remove(shared_memory_path);
    for (unsigned long i = 0; i < number_of_threads; i++)
    {
        res = future[i].get();
        if (res != 0)
        {
            std::cout << "A thread is failing to trace" << std::endl;
            return res;
        }
    }
    return 0;
}
