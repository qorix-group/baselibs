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
#include "score/os/utils/semaphore.h"

#include "score/os/semaphore.h"

#include "score/utility.hpp"

#include <cstring>
#include <stdexcept>

namespace score
{
namespace os
{
namespace utils
{

class Semaphore ::SemaphorePrivate
{
  private:
    /* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
    sem_t* m_sem;
    std::string m_name;
    bool m_open;
    /* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */

  public:
    /* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
    explicit SemaphorePrivate(const std::string& name,
                              const std::uint32_t count = 0,
                              const bool create_if_not_exists = true)
        : m_sem{nullptr}, m_name(name), m_open(true)
    {
        score::os::Semaphore::OpenFlag open_flags{};
        if (create_if_not_exists)
        {
            open_flags = open_flags | score::os::Semaphore::OpenFlag::kCreate;
        }
        const score::os::Semaphore::ModeFlag mode =
            score::os::Semaphore::ModeFlag::kReadUser |
            score::os::Semaphore::ModeFlag::kWriteUser; /* KW_SUPPRESS:MISRA.BITS.NOT_UNSIGNED:Macro has positive value */
        const auto ret = score::os::Semaphore::instance().sem_open(name.c_str(), open_flags, mode, count);
        m_sem = ret.has_value() ? ret.value() : SEM_FAILED;
        /* KW_SUPPRESS_START:AUTOSAR.CAST.CSTYLE:Cast is happening outside our code domain */
        if (SEM_FAILED == m_sem)
        {
            // NOLINTNEXTLINE(score-no-exceptions) The throw is needed for this implementation.
            throw std::runtime_error("Cannot open semaphore");
        } /* KW_SUPPRESS_END:AUTOSAR.CAST.CSTYLE:Cast is happening outside our code domain */
    } /* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */

    ~SemaphorePrivate()
    { /* KW_SUPPRESS:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
        if (m_open)
        {
            score::cpp::ignore = close();
        }
    }

    /* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
    SemaphorePrivate(SemaphorePrivate&&) noexcept = delete;
    SemaphorePrivate& operator=(SemaphorePrivate&&) noexcept = delete;
    SemaphorePrivate(const SemaphorePrivate&) = delete;
    SemaphorePrivate& operator=(const SemaphorePrivate&) = delete;
    /* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */

    score::cpp::expected_blank<score::os::Error> post() const noexcept
    { /* KW_SUPPRESS:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
        return score::os::Semaphore::instance().sem_post(m_sem);
    }

    score::cpp::expected_blank<score::os::Error> wait() const noexcept
    { /* KW_SUPPRESS:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
        return score::os::Semaphore::instance().sem_wait(m_sem);
    }

    score::cpp::expected_blank<score::os::Error> unlink() const noexcept
    { /* KW_SUPPRESS:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation*/
        return score::os::Semaphore::instance().sem_unlink(m_name.c_str());
    }

    score::cpp::expected_blank<score::os::Error> getvalue(std::int32_t& sval) const noexcept
    { /* KW_SUPPRESS:MISRA.LINKAGE.EXTERN:PIMPL model */
        return score::os::Semaphore::instance().sem_getvalue(m_sem, &sval);
    }

    score::cpp::expected_blank<score::os::Error> close() noexcept
    { /* KW_SUPPRESS:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
        m_open = false;
        return score::os::Semaphore::instance().sem_close(m_sem);
    }
};

Semaphore::Semaphore(const std::string& name, const std::uint32_t count, const bool create_if_not_exists)
    : d_ptr(score::utils::make_pimpl_64<SemaphorePrivate>(name, count, create_if_not_exists))
{
}

Semaphore::~Semaphore() = default; /* KW_SUPPRESS:MISRA.OBJ.TYPE.IDENT:Default destructor */

score::cpp::expected_blank<score::os::Error> Semaphore::post() const noexcept
{
    return d_ptr->post();
}

score::cpp::expected_blank<score::os::Error> Semaphore::wait() const noexcept
{
    return d_ptr->wait();
}

score::cpp::expected_blank<score::os::Error> Semaphore::unlink() const noexcept
{
    return d_ptr->unlink();
}

score::cpp::expected_blank<score::os::Error> Semaphore::getvalue(std::int32_t& sval) const noexcept
{
    return d_ptr->getvalue(sval);
}

score::cpp::expected_blank<score::os::Error> Semaphore::close() noexcept
{
    return d_ptr->close();
}

}  // namespace utils
}  // namespace os
}  // namespace score
