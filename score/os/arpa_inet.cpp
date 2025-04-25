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
#include "score/os/arpa_inet.h"
#include <sys/socket.h>

namespace
{

class ArpaInetInstance final : public score::os::ArpaInet
{
  public:
    /* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
    score::os::InAddrT InetAddr(const std::string& ip_addr) const override
    {
        return ::inet_addr(ip_addr.c_str());
    }
    /* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */

    // @todo: use score::Result instead of score::cpp::expected after merging this ticket(Ticket-61178).
    /* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
    score::cpp::expected<score::os::InAddr, score::os::Error::Code> InetPton(const std::string& src) const override
    /* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
    {
        score::os::InAddr imr_multiaddr{};
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:OS library macros */
        std::int32_t conversion_state = ::inet_pton(AF_INET, src.c_str(), &imr_multiaddr);
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:OS library macros */
        if (conversion_state == 0)
        {
            // We are using "kInvalidArgument" error code because inet_pton return 0 when 'src' does not contain a
            // character string representing a valid network address in the specified address family, so, it is similar
            // to invalid argument.
            // @todo: use score::MakeUnexpected instead of score::cpp::make_unexpected after merging this ticket(Ticket-61178).
            return score::cpp::make_unexpected(score::os::Error::Code::kInvalidArgument);
        }
        else if (conversion_state == 1)
        {
            // Network address was successfully converted.
            return imr_multiaddr;
        }
        else
        {
            // @todo: use score::MakeUnexpected instead of score::cpp::make_unexpected after merging this ticket(Ticket-61178).
            // Not supported error code returned from 'inet_pton' system call.
            // NOTE_1: ::inet_pton() returns -1 when the domain is not supported, however, in our case this will never
            // happen as we pass in the fixed `AF_INET` parameter. So exclude this line from coverage tool.
            return score::cpp::make_unexpected(score::os::Error::Code::kUnexpected);  // LCOV_EXCL_LINE - see NOTE_1 above
        }
    }
};

}  // namespace

score::os::ArpaInet& score::os::ArpaInet::instance() noexcept
{
    static ArpaInetInstance instance;
    return select_instance(instance);
}
