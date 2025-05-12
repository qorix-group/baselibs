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
#include "score/os/qnx/pci_safety_impl.h"

namespace score
{
namespace os
{
namespace qnx
{
const char* const PciSafetyImpl::kPciErrorMsg{"Safe PCI library error!"};

pci_bdf_t PciSafetyImpl::pci_bdf(const std::uint8_t bus, const std::uint8_t dev, const std::uint8_t func) const noexcept
{
    /* KW_SUPPRESS_START:AUTOSAR.CAST.CSTYLE: Cstyle cast happening outside code domain */
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    /* KW_SUPPRESS_START:MISRA.CAST.UNSIGNED_BITS: Bitwise operation happening outside code domain */
    /* KW_SUPPRESS_START:MISRA.LITERAL.SUFFIX.CASE: Cannot make 0xFFu, 0x1Fu & 0x7u uppercase as  */
    /* these values are passed as agruments */
    return PCI_BDF(bus, dev, func);
    /* KW_SUPPRESS_END:MISRA.LITERAL.SUFFIX.CASE: */
    /* KW_SUPPRESS_END:MISRA.CAST.UNSIGNED_BITS: Bitwise operation happening outside code domain */
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    /* KW_SUPPRESS_END:AUTOSAR.CAST.CSTYLE: Cstyle cast happening outside code domain */
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<std::string> PciSafetyImpl::pci_device_cfg_rd32(const pci_bdf_t bdf,
                                                                    const std::uint16_t offset,
                                                                    std::uint32_t* const val) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const pci_err_t res = ::pci_device_cfg_rd32(bdf, offset, val);
    if (res != PCI_ERR_OK)
    {
        return score::cpp::make_unexpected<std::string>(kPciErrorMsg);
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<std::string> PciSafetyImpl::pci_device_read_did(const pci_bdf_t bdf,
                                                                    pci_did_t* const did) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const pci_err_t res = ::pci_device_read_did(bdf, did);
    if (res != PCI_ERR_OK)
    {
        return score::cpp::make_unexpected<std::string>(kPciErrorMsg);
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<std::string> PciSafetyImpl::pci_device_read_vid(const pci_bdf_t bdf,
                                                                    pci_vid_t* const vid) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const pci_err_t res = ::pci_device_read_vid(bdf, vid);
    if (res != PCI_ERR_OK)
    {
        return score::cpp::make_unexpected<std::string>(kPciErrorMsg);
    }
    return {};
}
}  // namespace qnx
}  // namespace os
}  // namespace score
