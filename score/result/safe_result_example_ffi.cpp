#include "score/result/safe_result_example_ffi.h"

namespace score::result {

extern "C" SafeResult<bool> cpp_check_configuration_valid() {
  // Simulate configuration check
  bool is_valid = true;
  return SafeResult<bool>(is_valid);
}

extern "C" SafeResult<Complex> cpp_get_complex_configuration() {
  Complex config;
  config.SetFlag(true);
  config.SetNumber(42);
  config.SetDecimal(3.14f);
  config.AddText("example");
  config.AddText("configuration");
  return SafeResult<Complex>(std::move(config));
}

}
