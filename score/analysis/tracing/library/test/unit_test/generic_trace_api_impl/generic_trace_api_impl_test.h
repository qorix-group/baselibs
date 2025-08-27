#include "score/analysis/tracing/library/generic_trace_api/generic_trace_api_impl.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_allocator/trace_job_allocator.h"
#include "score/analysis/tracing/library/generic_trace_api/trace_job_processor/trace_job_processor.h"
#include "score/analysis/tracing/library/interface/generic_trace_api.h"
#include "score/language/safecpp/scoped_function/scope.h"

#include "score/analysis/tracing/common/testing_utils/notification/notification_helper.h"
#include "score/analysis/tracing/library/test/unit_test/mocks/mock_daemon_communicator.h"
#include "score/analysis/tracing/library/test/unit_test/mocks/mock_memory_validator.h"
#include "score/analysis/tracing/library/test/unit_test/mocks/mock_object_factory.h"
#include "score/analysis/tracing/library/test/unit_test/mocks/mock_trace_job_allocator.h"
#include "score/analysis/tracing/library/test/unit_test/mocks/mock_trace_job_processor.h"
#include "score/memory/shared/shared_memory_factory.h"
#include "score/memory/shared/shared_memory_factory_mock.h"
#include "score/memory/shared/shared_memory_resource_mock.h"
#include "score/os/mocklib/unistdmock.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using testing::_;
using testing::An;
using testing::AtLeast;
using testing::ByMove;
using testing::Eq;
using testing::InSequence;
using testing::Invoke;
using testing::Matcher;
using testing::NiceMock;
using testing::Return;
using testing::StrEq;
using testing::StrictMock;
using testing::WithArgs;

namespace score
{
namespace analysis
{
namespace tracing
{

class GenericTraceAPIImplFixtureBase : public testing::Test
{
  protected:
    TraceDoneCallBackType trace_done_callback{score::safecpp::Scope<>{}, [](TraceContextId) noexcept {}};

    void SetUp() override;

    const DltMetaInfo meta_info_{DltProperties{}};
    const std::string app_instance_id_{"client"};
    const std::string path_{"path"};
    const std::string tmd_filename_{"/dev_tmd_0"};
    static constexpr auto pid_{0};
    static constexpr ShmObjectHandle tmd_shm_obj_handle_{0};
    static constexpr ShmObjectHandle shm_obj_handle_{1};
    static constexpr std::int32_t file_descriptor_{0};
    static constexpr std::int32_t invalid_file_descriptor_{-1};
    static constexpr TraceContextId context_id_{0};
    static constexpr TraceClientId client_id_{1};

    const SharedMemoryLocation shm_offset_ptr_{shm_obj_handle_, 0};
    const SharedMemoryChunk shm_chunk_{shm_offset_ptr_, 0};
    ShmDataChunkList shm_data_chunk_list_{shm_chunk_};

    const LocalDataChunk local_data_chunk_{nullptr, 0};
    LocalDataChunkList local_data_chunk_list_{local_data_chunk_};

    std::unique_ptr<StrictMock<MockDaemonCommunicator>> mock_daemon_communicator_ptr_;
    std::unique_ptr<MockObjectFactory> mock_object_factory_;
    score::cpp::stop_token stop_token{};
};

class GenericTraceAPIImplFixture : public GenericTraceAPIImplFixtureBase
{
  protected:
    static TraceResult deallocator(SharedMemoryLocation, TraceJobType);

    void SetUp() override;
    void TearDown() override;
    void ExpectCorrectInitialization(
        PromiseNotifier& daemon_notifier,
        PromiseNotifier& library_notifier,
        std::optional<std::reference_wrapper<std::atomic<bool>>> delay_ltpm_daemon_param = std::nullopt);

    void RequestLibraryWorkerThreadShutdown(PromiseNotifier& notifier);
    void ExpectCorrectCleaningUp(PromiseNotifier& terminate_notifier);

    std::shared_ptr<StrictMock<memory::shared::SharedMemoryResourceMock>> mock_shared_memory_resource_;
    std::shared_ptr<StrictMock<memory::shared::SharedMemoryFactoryMock>> mock_shared_memory_factory_;
    std::unique_ptr<StrictMock<MockTraceJobAllocator>> mock_trace_job_allocator_;
    std::unique_ptr<StrictMock<MockTraceJobProcessor>> mock_trace_job_processor_;
    std::unique_ptr<StrictMock<os::UnistdMock>> mock_unistd_;
    std::unique_ptr<StrictMock<MockMemoryValidator>> mock_memory_validator_;

    StrictMock<MockTraceJobAllocator>* mock_trace_job_allocator_ptr_raw_{nullptr};
    StrictMock<MockTraceJobProcessor>* mock_trace_job_processor_ptr_raw_{nullptr};
    StrictMock<MockDaemonCommunicator>* mock_daemon_communicator_ptr_raw_{nullptr};

    std::atomic<bool> valid_pointer_{true};
    std::mutex mutex_;
    TraceJobDeallocator callback_;
    IDaemonCommunicator::DaemonTerminationCallback daemon_crash_callback_;
    score::cpp::stop_source stop_source_{};
    static constexpr auto kLibraryCheckPeriodicity{std::chrono::milliseconds{10}};
    static constexpr auto kDaemonReadyCheckPeriodicity{std::chrono::milliseconds{10}};
    std::condition_variable condition_variable_;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score
