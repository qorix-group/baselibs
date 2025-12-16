// safe_result_example.cpp
//
// Example usage of SafeResult with compile-time type signature verification
// Demonstrates the type-safety guarantees provided by CRC32 checksums

#include <iostream>
#include "score/result/safe_result.h"

namespace example {

using namespace score::result;

// ============================================================================
// Example 1: Simple SafeResult with bool
// ============================================================================

/// Returns a SafeResult<bool> with success/failure indication
/// The type signature is computed at compile time as:
/// - TypeSignature<bool>::value = "b"
/// - CRC32("b") = computed at compile time
SafeResult<bool> CheckConfiguration() {
  // ... perform checks ...
  return SafeResult<bool>(true);
}

// ============================================================================
// Example 2: SafeResult with integer return
// ============================================================================

/// Returns a SafeResult<int> with error handling
/// Type signature:
/// - TypeSignature<int>::value = "i32"
/// - CRC32("i32") = computed at compile time
SafeResult<int> ReadConfigurationValue(const std::string& key) {
  if (key.empty()) {
    return SafeResult<int>(
        MakeUnexpected(ErrorCode::kInvalidArgument, "Key is empty"));
  }

  // Simulate reading a value
  int value = 42;
  return SafeResult<int>(value);
}

// ============================================================================
// Example 3: Accessing SafeResult
// ============================================================================

void DemoAccessPatterns() {
  // Create a SafeResult<bool>
  auto result = CheckConfiguration();

  // Verify the type signature checksum
  std::cout << "Checksum: 0x" << std::hex << result.GetChecksum() << "\n";

  if (result.VerifyChecksum()) {
    std::cout << "Type signature verified!\n";
  } else {
    std::cout << "ERROR: Type signature mismatch!\n";
    return;
  }

  // Access the underlying Result
  if (result.HasValue()) {
    std::cout << "Configuration is valid\n";
    bool value = result.ExtractValue();
    std::cout << "Value: " << (value ? "true" : "false") << "\n";
  }
}

// ============================================================================
// Example 4: Compile-Time Type Signature Generation
// ============================================================================

/// Demonstrates compile-time type signature generation
void DemoCompileTimeSignatures() {
  // These are all constexpr and computed at compile time:

  constexpr std::string_view bool_sig = TypeSignature<bool>::value;
  std::cout << "bool signature: " << bool_sig << "\n";  // Output: "b"

  constexpr std::string_view int_sig = TypeSignature<int>::value;
  std::cout << "int signature: " << int_sig << "\n";  // Output: "i32"

  constexpr std::string_view float_sig = TypeSignature<float>::value;
  std::cout << "float signature: " << float_sig << "\n";  // Output: "f32"

  // CRC32 checksums are computed at compile time
  constexpr uint32_t bool_crc = ComputeCrc32(bool_sig);
  constexpr uint32_t int_crc = ComputeCrc32(int_sig);

  std::cout << "bool CRC32: 0x" << std::hex << bool_crc << "\n";
  std::cout << "int CRC32: 0x" << std::hex << int_crc << "\n";
}

// ============================================================================
// Example 5: SafeResult as Function Return Type
// ============================================================================

/// A more realistic example with error handling
/// This function returns SafeResult<float> with full type verification
SafeResult<float> CalculateTemperature(float rawSensorValue) {
  // Validate input
  if (rawSensorValue < -50.0f || rawSensorValue > 150.0f) {
    return SafeResult<float>(
        MakeUnexpected(ErrorCode::kOutOfRange, "Sensor value out of range"));
  }

  // Perform calculation
  float calibrated = rawSensorValue * 1.05f;  // Apply calibration
  return SafeResult<float>(calibrated);
}

/// Client code that uses the SafeResult
void UseTemperatureResult() {
  auto temp_result = CalculateTemperature(25.0f);

  // Verify the type signature (this should always pass if C++ and Rust agree)
  if (!temp_result.VerifyChecksum()) {
    std::cerr << "Type mismatch detected!\n";
    return;
  }

  // Now safely use the result
  if (temp_result.HasValue()) {
    float temp = temp_result.ExtractValue();
    std::cout << "Temperature: " << temp << "°C\n";
  } else {
    std::cout << "Failed to read temperature\n";
  }
}

// ============================================================================
// Example 6: Type Signature Guarantees
// ============================================================================

/// Demonstrates the type safety guarantees
/// If C++ and Rust have different type definitions, the CRC32 will mismatch!

// Suppose Rust defines the result expecting:
//   SafeResult<bool> with signature "b"
//   CRC32("b") = X
//
// But C++ mistakenly creates:
//   SafeResult<int> with signature "i32"
//   CRC32("i32") = Y
//
// Where X != Y
//
// When Rust receives it, verification will fail because:
//   Expected: CRC32("b") = X
//   Received: CRC32("i32") = Y
// Result: ChecksumMismatch error

/// This function intentionally creates a type mismatch for demonstration
SafeResult<int> IntentionalMismatch() {
  // This is SafeResult<int>, but Rust might expect SafeResult<bool>
  return SafeResult<int>(42);
}

// ============================================================================
// Example 7: Advanced - Multiple Type Combinations
// ============================================================================

/// Demonstrates that different type combinations produce different checksums

void DemoTypeVariations() {
  // Different types → Different checksums
  const uint32_t checksum_bool = SafeResult<bool>::GetTypeChecksum();
  const uint32_t checksum_int = SafeResult<int>::GetTypeChecksum();
  const uint32_t checksum_float = SafeResult<float>::GetTypeChecksum();

  std::cout << "SafeResult<bool> checksum: 0x" << std::hex << checksum_bool
            << "\n";
  std::cout << "SafeResult<int> checksum: 0x" << std::hex << checksum_int
            << "\n";
  std::cout << "SafeResult<float> checksum: 0x" << std::hex << checksum_float
            << "\n";

  // All should be different
  assert(checksum_bool != checksum_int);
  assert(checksum_int != checksum_float);
  assert(checksum_bool != checksum_float);
}

// ============================================================================
// Example 8: FFI Boundary with Type Verification
// ============================================================================

/// Demonstrates SafeResult at the C++/Rust boundary
///
/// On the C++ side:
/// - Creates SafeResult<bool> with CRC32 computed at compile time
/// - Embeds CRC32 in the return value
/// - Returns by value across calling convention
///
/// On the Rust side:
/// - Receives the SafeResult as #[repr(C)] struct
/// - Extracts and verifies CRC32 matches expected type signature
/// - If mismatch, returns ChecksumMismatch error
/// - Otherwise, safely converts to std::result::Result<T, E>

extern "C" SafeResult<bool> FFIFunctionReturnsResult() {
  // The CRC32 is embedded here
  return SafeResult<bool>(true);
}

// To call from Rust:
//   let safe_result = ffi::ffi_function_returns_result();
//   safe_result.verify_type_signature()?;  // Will return ChecksumMismatch if wrong
//   let result: Result<bool, Error> = safe_result.into();

}  // namespace example

int main() {
  std::cout << "SafeResult Examples\n"
            << "===================\n\n";

  example::DemoCompileTimeSignatures();
  std::cout << "\n";

  example::DemoAccessPatterns();
  std::cout << "\n";

  example::UseTemperatureResult();
  std::cout << "\n";

  example::DemoTypeVariations();

  return 0;
}
