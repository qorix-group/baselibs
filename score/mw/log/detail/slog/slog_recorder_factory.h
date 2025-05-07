#ifndef SCORE_MW_LOG_DETAIL_SLOG_RECORDER_FACTORY_H
#define SCORE_MW_LOG_DETAIL_SLOG_RECORDER_FACTORY_H

#include "score/mw/log/detail/file_logging/text_recorder.h"
#include "score/mw/log/detail/log_recorder_factory.hpp"
#include "score/mw/log/detail/slog/slog_backend.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
class SlogRecorderFactory : public LogRecorderFactory<SlogRecorderFactory>
{
  public:
    std::unique_ptr<Recorder> CreateConcreteLogRecorder(const Configuration& config,
                                                        score::cpp::pmr::memory_resource* memory_resource);

  private:
    std::unique_ptr<Backend> CreateSystemBackend(const Configuration& config,
                                                 score::cpp::pmr::memory_resource* memory_resource);
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_SLOG_RECORDER_FACTORY_H
