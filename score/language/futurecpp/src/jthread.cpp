///
/// \file
/// \copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/jthread.hpp>

#include <unistd.h>

namespace score::cpp
{

unsigned int jthread::hardware_concurrency() noexcept
{
    const long result{::sysconf(_SC_NPROCESSORS_ONLN)};
    if (result < 0)
    {
        return 0U;
    }
    return static_cast<unsigned int>(result);
}

} // namespace score::cpp
