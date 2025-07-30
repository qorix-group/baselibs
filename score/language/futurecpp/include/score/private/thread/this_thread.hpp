///
/// \file
/// \copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.ThisThread component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THIS_THREAD_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THIS_THREAD_HPP

#include <score/private/thread/thread.hpp>

// Header part of safety qualification
// broken_link_s/:b:/r/teams/SafeSoftwarePlatform/Freigegebene%20Dokumente/Ironman/safety/mpad/tuv/EZ_653_22_23_RL_2023_08_10_QOS_2_2_4.pdf?csf=1&web=1&e=FTMJRu
//{
#include <pthread.h>
#include <sched.h>
//}

namespace score::cpp
{
namespace this_thread
{

/// \brief Returns the id of the current thread.
///
/// \return id of the current thread.
inline score::cpp::thread::id get_id() noexcept { return ::pthread_self(); }

/// \brief Provides a hint to the implementation to reschedule the execution of threads, allowing other threads to run
inline void yield() noexcept { ::sched_yield(); }

} // namespace this_thread
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THIS_THREAD_HPP
