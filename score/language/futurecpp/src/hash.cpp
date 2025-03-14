///
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/hash.hpp>

namespace score::cpp
{
namespace detail
{

constexpr std::uint32_t fnv_params<4U>::prime;
constexpr std::uint32_t fnv_params<4U>::offset_basis;

constexpr std::uint64_t fnv_params<8U>::prime;
constexpr std::uint64_t fnv_params<8U>::offset_basis;

} // namespace detail
} // namespace score::cpp
