///
/// \file
/// \copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#ifdef __cplusplus
#error "File must be compiled as C source file"
#endif // __cplusplus

#include <score/private/math/cmath.h>

#include <math.h>

double score_acos(double num) { return acos(num); } // NOLINT(bmw-no-standard-math)
float score_acosf(float num) { return acosf(num); } // NOLINT(bmw-no-standard-math)

double score_asin(double num) { return asin(num); } // NOLINT(bmw-no-standard-math)
float score_asinf(float num) { return asinf(num); } // NOLINT(bmw-no-standard-math)

double score_atan2(double y, double x) { return atan2(y, x); } // NOLINT(bmw-no-standard-math)
float score_atan2f(float y, float x) { return atan2f(y, x); }  // NOLINT(bmw-no-standard-math)

double score_atan(double num) { return atan(num); } // NOLINT(bmw-no-standard-math)
float score_atanf(float num) { return atanf(num); } // NOLINT(bmw-no-standard-math)

double score_cos(double num) { return cos(num); } // NOLINT(bmw-no-standard-math)
float score_cosf(float num) { return cosf(num); } // NOLINT(bmw-no-standard-math)

double score_sin(double num) { return sin(num); } // NOLINT(bmw-no-standard-math)
float score_sinf(float num) { return sinf(num); } // NOLINT(bmw-no-standard-math)

double score_tan(double num) { return tan(num); } // NOLINT(bmw-no-standard-math)
float score_tanf(float num) { return tanf(num); } // NOLINT(bmw-no-standard-math)

double score_acosh(double num) { return acosh(num); } // NOLINT(bmw-no-standard-math)
float score_acoshf(float num) { return acoshf(num); } // NOLINT(bmw-no-standard-math)

double score_asinh(double num) { return asinh(num); } // NOLINT(bmw-no-standard-math)
float score_asinhf(float num) { return asinhf(num); } // NOLINT(bmw-no-standard-math)

double score_atanh(double num) { return atanh(num); } // NOLINT(bmw-no-standard-math)
float score_atanhf(float num) { return atanhf(num); } // NOLINT(bmw-no-standard-math)

double score_cosh(double num) { return cosh(num); } // NOLINT(bmw-no-standard-math)
float score_coshf(float num) { return coshf(num); } // NOLINT(bmw-no-standard-math)

double score_sinh(double num) { return sinh(num); } // NOLINT(bmw-no-standard-math)
float score_sinhf(float num) { return sinhf(num); } // NOLINT(bmw-no-standard-math)

double score_tanh(double num) { return tanh(num); } // NOLINT(bmw-no-standard-math)
float score_tanhf(float num) { return tanhf(num); } // NOLINT(bmw-no-standard-math)

int score_ilogb(double num) { return ilogb(num); }  // NOLINT(bmw-no-standard-math)
int score_ilogbf(float num) { return ilogbf(num); } // NOLINT(bmw-no-standard-math)

double score_log10(double num) { return log10(num); } // NOLINT(bmw-no-standard-math)
float score_log10f(float num) { return log10f(num); } // NOLINT(bmw-no-standard-math)

double score_log1p(double num) { return log1p(num); } // NOLINT(bmw-no-standard-math)
float score_log1pf(float num) { return log1pf(num); } // NOLINT(bmw-no-standard-math)

double score_log2(double num) { return log2(num); } // NOLINT(bmw-no-standard-math)
float score_log2f(float num) { return log2f(num); } // NOLINT(bmw-no-standard-math)

double score_logb(double num) { return logb(num); } // NOLINT(bmw-no-standard-math)
float score_logbf(float num) { return logbf(num); } // NOLINT(bmw-no-standard-math)

double score_log(double num) { return log(num); } // NOLINT(bmw-no-standard-math)
float score_logf(float num) { return logf(num); } // NOLINT(bmw-no-standard-math)

double score_fmax(double x, double y) { return fmax(x, y); } // NOLINT(bmw-no-standard-math)
float score_fmaxf(float x, float y) { return fmaxf(x, y); }  // NOLINT(bmw-no-standard-math)

double score_fmin(double x, double y) { return fmin(x, y); } // NOLINT(bmw-no-standard-math)
float score_fminf(float x, float y) { return fminf(x, y); }  // NOLINT(bmw-no-standard-math)

double score_ceil(double num) { return ceil(num); } // NOLINT(bmw-no-standard-math)
float score_ceilf(float num) { return ceilf(num); } // NOLINT(bmw-no-standard-math)

