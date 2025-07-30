///
/// \file
/// \copyright Copyright (C) 2020-2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/private/memory/uninitialized_construct_using_allocator.hpp>
#include <score/private/memory/uninitialized_construct_using_allocator.hpp> // check include guard

#include <score/memory_resource.hpp>
#include <score/vector.hpp>

#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

namespace
{

using trailing_archetype = score::cpp::pmr::vector<int>;

// NOTRACING
TEST(UninitializedConstructUsingAllocatorTest, WithLValue)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<int>;
    allocator_type pma{score::cpp::pmr::new_delete_resource()};
    std::aligned_storage_t<sizeof(trailing_archetype), alignof(trailing_archetype)> storage;
    auto* const pointer = reinterpret_cast<trailing_archetype*>(&storage);
    const trailing_archetype original{1, 23, pma};
    trailing_archetype* const result{score::cpp::detail::uninitialized_construct_using_allocator(pointer, pma, original)};
    EXPECT_EQ(result->get_allocator(), pma);
    EXPECT_EQ(result->at(0), 23);
    result->~trailing_archetype();
}

// NOTRACING
TEST(UninitializedConstructUsingAllocatorTest, WithRValue)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<int>;
    allocator_type pma{score::cpp::pmr::new_delete_resource()};
    std::aligned_storage_t<sizeof(trailing_archetype), alignof(trailing_archetype)> storage;
    auto* const pointer = reinterpret_cast<trailing_archetype*>(&storage);
    trailing_archetype original{1, 23, pma};
    trailing_archetype* const result{
        score::cpp::detail::uninitialized_construct_using_allocator(pointer, pma, std::move(original))};
    EXPECT_EQ(result->get_allocator(), pma);
    EXPECT_EQ(result->at(0), 23);
    EXPECT_TRUE(original.empty());
    result->~trailing_archetype();
}

} // namespace
