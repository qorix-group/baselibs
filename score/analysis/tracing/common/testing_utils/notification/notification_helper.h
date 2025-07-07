#ifndef SCORE_ANALYSIS_TRACING_COMMON_TESTING_UTILS_NOTIFICATION_HELPER_H
#define SCORE_ANALYSIS_TRACING_COMMON_TESTING_UTILS_NOTIFICATION_HELPER_H

#include <chrono>
#include <condition_variable>
#include <mutex>

namespace score
{
namespace analysis
{
namespace tracing
{

class NotificationHelper
{
  public:
    NotificationHelper() : notified_(false) {}

    void Notify()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        notified_ = true;
        cv_.notify_all();
    }

    void WaitForNotification()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] {
            return notified_;
        });
        notified_ = false;
    }

    // Returns true if notified, false if timeout occurred
    template <class Rep, class Period>
    bool WaitForNotificationWithTimeout(const std::chrono::duration<Rep, Period>& timeout_duration)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        bool was_notified = cv_.wait_for(lock, timeout_duration, [this] {
            return notified_;
        });
        if (was_notified)
        {
            notified_ = false;
        }
        return was_notified;
    }

  private:
    std::mutex mutex_;
    std::condition_variable cv_;
    bool notified_;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif /* SCORE_ANALYSIS_TRACING_COMMON_TESTING_UTILS_NOTIFICATION_HELPER_H */
