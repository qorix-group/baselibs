#ifndef SCORE_ANALYSIS_TRACING_COMMON_TESTING_UTILS_NOTIFICATION_HELPER_H
#define SCORE_ANALYSIS_TRACING_COMMON_TESTING_UTILS_NOTIFICATION_HELPER_H

#include <chrono>
#include <condition_variable>
#include <future>
#include <memory>
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

class PromiseNotifier
{
  public:
    PromiseNotifier() : promise_(std::make_shared<std::promise<void>>()) {}

    void Notify()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (promise_)
        {
            promise_->set_value();  // unblocks future.get()
        }
    }

    void WaitForNotification()
    {
        auto future = GetFuture();
        future.get();  // Blocks thread until promise is fulfilled. i.e, until Notify() is called
    }

    template <class Rep, class Period>
    bool WaitForNotificationWithTimeout(const std::chrono::duration<Rep, Period>& timeout_duration)
    {
        auto future = GetFuture();
        if (future.wait_for(timeout_duration) == std::future_status::ready)
        {
            future.get();
            return true;
        }
        return false;  // Timeout occurred
    }

    void Reset()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        promise_.reset();
        promise_ = std::make_shared<std::promise<void>>();
    }

  private:
    std::shared_future<void> GetFuture()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return promise_->get_future().share();
    }

    std::mutex mutex_;
    std::shared_ptr<std::promise<void>> promise_;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif /* SCORE_ANALYSIS_TRACING_COMMON_TESTING_UTILS_NOTIFICATION_HELPER_H */

#include <gmock/gmock.h>

// Simple ACTION_P for gMock to call Notify on a Notification object
ACTION_P(CallNotify, notification)
{
    notification->Notify();
}

// Usage example in a test flow:
//
// #include "score/analysis/tracing/daemon/code/plugin/perf/notification.h"
// #include <gmock/gmock.h>
// #include <gtest/gtest.h>
// #include <thread>
//
// using score::analysis::tracing::Notification;
//
// class MyMock {
//  public:
//   MOCK_METHOD(void, DoAsync, ());
// };
//
// TEST(MyTest, NotifiesOnAsyncCall) {
//   PromiseNotifier notification;
//   MyMock mock;
//   EXPECT_CALL(mock, DoAsync()).WillOnce(Notify(&notification));
//
//   std::thread t([&](){
//     mock.DoAsync();
//   });
//
//   notification.WaitForNotification();
//   t.join();
// }
//
// This test will pass if DoAsync() is called and notification.Notify() is triggered.
