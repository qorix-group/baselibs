///
/// @file
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/execution.hpp>
#include <score/execution.hpp> //  // test include guard

#include <cstdint>

#include <gtest/gtest.h>

namespace score::cpp
{
namespace execution
{
namespace
{

struct op_state
{
    using operation_state_concept = operation_state_t;

    enum class action
    {
        init,
        non_const_call,
        const_call,
    };

    void start() & { *a = action::non_const_call; }
    void start() && = delete;
    void start() const& { *a = action::const_call; }
    void start() const&& = delete;

    action* a;
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(start_test, start_GivenNonConstOperation_ExpectStartIsCalledOnOperation)
{
    op_state::action a{op_state::action::init};
    op_state op{&a};
    start(op);
    EXPECT_EQ(op_state::action::non_const_call, a);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(start_test, start_GivenConstOperation_ExpectStartIsCalledOnOperation)
{
    op_state::action a{op_state::action ::init};
    const op_state op{&a};
    start(op);
    EXPECT_EQ(op_state::action::const_call, a);
}

} // namespace
} // namespace execution
} // namespace score::cpp
