/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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

#include "score/os/dlfcn_impl.h"

#include <dlfcn.h>
#include <iostream>
#include <type_traits>

namespace score::os
{
namespace
{

[[nodiscard]] std::int32_t DlOpenFlagsToNative(const Dlfcn::DlOpenFlag flags) noexcept
{
    std::uint32_t native_flags{0U};

    if (static_cast<std::underlying_type_t<Dlfcn::DlOpenFlag>>(flags & Dlfcn::DlOpenFlag::kLazy) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(RTLD_LAZY);
    }
    if (static_cast<std::underlying_type_t<Dlfcn::DlOpenFlag>>(flags & Dlfcn::DlOpenFlag::kNow) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(RTLD_NOW);
    }
    if (static_cast<std::underlying_type_t<Dlfcn::DlOpenFlag>>(flags & Dlfcn::DlOpenFlag::kLocal) != 0U)
    {
        // The |= is intentional for portability.
        native_flags |= static_cast<std::uint32_t>(RTLD_LOCAL);
    }
    if (static_cast<std::underlying_type_t<Dlfcn::DlOpenFlag>>(flags & Dlfcn::DlOpenFlag::kGlobal) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(RTLD_GLOBAL);
    }

    return static_cast<std::int32_t>(native_flags);
}

}  // namespace

score::cpp::expected<void*, Error> DlfcnImpl::dlopen(const char* const file_name, const DlOpenFlag flags) const noexcept
{
    void* const handle{::dlopen(file_name, DlOpenFlagsToNative(flags))};
    if (handle == nullptr)
    {
        const char* const dl_error{::dlerror()};
        if (dl_error != nullptr)
        {
            // Intentional - last resort for diagnosing dlopen failures, since dlerror() is the only way to get error
            // details.
            std::cerr << "dlopen failed: " << dl_error << '\n';
        }
        return score::cpp::make_unexpected(Error::createUnspecifiedError());
    }
    return handle;
}

score::cpp::expected<void*, Error> DlfcnImpl::dlsym(void* const handle, const char* const symbol_name) const noexcept
{
    // POSIX mandates: clear dlerror(), call dlsym(), then check dlerror() to detect failure
    // (dlsym may legitimately return nullptr for a valid symbol).
    static_cast<void>(::dlerror());
    void* const symbol{::dlsym(handle, symbol_name)};
    const char* const dl_error{::dlerror()};
    if (dl_error != nullptr)
    {
        // Intentional - last resort for diagnosing dlsym failures, since dlerror() is the only way to get error
        // details.
        std::cerr << "dlsym failed: " << dl_error << '\n';
        return score::cpp::make_unexpected(Error::createUnspecifiedError());
    }
    return symbol;
}

}  // namespace score::os
