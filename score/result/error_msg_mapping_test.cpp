#include "score/result/error_msg_mapping.h"
#include "score/result/dummy_error_code.h"

#include <gtest/gtest.h>

namespace score::result
{

TEST(RustBridgeTests, GetMessageForErrorCodeFFI)
{
    std::string_view error_message;
    LibResultErrorDomainGetMessageForErrorCode(
        dummy_error_domain, static_cast<ErrorCode>(DummyErrorCode::kFirstError), error_message);
    ASSERT_EQ("First Error!", error_message);
}

}  // namespace score::result
