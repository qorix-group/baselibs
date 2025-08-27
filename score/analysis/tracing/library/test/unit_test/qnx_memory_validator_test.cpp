#include "score/analysis/tracing/library/generic_trace_api/memory_validator/qnx/qnx_memory_validator.h"
#include "score/analysis/tracing/library/generic_trace_api/error_code/error_code.h"
#include "score/os/mocklib/qnx/mock_mman.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string_view>

using ::testing::_;
using ::testing::Mock;
using ::testing::Return;
using ::testing::StrEq;
using ::testing::StrictMock;

using namespace score::analysis::tracing;

class QnxMemoryValidatorTestFixture : public ::testing::Test
{
  public:
    QnxMemoryValidatorTestFixture()
    {
        mman_mock_ = std::make_unique<StrictMock<score::os::qnx::MmanQnxMock>>();
        mman_mock_raw_ptr_ = mman_mock_.get();
        memory_validator_ = std::make_unique<MemoryValidator>(std::move(mman_mock_));
    }

    static constexpr std::int32_t invalid_file_descriptor_{-1};
    static constexpr std::int32_t valid_file_descriptor_{0};
    static constexpr std::string_view valid_path_{"/path"};

    std::unique_ptr<StrictMock<score::os::qnx::MmanQnxMock>> mman_mock_;
    StrictMock<score::os::qnx::MmanQnxMock>* mman_mock_raw_ptr_;
    std::unique_ptr<IMemoryValidator> memory_validator_;
};

TEST_F(QnxMemoryValidatorTestFixture, IsSharedMemoryTypedBadFileDescriptor)
{
    const auto is_shared_memory_typed_result = memory_validator_->IsSharedMemoryTyped(invalid_file_descriptor_);
    EXPECT_FALSE(is_shared_memory_typed_result.has_value());
    EXPECT_EQ(is_shared_memory_typed_result.error(), ErrorCode::kBadFileDescriptorFatal);
}

TEST_F(QnxMemoryValidatorTestFixture, IsSharedMemoryTypedFileDescriptorShmCtlFailed)
{
    EXPECT_CALL(*mman_mock_raw_ptr_, shm_ctl)
        .WillOnce(Return(score::cpp::unexpected<score::os::Error>(score::os::Error::createUnspecifiedError())));

    const auto is_shared_memory_typed_result = memory_validator_->IsSharedMemoryTyped(valid_file_descriptor_);
    EXPECT_FALSE(is_shared_memory_typed_result.has_value());
    EXPECT_EQ(is_shared_memory_typed_result.error(), ErrorCode::kSharedMemoryObjectFlagsRetrievalFailedFatal);
}

TEST_F(QnxMemoryValidatorTestFixture, IsSharedMemoryTypedFileDescriptorNotInTypedMemory)
{
    EXPECT_CALL(*mman_mock_raw_ptr_, shm_ctl).WillOnce(Return(0));

    const auto is_shared_memory_typed_result = memory_validator_->IsSharedMemoryTyped(valid_file_descriptor_);
    EXPECT_TRUE(is_shared_memory_typed_result.has_value());
    EXPECT_FALSE(is_shared_memory_typed_result.value());
}

TEST_F(QnxMemoryValidatorTestFixture, IsSharedMemoryTypedFileDescriptorSuccess)
{
    EXPECT_CALL(*mman_mock_raw_ptr_, shm_ctl).WillOnce(Return(SHMCTL_PHYS));

    const auto is_shared_memory_typed_result = memory_validator_->IsSharedMemoryTyped(valid_file_descriptor_);
    EXPECT_TRUE(is_shared_memory_typed_result.has_value());
}

TEST_F(QnxMemoryValidatorTestFixture, IsSharedMemoryTypedEmptyPath)
{
    const auto is_shared_memory_typed_result = memory_validator_->IsSharedMemoryTyped(std::string{});
    EXPECT_FALSE(is_shared_memory_typed_result.has_value());
    EXPECT_EQ(is_shared_memory_typed_result.error(), ErrorCode::kInvalidArgumentFatal);
}

TEST_F(QnxMemoryValidatorTestFixture, IsSharedMemoryTypedShmOpenFailed)
{
    EXPECT_CALL(*mman_mock_raw_ptr_, shm_open(StrEq(valid_path_.data()), _, _))
        .WillOnce(Return(score::cpp::unexpected<score::os::Error>(score::os::Error::createUnspecifiedError())));

    const auto is_shared_memory_typed_result = memory_validator_->IsSharedMemoryTyped(valid_path_.data());
    EXPECT_FALSE(is_shared_memory_typed_result.has_value());
    EXPECT_EQ(is_shared_memory_typed_result.error(), ErrorCode::kBadFileDescriptorFatal);
}

TEST_F(QnxMemoryValidatorTestFixture, IsSharedMemoryTypedPathSuccess)
{
    EXPECT_CALL(*mman_mock_raw_ptr_, shm_open(StrEq(valid_path_.data()), _, _))
        .WillOnce(Return(valid_file_descriptor_));
    EXPECT_CALL(*mman_mock_raw_ptr_, shm_ctl).WillOnce(Return(SHMCTL_PHYS));

    const auto is_shared_memory_typed_result = memory_validator_->IsSharedMemoryTyped(valid_path_.data());
    EXPECT_TRUE(is_shared_memory_typed_result.has_value());
}
