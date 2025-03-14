///
/// \file
/// \copyright Copyright (C) 2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#ifndef TESTS_STATIC_CONST_TEST_HELPER_HPP // NOLINT(llvm-header-guard)
#define TESTS_STATIC_CONST_TEST_HELPER_HPP

#include <score/private/utility/static_const.hpp>

namespace score::cpp
{

struct static_const_test_type
{
    explicit static_const_test_type() = default;
};

namespace
{
constexpr auto& static_const_test_type_instance = static_const<static_const_test_type>::value;
}

const static_const_test_type* static_const_test_type_instance_address();

} // namespace score::cpp

#endif // TESTS_STATIC_CONST_TEST_HELPER_HPP
