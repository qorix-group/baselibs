// safe_result_cxx.h
//
// CXX bridge definitions for SafeResult FFI compatibility
// Ensures SafeResult types work correctly with the cxx crate

#ifndef SCORE_LIB_RESULT_SAFE_RESULT_CXX_H_
#define SCORE_LIB_RESULT_SAFE_RESULT_CXX_H_

#include <cstdint>
#include "score/result/safe_result.h"

namespace score::result {

// ============================================================================
// CXX Traits for SafeResult
// ============================================================================

/// Trait to declare that SafeResult is relocatable for cxx compatibility
/// This allows the cxx crate to move SafeResult values in memory safely
template <typename T>
struct IsRelocatable<SafeResult<T>> {
  enum { value = IsRelocatable<T>::value };
};

/// Forward declaration of cxx::IsRelocatable for integration
namespace cxx {
  template <typename T>
  struct is_relocatable;

  template <typename T>
  struct is_relocatable<score::result::SafeResult<T>> {
    static constexpr bool value = is_relocatable<T>::value;
  };

  template <>
  struct is_relocatable<score::result::SafeResult<bool>> {
    static constexpr bool value = true;
  };

  template <>
  struct is_relocatable<score::result::SafeResult<int>> {
    static constexpr bool value = true;
  };

  template <>
  struct is_relocatable<score::result::SafeResult<float>> {
    static constexpr bool value = true;
  };

  template <>
  struct is_relocatable<score::result::SafeResult<double>> {
    static constexpr bool value = true;
  };

}  // namespace cxx

}  // namespace score::result

#endif  // SCORE_LIB_RESULT_SAFE_RESULT_CXX_H_
