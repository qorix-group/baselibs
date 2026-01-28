#include "score/result/safe_result_example_ffi.h"

namespace score::result
{

extern "C" void cpp_check_configuration_valid(SafeResult<bool>* result)
{
    // Simulate configuration check
    bool is_valid = true;
    new (result) SafeResult(is_valid);
}

extern "C" void cpp_get_complex_configuration(SafeResult<Complex>* result)
{
    Complex config;
    config.SetFlag(true);
    config.SetNumber(42);
    config.SetDecimal(3.14f);
    config.AddText("example");
    config.AddText("configuration");
    new (result) SafeResult(std::move(config));
}

// This is an "alias" to force an invalid type signature for testing
// On the Rust side, this will cause a checksum mismatch as the type
// will be defined differently.
extern "C" void cpp_get_complex_configuration_force_invalid(SafeResult<Complex>* result)
{
    cpp_get_complex_configuration(result);
}

}  // namespace score::result
