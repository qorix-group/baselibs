#ifndef SCORE_MW_LOG_DETAIL_SLOG_SLOG_BACKEND_H
#define SCORE_MW_LOG_DETAIL_SLOG_SLOG_BACKEND_H

#include "score/os/qnx/slog2_impl.h"
#include "score/mw/log/detail/backend.h"
#include "score/mw/log/detail/circular_allocator.h"

#include <cstdint>
#include <string_view>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

class SlogBackend final : public Backend
{
  public:
    explicit SlogBackend(const std::size_t number_of_slots,
                         const LogRecord& initial_slot_value,
                         const std::string_view app_id,
                         score::cpp::pmr::unique_ptr<score::os::qnx::Slog2> slog2_instance) noexcept;

    score::cpp::optional<SlotHandle> ReserveSlot() noexcept override;
    void FlushSlot(const SlotHandle& slot) noexcept override;
    LogRecord& GetLogRecord(const SlotHandle& slot) noexcept override;

  private:
    void Init(const std::uint8_t verbosity) noexcept;

    std::string app_id_;
    CircularAllocator<score::mw::log::detail::LogRecord> buffer_;
    slog2_buffer_t slog_buffer_;
    slog2_buffer_set_config_t slog_buffer_config_;
    score::cpp::pmr::unique_ptr<score::os::qnx::Slog2> slog2_instance_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_SLOG_SLOG_BACKEND_H
