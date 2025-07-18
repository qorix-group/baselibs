
/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
#include "my_custom_type_mw_log.h"

namespace my
{
namespace custom
{
namespace type
{

/* KW_SUPPRESS_START:AUTOSAR.OP.BINARY.RETVAL: False positive: this is correct signature of operator<<. */
score::mw::log::LogStream& operator<<(score::mw::log::LogStream& log_stream,
                                    const my::custom::type::MyCustomType& my_custom_type) noexcept
{
    log_stream << "my_custom_type: int_field : " << my_custom_type.int_field
               << " , string_field : " << my_custom_type.string_field;
    return log_stream;
}
/* KW_SUPPRESS_END:AUTOSAR.OP.BINARY.RETVAL: False positive: this is correct signature of operator<<. */

}  // namespace type
}  // namespace custom
}  // namespace my
