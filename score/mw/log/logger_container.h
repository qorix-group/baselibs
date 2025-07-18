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
#ifndef SCORE_MW_LOG_LOGGER_CONTAINER_H
#define SCORE_MW_LOG_LOGGER_CONTAINER_H

#include "score/mw/log/detail/wait_free_stack/wait_free_stack.h"
#include "score/mw/log/logger.h"
#include "score/mw/log/slot_handle.h"
#include <score/static_vector.hpp>

namespace score
{
namespace mw
{
namespace log
{
/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN:False positive. */
/* KW_SUPPRESS_START:MISRA.OBJ.TYPE.IDENT:False positive  */
/* KW_SUPPRESS_START:MISRA.OBJ.TYPE.COMPAT:False positive */
/* KW_SUPPRESS_START:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE:False positive*/
/* KW_SUPPRESS_START:MISRA.INIT.BRACES:False positive: Braces are correctly placed. */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:False positive*/
class LoggerContainer final /* KW_SUPPRESS:MISRA.ONEDEFRULE.VAR: False positive not relevant. */
/* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN */
/* KW_SUPPRESS_END:MISRA.OBJ.TYPE.IDENT */
/* KW_SUPPRESS_END:MISRA.OBJ.TYPE.COMPAT*/
/* KW_SUPPRESS_END:MISRA.INIT.BRACES*/
{
  public:
    explicit LoggerContainer();

    Logger& GetLogger(
        const std::string_view context) noexcept; /* KW_SUPPRESS:MISRA.ONEDEFRULE.VAR: False positive not relevant. */

    size_t GetCapacity() const noexcept;

    Logger& GetDefaultLogger() noexcept;

    /* KW_SUPPRESS_START: MISRA.USE.EXPANSION: False positive: it is not macro. */
  private:
    /* KW_SUPPRESS_END: MISRA.USE.EXPANSION */
    Logger& InsertNewLogger(const std::string_view context) noexcept;

    score::cpp::optional<std::reference_wrapper<Logger>> FindExistingLogger(const std::string_view context) noexcept;
    detail::WaitFreeStack<Logger, memory::shared::AtomicIndirectorReal> stack_;
    Logger default_logger_;
};
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN*/

/* KW_SUPPRESS_END:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE*/

}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_LOGGER_CONTAINER_H
