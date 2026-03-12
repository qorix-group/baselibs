#include "score/result/error_msg_mapping.h"

extern "C" void LibResultErrorDomainGetMessageForErrorCode(const score::result::ErrorDomain& domain,
                                                           score::result::ErrorCode code,
                                                           std::string_view& result) noexcept
{
    result = domain.MessageFor(code);
}
