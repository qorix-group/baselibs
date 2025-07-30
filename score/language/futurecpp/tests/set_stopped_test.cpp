///
/// @file
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/execution.hpp>
#include <score/execution.hpp> //  // test include guard

#include <score/utility.hpp>

#include <gtest/gtest.h>

namespace score::cpp
{
namespace execution
{
namespace
{

struct receiver
{
    using receiver_concept = receiver_t;

    enum class action
    {
        init,
        stopped,
    };

    void set_stopped() & = delete;
    void set_stopped() && { *a = action::stopped; }
    void set_stopped() const& = delete;
    void set_stopped() const&& = delete;

    action* a{};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(set_stopped_test, set_stopped_GivenRValueReceiver_ExpectSetStoppedCalled)
{
    receiver::action a{receiver::action::init};
    set_stopped(receiver{&a});

    EXPECT_EQ(a, receiver::action::stopped);
}

} // namespace
} // namespace execution
} // namespace score::cpp