double score_floor(double num) { return floor(num); } // NOLINT(bmw-no-standard-math)
float score_floorf(float num) { return floorf(num); } // NOLINT(bmw-no-standard-math)

long score_lrint(double num) { return lrint(num); }  // NOLINT(bmw-no-standard-math)
long score_lrintf(float num) { return lrintf(num); } // NOLINT(bmw-no-standard-math)

long score_lround(double num) { return lround(num); }  // NOLINT(bmw-no-standard-math)
long score_lroundf(float num) { return lroundf(num); } // NOLINT(bmw-no-standard-math)

double score_nearbyint(double num) { return nearbyint(num); } // NOLINT(bmw-no-standard-math)
float score_nearbyintf(float num) { return nearbyintf(num); } // NOLINT(bmw-no-standard-math)

double score_nextafter(double from, double to) { return nextafter(from, to); } // NOLINT(bmw-no-standard-math)
float score_nextafterf(float from, float to) { return nextafterf(from, to); }  // NOLINT(bmw-no-standard-math)

double score_nexttoward(double from, double to) { return nexttoward(from, to); } // NOLINT(bmw-no-standard-math)
float score_nexttowardf(float from, float to) { return nexttowardf(from, to); }  // NOLINT(bmw-no-standard-math)

double score_rint(double num) { return rint(num); } // NOLINT(bmw-no-standard-math)
float score_rintf(float num) { return rintf(num); } // NOLINT(bmw-no-standard-math)

double score_round(double num) { return round(num); } // NOLINT(bmw-no-standard-math)
float score_roundf(float num) { return roundf(num); } // NOLINT(bmw-no-standard-math)

double score_trunc(double num) { return trunc(num); } // NOLINT(bmw-no-standard-math)
float score_truncf(float num) { return truncf(num); } // NOLINT(bmw-no-standard-math)

double score_erfc(double num) { return erfc(num); } // NOLINT(bmw-no-standard-math)
float score_erfcf(float num) { return erfcf(num); } // NOLINT(bmw-no-standard-math)

double score_erf(double num) { return erf(num); } // NOLINT(bmw-no-standard-math)
float score_erff(float num) { return erff(num); } // NOLINT(bmw-no-standard-math)

double score_cbrt(double num) { return cbrt(num); } // NOLINT(bmw-no-standard-math)
float score_cbrtf(float num) { return cbrtf(num); } // NOLINT(bmw-no-standard-math)

double score_exp2(double num) { return exp2(num); } // NOLINT(bmw-no-standard-math)
float score_exp2f(float num) { return exp2f(num); } // NOLINT(bmw-no-standard-math)

double score_expm1(double num) { return expm1(num); } // NOLINT(bmw-no-standard-math)
float score_expm1f(float num) { return expm1f(num); } // NOLINT(bmw-no-standard-math)

double score_exp(double num) { return exp(num); } // NOLINT(bmw-no-standard-math)
float score_expf(float num) { return expf(num); } // NOLINT(bmw-no-standard-math)

double score_pow(double base, double exp) { return pow(base, exp); } // NOLINT(bmw-no-standard-math)
float score_powf(float base, float exp) { return powf(base, exp); }  // NOLINT(bmw-no-standard-math)

double score_sqrt(double num) { return sqrt(num); } // NOLINT(bmw-no-standard-math)
float score_sqrtf(float num) { return sqrtf(num); } // NOLINT(bmw-no-standard-math)

double score_lgamma(double num) { return lgamma(num); } // NOLINT(bmw-no-standard-math)
float score_lgammaf(float num) { return lgammaf(num); } // NOLINT(bmw-no-standard-math)

double score_tgamma(double num) { return tgamma(num); } // NOLINT(bmw-no-standard-math)
float score_tgammaf(float num) { return tgammaf(num); } // NOLINT(bmw-no-standard-math)

double score_fmod(double x, double y) { return fmod(x, y); } // NOLINT(bmw-no-standard-math)
float score_fmodf(float x, float y) { return fmodf(x, y); }  // NOLINT(bmw-no-standard-math)

double score_modf(double num, double* iptr) { return modf(num, iptr); } // NOLINT(bmw-no-standard-math)
float score_modff(float num, float* iptr) { return modff(num, iptr); }  // NOLINT(bmw-no-standard-math)

double score_remainder(double x, double y) { return remainder(x, y); } // NOLINT(bmw-no-standard-math)
float score_remainderf(float x, float y) { return remainderf(x, y); }  // NOLINT(bmw-no-standard-math)

double score_remquo(double x, double y, int* quo) { return remquo(x, y, quo); } // NOLINT(bmw-no-standard-math)
float score_remquof(float x, float y, int* quo) { return remquof(x, y, quo); }  // NOLINT(bmw-no-standard-math)

