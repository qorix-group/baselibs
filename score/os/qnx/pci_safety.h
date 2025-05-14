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
#ifndef SCORE_LIB_OS_QNX_PCI_SAFETY_H
#define SCORE_LIB_OS_QNX_PCI_SAFETY_H

#include "score/os/ObjectSeam.h"

#include "score/os/errno.h"

#include "score/expected.hpp"

extern "C" {
/*KW_SUPPRESS_START: MISRA.INCL.INSIDE:extern "C" is needed to link C related code*/
#include <pci/pci_safety.h>
/*KW_SUPPRESS_END: MISRA.INCL.INSIDE:extern "C" is needed to link C related code*/
}

namespace score
{
namespace os
{
namespace qnx
{

class PciSafety : public score::os::ObjectSeam<PciSafety>
{
  public:
    static PciSafety& instance() noexcept;

    virtual pci_bdf_t pci_bdf(std::uint8_t bus, std::uint8_t dev, std::uint8_t func) const noexcept = 0;
    // TODO: Ticket-25608 Replace score::cpp::expected with score::Result
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<std::string> pci_device_cfg_rd32(pci_bdf_t bdf,
                                                                 std::uint16_t offset,
                                                                 std::uint32_t* val) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<std::string> pci_device_read_vid(pci_bdf_t bdf, pci_vid_t* vid) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<std::string> pci_device_read_did(pci_bdf_t bdf, pci_did_t* did) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    virtual ~PciSafety() = default;
};

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_PCI_SAFETY_H
