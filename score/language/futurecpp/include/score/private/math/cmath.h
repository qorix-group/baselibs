///
/// \file
/// \copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// The safety manual (broken_link_c/issue/3871296) for QNX requires to compile math functions
/// with `-fno-builtin`. `amp` is taking care of this by providing for
/// Trigonometric Functions
///     - acos(), acosf()
///     - asin(), asinf()
///     - atan2(), atan2f()
///     - atan(), atanf()
///     - cos(), cosf()
///     - sin(), sinf()
///     - tan(), tanf()
/// Hyperbolic Functions
///     - acosh(), acoshf()
///     - asinh(), asinhf()
///     - atanh(), atanhf()
///     - cosh(), coshf()
///     - sinh(), sinhf()
///     - tanh(), tanhf()
/// Logarithmic Functions
///     - ilogb(), ilogbf()
///     - log10(), log10f()
///     - log1p(), log1pf()
///     - log2(), log2f()
///     - logb(), logbf()
///     - log(), logf()
/// Comparison and Classification Functions
///     - fmax(), fmaxf()
///     - fmin(), fminf()
/// Rounding Functions
///     - ceil(), ceilf()
///     - floor(), floorf()
///     - lrint(), lrintf()
///     - lround(), lroundf()
///     - nearbyint(), nearbyintf()
///     - nextafter(), nextafterf()
///     - nexttoward(), nexttowardf()
///     - rint(), rintf()
///     - round(), roundf()
///     - trunc(), truncf()
/// Statistical Functions
///     - erfc(), erfcf()
///     - erf(), erff()
/// Power Functions
///     - cbrt(), cbrtf()
///     - exp2(), exp2f()
///     - expm1(), expm1f()
///     - exp(), expf()
///     - pow(), powf()
///     - sqrt(), sqrtf()
/// Gamma Functions
///     - lgamma(), lgammaf()
///     - tgamma(), tgammaf()
/// Remainder Functions
///     - fmod(), fmodf()
///     - modf(), modff()
///     - remainder(), remainderf()
///     - remquo(), remquof()
/// Miscellaneous Functions
///     - copysign(), copysignf()
///     - fabs(), fabsf()
///     - fdim(), fdimf()
///     - fma(), fmaf()
///     - frexp(), frexpf()
///     - hypot(), hypotf()
///     - ldexp(), ldexpf()
///     - nan(), nanf()
///     - scalbln(), scalblnf()
///     - scalbn(), scalbnf()
/// Macros Comparison and Classification
///     - fpclassify()
///     - isfinite()
///     - isinf()
///     - isnan()
///     - isnormal()
///     - signbit()
///     - isgreater()
///     - isgreaterequal()
///     - isless()
///     - islessequal()
///     - islessgreater()
///     - isunordered()
/// a corresponding function in amp. The lib is compiled with `-fno-builtin`.
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_CMATH_H
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_CMATH_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    double score_acos(double num);
    float score_acosf(float num);

    double score_asin(double num);
    float score_asinf(float num);

    double score_atan2(double y, double x);
    float score_atan2f(float y, float x);

    double score_atan(double num);
    float score_atanf(float num);

    double score_cos(double num);
    float score_cosf(float num);

    double score_sin(double num);
    float score_sinf(float num);

    double score_tan(double num);
    float score_tanf(float num);

    double score_acosh(double num);
    float score_acoshf(float num);

    double score_asinh(double num);
    float score_asinhf(float num);

    double score_atanh(double num);
    float score_atanhf(float num);

    double score_cosh(double num);
    float score_coshf(float num);

    double score_sinh(double num);
    float score_sinhf(float num);

    double score_tanh(double num);
    float score_tanhf(float num);

    int score_ilogb(double num);
    int score_ilogbf(float num);

    double score_log10(double num);
    float score_log10f(float num);

    double score_log1p(double num);
    float score_log1pf(float num);

    double score_log2(double num);
    float score_log2f(float num);

    double score_logb(double num);
    float score_logbf(float num);

    double score_log(double num);
    float score_logf(float num);

    double score_fmax(double x, double y);
    float score_fmaxf(float x, float y);

    double score_fmin(double x, double y);
    float score_fminf(float x, float y);

    double score_ceil(double num);
    float score_ceilf(float num);

    double score_floor(double num);
    float score_floorf(float num);

    long score_lrint(double num);
    long score_lrintf(float num);

    long score_lround(double num);
    long score_lroundf(float num);

    double score_nearbyint(double num);
    float score_nearbyintf(float num);

    double score_nextafter(double from, double to);
    float score_nextafterf(float from, float to);

    double score_nexttoward(double from, double to);
    float score_nexttowardf(float from, float to);

    double score_rint(double num);
    float score_rintf(float num);

    double score_round(double num);
    float score_roundf(float num);

    double score_trunc(double num);
    float score_truncf(float num);

    double score_erfc(double num);
    float score_erfcf(float num);

    double score_erf(double num);
    float score_erff(float num);

    double score_cbrt(double num);
    float score_cbrtf(float num);

    double score_exp2(double num);
    float score_exp2f(float num);

    double score_expm1(double num);
    float score_expm1f(float num);

    double score_exp(double num);
    float score_expf(float num);

    double score_pow(double base, double exp);
    float score_powf(float base, float exp);

    double score_sqrt(double num);
    float score_sqrtf(float num);

    double score_lgamma(double num);
    float score_lgammaf(float num);

    double score_tgamma(double num);
    float score_tgammaf(float num);

    double score_fmod(double x, double y);
    float score_fmodf(float x, float y);

    double score_modf(double num, double* iptr);
    float score_modff(float num, float* iptr);

    double score_remainder(double x, double y);
    float score_remainderf(float x, float y);

    double score_remquo(double x, double y, int* quo);
    float score_remquof(float x, float y, int* quo);

    double score_copysign(double mag, double sgn);
    float score_copysignf(float mag, float sgn);

    double score_fabs(double arg);
    float score_fabsf(float arg);

    double score_fdim(double x, double y);
    float score_fdimf(float x, float y);

    double score_fma(double x, double y, double z);
    float score_fmaf(float x, float y, float z);

    double score_frexp(double num, int* exp);
    float score_frexpf(float num, int* exp);

    double score_hypot(double x, double y);
    float score_hypotf(float x, float y);

    double score_ldexp(double num, int exp);
    float score_ldexpf(float num, int exp);

    double score_nan(const char* arg);
    float score_nanf(const char* arg);

    double score_scalbln(double num, int exp);
    float score_scalblnf(float num, int exp);

    double score_scalbn(double num, int exp);
    float score_scalbnf(float num, int exp);

    int score_fpclassify(double num);
    int score_fpclassifyf(float num);

    int score_isfinite(double num);
    int score_isfinitef(float num);

    int score_isinf(double num);
    int score_isinff(float num);

    int score_isnan(double num);
    int score_isnanf(float num);

    int score_isnormal(double num);
    int score_isnormalf(float num);

    int score_signbit(double num);
    int score_signbitf(float num);

    int score_isgreater(double x, double y);
    int score_isgreaterf(float x, float y);

    int score_isgreaterequal(double x, double y);
    int score_isgreaterequalf(float x, float y);

    int score_isless(double x, double y);
    int score_islessf(float x, float y);

    int score_islessequal(double x, double y);
    int score_islessequalf(float x, float y);

    int score_islessgreater(double x, double y);
    int score_islessgreaterf(float x, float y);

    int score_isunordered(double x, double y);
    int score_isunorderedf(float x, float y);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_CMATH_H
