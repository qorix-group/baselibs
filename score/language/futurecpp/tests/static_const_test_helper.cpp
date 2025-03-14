///
/// \file
/// \copyright Copyright (C) 2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include "static_const_test_helper.hpp"

namespace score::cpp
{

const static_const_test_type* static_const_test_type_instance_address() { return &static_const_test_type_instance; }

} // namespace score::cpp
