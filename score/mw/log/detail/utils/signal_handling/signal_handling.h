#ifndef SCORE_MW_LOG_DETAIL_UTILS_SIGNAL_HANDLING_SIGNAL_HANDLING_H_
#define SCORE_MW_LOG_DETAIL_UTILS_SIGNAL_HANDLING_SIGNAL_HANDLING_H_

#include "score/os/utils/signal.h"

namespace score::mw::log::detail
{

class SignalHandling
{
  public:
    static score::cpp::expected<std::int32_t, score::os::Error> PThreadBlockSigTerm(score::os::Signal& signal) noexcept;
    static score::cpp::expected<std::int32_t, score::os::Error> PThreadUnblockSigTerm(score::os::Signal& signal) noexcept;

    template <typename Func>
    static auto WithSigTermBlocked(score::os::Signal& signal, Func&& func) noexcept -> decltype(func())
    {
        const auto block_result = PThreadBlockSigTerm(signal);
        auto result = func();
        const auto unblock_result = PThreadUnblockSigTerm(signal);
        std::ignore = unblock_result;
        return result;
    }
};

}  //  namespace score::mw::log::detail

#endif  //  SCORE_MW_LOG_DETAIL_UTILS_SIGNAL_HANDLING_SIGNAL_HANDLING_H_