double score_copysign(double mag, double sgn) { return copysign(mag, sgn); } // NOLINT(bmw-no-standard-math)
float score_copysignf(float mag, float sgn) { return copysignf(mag, sgn); }  // NOLINT(bmw-no-standard-math)

double score_fabs(double arg) { return fabs(arg); } // NOLINT(bmw-no-standard-math)
float score_fabsf(float arg) { return fabsf(arg); } // NOLINT(bmw-no-standard-math)

double score_fdim(double x, double y) { return fdim(x, y); } // NOLINT(bmw-no-standard-math)
float score_fdimf(float x, float y) { return fdimf(x, y); }  // NOLINT(bmw-no-standard-math)

double score_fma(double x, double y, double z) { return fma(x, y, z); } // NOLINT(bmw-no-standard-math)
float score_fmaf(float x, float y, float z) { return fmaf(x, y, z); }   // NOLINT(bmw-no-standard-math)

double score_frexp(double num, int* exp) { return frexp(num, exp); } // NOLINT(bmw-no-standard-math)
float score_frexpf(float num, int* exp) { return frexpf(num, exp); } // NOLINT(bmw-no-standard-math)

double score_hypot(double x, double y) { return hypot(x, y); } // NOLINT(bmw-no-standard-math)
float score_hypotf(float x, float y) { return hypotf(x, y); }  // NOLINT(bmw-no-standard-math)

double score_ldexp(double num, int exp) { return ldexp(num, exp); } // NOLINT(bmw-no-standard-math)
float score_ldexpf(float num, int exp) { return ldexpf(num, exp); } // NOLINT(bmw-no-standard-math)

double score_nan(const char* arg) { return nan(arg); }  // NOLINT(bmw-no-standard-math)
float score_nanf(const char* arg) { return nanf(arg); } // NOLINT(bmw-no-standard-math)

double score_scalbln(double num, int exp) { return scalbln(num, exp); } // NOLINT(bmw-no-standard-math)
float score_scalblnf(float num, int exp) { return scalblnf(num, exp); } // NOLINT(bmw-no-standard-math)

double score_scalbn(double num, int exp) { return scalbn(num, exp); } // NOLINT(bmw-no-standard-math)
float score_scalbnf(float num, int exp) { return scalbnf(num, exp); } // NOLINT(bmw-no-standard-math)

int score_fpclassify(double num) { return fpclassify(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)
int score_fpclassifyf(float num) { return fpclassify(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)

int score_isfinite(double num) { return isfinite(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)
int score_isfinitef(float num) { return isfinite(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)

int score_isinf(double num) { return isinf(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)
int score_isinff(float num) { return isinf(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)

int score_isnan(double num) { return isnan(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)
int score_isnanf(float num) { return isnan(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)

int score_isnormal(double num) { return isnormal(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)
int score_isnormalf(float num) { return isnormal(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)

int score_signbit(double num) { return signbit(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)
int score_signbitf(float num) { return signbit(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)

int score_isgreater(double x, double y) { return isgreater(x, y); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)
int score_isgreaterf(float x, float y) { return isgreater(x, y); }  // NOLINT(bmw-no-standard-math,bmw-banned-macro)

// NOLINTNEXTLINE(bmw-no-standard-math,bmw-banned-macro)
int score_isgreaterequal(double x, double y) { return isgreaterequal(x, y); }
// NOLINTNEXTLINE(bmw-no-standard-math,bmw-banned-macro)
int score_isgreaterequalf(float x, float y) { return isgreaterequal(x, y); }

int score_isless(double x, double y) { return isless(x, y); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)
int score_islessf(float x, float y) { return isless(x, y); }  // NOLINT(bmw-no-standard-math,bmw-banned-macro)

int score_islessequal(double x, double y) { return islessequal(x, y); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)
int score_islessequalf(float x, float y) { return islessequal(x, y); }  // NOLINT(bmw-no-standard-math,bmw-banned-macro)

// NOLINTNEXTLINE(bmw-no-standard-math,bmw-banned-macro)
int score_islessgreater(double x, double y) { return islessgreater(x, y); }
// NOLINTNEXTLINE(bmw-no-standard-math,bmw-banned-macro)
int score_islessgreaterf(float x, float y) { return islessgreater(x, y); }

int score_isunordered(double x, double y) { return isunordered(x, y); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)
int score_isunorderedf(float x, float y) { return isunordered(x, y); }  // NOLINT(bmw-no-standard-math,bmw-banned-macro)
