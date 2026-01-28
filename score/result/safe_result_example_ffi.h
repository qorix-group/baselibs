#ifndef SCORE_LIB_RESULT_SAFE_RESULT_EXAMPLE_FFI_H_
#define SCORE_LIB_RESULT_SAFE_RESULT_EXAMPLE_FFI_H_

#include "score/result/safe_result.h"

namespace score::result
{
class Complex
{
  public:
    Complex() : flag_(false), number_(0), decimal_(0.0f), texts_(std::make_unique<std::vector<std::string>>()) {}

    Complex(const Complex&) = delete;
    Complex(Complex&&) = default;
    Complex& operator=(const Complex&) = delete;
    Complex& operator=(Complex&&) = default;

    ~Complex() = default;

    void SetFlag(bool flag)
    {
        flag_ = flag;
    }
    void SetNumber(int number)
    {
        number_ = number;
    }
    void SetDecimal(float decimal)
    {
        decimal_ = decimal;
    }
    void AddText(const std::string& text)
    {
        texts_->push_back(text);
    }

    bool GetFlag() const
    {
        return flag_;
    }
    int GetNumber() const
    {
        return number_;
    }
    float GetDecimal() const
    {
        return decimal_;
    }
    const std::vector<std::string>& GetTexts() const
    {
        return *texts_;
    }

  private:
    bool flag_;
    int number_;
    float decimal_;
    std::unique_ptr<std::vector<std::string>> texts_;
};

}  // namespace score::result

// Register Complex type OUTSIDE the namespace to ensure proper template specialization
REGISTER_COMPLEX_TYPE_SIGNATURE_DIRECT(score::result::Complex, "{b, i32, f32, vec<str>}");

// FFI functions that C++ exposes (must be at global scope for extern "C")
extern "C" {
void cpp_check_configuration_valid(score::result::SafeResult<bool>* result);
void cpp_get_complex_configuration(score::result::SafeResult<score::result::Complex>* result);
void cpp_get_complex_configuration_force_invalid(score::result::SafeResult<score::result::Complex>* result);
}

#endif  // SCORE_LIB_RESULT_SAFE_RESULT_EXAMPLE_FFI_H_
