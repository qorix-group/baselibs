///
/// \file
/// \copyright Copyright (C) 2019-2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.Pmr.MemoryResource component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_FREE_LIST_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_FREE_LIST_HPP

#include <score/assert.hpp>

#include <memory>
#include <utility>

namespace score::cpp
{
namespace pmr
{
namespace detail
{

class free_list
{
public:
    bool empty() const { return next_ == nullptr; }

    void* pop_front()
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());
        return std::exchange(next_, next_->next_);
    }

    void push_front(void* const p)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(p != nullptr);
        free_list* const new_head{::new (p) free_list{}};
        new_head->next_ = next_;
        next_ = new_head;
    }

    void clear() { next_ = nullptr; }

private:
    free_list* next_{};
};

} // namespace detail
} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_FREE_LIST_HPP
