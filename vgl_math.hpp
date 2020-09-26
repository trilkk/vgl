#ifndef VGL_MATH_HPP
#define VGL_MATH_HPP

#include "vgl_algorithm.hpp"
#include "vgl_assert.hpp"
#include "vgl_type_traits.hpp"

#include <cmath>

#if defined(VGL_IS_CONSTANT_EVALUATED)
/// Math functions have constexpr path.
#define VGL_MATH_CONSTEXPR constexpr
#else
/// No constexpr path for math functions.
#define VGL_MATH_CONSTEXPR
#endif

namespace vgl
{

using std::abs;

namespace detail
{

/// Rounds floating point value towards negative infinity.
///
/// \param val Value to round.
/// \return Rounded integer value.
template<typename T> constexpr T floor_template(T val) noexcept
{
    if(val < static_cast<T>(0))
    {
        return static_cast<T>(static_cast<int>(val) - 1);
    }
    return static_cast<T>(static_cast<int>(val));
}

/// Remainder function.
///
/// \param val Value to divide.
/// \param divisor Divisor to the value.
/// \return Remainder of val / divisor.
template<typename T> constexpr T remainder_template(T val, T divisor) noexcept
{
    return val - (floor_template(val / divisor) * divisor);
}
/// \cond
template<> constexpr int remainder_template(int val, int divisor) noexcept
{
    return val % divisor;
}
/// \endcond

/// Congruence function.
///
/// \param val Value to divide.
/// \param divisor Divisor, must be positive.
/// \return Value in [0, divisor[.
template<typename T> constexpr T congr_template(T val, T divisor) noexcept
{
    VGL_ASSERT(divisor > static_cast<T>(0));
    if(static_cast<T>(0) <= val)
    {
        return remainder_template(val, divisor);
    }
    T ret = divisor - remainder_template(-val, divisor);
    return (ret < divisor) ? ret : static_cast<T>(0);
}

#if defined(VGL_IS_CONSTANT_EVALUATED)

/// Calculate power (compile-time).
///
/// \param val Value.
/// \param power Power to raise the value to.
/// \return Power of value.
constexpr double compile_time_pow(double val, int power)
{
    if(power == 0)
    {
        return 1.0;
    }
    if(power < 0)
    {
        return 1.0 / compile_time_pow(val, -power);
    }
    double ret = val;
    for(int ii = 1; (ii < power); ++ii)
    {
        ret *= val;
    }
    return ret;
}

/// Calculate factorial (compile-time).
///
/// \param op Value.
/// \return Factorial of value.
constexpr int64_t compile_time_factorial(int op)
{
    if(op <= 1)
    {
        return 1;
    }
    int64_t ret = 1;
    for(int64_t ii = 2; (ii <= static_cast<int64_t>(op)); ++ii)
    {
        ret *= ii;
    }
    return ret;
}

/// Find cosine through series expansion.
///
/// \param op Value.
/// \return Cosine of value.
constexpr double compile_time_cos(double op)
{
    double sum = 1.0;
    double mul = -1.0;
    for(int ii = 2; (ii < 20); ii += 2)
    {
        sum += mul * compile_time_pow(op, ii) / static_cast<double>(compile_time_factorial(ii));
        mul *= -1.0;
    }
    return sum;
}

/// Find sine through series expansion.
///
/// \param op Value.
/// \return Sine of value.
constexpr long double compile_time_sin(double op)
{
    double sum = op;
    double mul = -1.0;
    for(int ii = 3; (ii < 20); ii += 2)
    {
        sum += mul * compile_time_pow(op, ii) / static_cast<double>(compile_time_factorial(ii));
        mul *= -1.0;
    }
    return sum;
}

/// Iteratively find square root.
///
/// Algorithm directly from Wikipedia's description of Newton's method:
/// https://en.wikipedia.org/wiki/Newton%27s_method#Square_root
///
/// \param tgt Target value.
/// \param curr Current guess.
/// \return Square root of target value.
double constexpr compile_time_sqrt(double tgt, double curr)
{
    double next = curr - (((curr * curr) - tgt) / (2 * curr));

    if(abs((next * next) - tgt) < std::numeric_limits<double>::epsilon() * 8.0)
    {
        return next;
    }
    return compile_time_sqrt(tgt, next);
}

#endif

}

/// Check if floats match on some relative level.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return True if match, false if floats definitely differ.
constexpr bool floats_almost_equal(float lhs, float rhs) noexcept
{
    if(lhs == rhs)
    {
        return true;
    }
    // The multiplier has been chosen empirically.
    const float ALMOST_EQUAL_MUL = 0.1f;
    float lr = (lhs - rhs) * ALMOST_EQUAL_MUL;
    float rl = (rhs - lhs) * ALMOST_EQUAL_MUL;
    return ((lhs + lr) == lhs) || ((lhs + rl) == lhs) || ((rhs + lr) == rhs) || ((rhs + rl) == rhs);
}

/// Clamp float.
///
/// \param val Value.
/// \param min_val Minimum value.
/// \param max_val Maximum value.
constexpr float clamp(float val, float min_val, float max_val) noexcept
{
    return min(max(val, min_val), max_val);
}

/// Round float to nearest integer boundary.
///
/// Rounds halfway away from zero.
///
/// \param val Value to round.
/// \return Rounded integer value.
constexpr int iround(float val) noexcept
{
    if(val < 0.0f)
    {
        return -static_cast<int>(-val + 0.5f);
    }
    return static_cast<int>(val + 0.5f);
}
/// Round float to nearest integer boundary.
///
/// Rounds halfway away from zero.
///
/// \param val Value to round.
/// \return Rounded integer value.
constexpr float round(float val) noexcept
{
    return static_cast<float>(iround(val));
}

/// Convert 4.12 signed fixed point number to floating point.
///
/// \param op Input number.
/// \return Converted number.
constexpr float fixed_4_12_to_float(int16_t op) noexcept
{
    return static_cast<float>(op) * (1.0f / 4096.0f);
}

/// Convert 8.8 signed fixed point number to floating point.
///
/// \param op Input number.
/// \return Converted number.
constexpr float fixed_8_8_to_float(int16_t op) noexcept
{
    return static_cast<float>(op) * (1.0f / 256.0f);
}

/// Convert 14.2 signed fixed point number to floating point.
///
/// \param op Input number.
/// \return Converted number.
constexpr float fixed_14_2_to_float(int16_t op) noexcept
{
    return static_cast<float>(op) * 0.25f;
}

/// Convert to normalized float space.
///
/// \param op Input integer value.
/// \return Normalized floating point value.
constexpr float to_fnorm(int16_t op) noexcept
{
    return static_cast<float>(static_cast<int>(op) + 32768) * (2.0f / 65535.0f) - 1.0f;
}

/// Convert to normalized float space.
///
/// \param op Input integer value.
/// \return Normalized floating point value.
constexpr float to_fnorm(uint8_t op) noexcept
{
    return static_cast<float>(op) * (1.0f / 255.0f);
}

/// Weight normalized space float away from half.
///
/// \param op Input value.
/// \return Weighted value.
constexpr float fnorm_weigh_away(float op) noexcept
{
    if(op > 0.5f)
    {
        float ret = (1.0f - op) * 2.0f;
        return 1.0f - (ret * ret) * 0.5f;
    }
    float ret = op * 2.0f;
    return ret * ret * 0.5f;
}

/// Linear step function.
///
/// \param edge0 First edge.
/// \param edge1 Second edge.
/// \param value Value.
/// \return 0 if value <= edge0, 1 if value >= edge1, linearly interpolated value otherwise.
constexpr float linear_step(float edge0, float edge1, float value) noexcept
{
    if(value <= edge0)
    {
        return 0.0f;
    }
    if(value >= edge1)
    {
        return 1.0f;
    }
    return (value - edge0) / (edge1 - edge0);
}

/// Linear step down function.
///
/// \param edge0 First edge.
/// \param edge1 Second edge.
/// \param value Value.
/// \return Linearly stepped value between edges, otherwise 1 before edge0 and 0 after edge1.
constexpr float linear_step_down(float edge0, float edge1, float value) noexcept
{
    return 1.0f - linear_step(edge0, edge1, value);
}

/// Smooth step function.
///
/// \param edge0 First edge.
/// \param edge1 Second edge.
/// \param value Value.
/// \return 0 if value <= edge0, 1 if value >= edge1, smoothstep interpolated value otherwise.
constexpr float smooth_step(float edge0, float edge1, float value) noexcept
{
    float ret = clamp((value - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return ret * ret * (3.0f - 2.0f * ret);
}

/// Mix two floats.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \param ratio Ratio.
/// \return Value mixed from lhs and rhs. Overflows if ratio is outside [0, 1].
constexpr float mix(float lhs, float rhs, float ratio) noexcept
{
    return lhs + (rhs - lhs) * ratio;
}

/// Mix two signed integers.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \param ratio Mixing ratio.
/// \return Mixing value.
int16_t constexpr mix(int16_t lhs, int16_t rhs, float ratio) noexcept
{
    float c1 = static_cast<float>(lhs);
    float c2 = static_cast<float>(rhs);
    float ret = clamp(mix(c1, c2, ratio), -32768.0f, 32767.0f);
    return static_cast<int16_t>(iround(ret));
}

/// Mix two unsigned integers.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \param ratio Mixing ratio.
/// \return Mixing value.
uint8_t constexpr mix(uint8_t lhs, uint8_t rhs, float ratio) noexcept
{
    float c1 = static_cast<float>(lhs);
    float c2 = static_cast<float>(rhs);
    float ret = clamp(mix(c1, c2, ratio), 0.0f, 255.0f);
    return static_cast<uint8_t>(iround(ret));
}

/// Linear mix function.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \param ratio Mixing ratio.
/// \return Linearly mixed value between lhs and rhs. Clamped if ratio is outside [0, 1].
constexpr float linear_mix(float lhs, float rhs, float ratio) noexcept
{
    return mix(lhs, rhs, clamp(ratio, 0.0f, 1.0f));
}

/// Smooth mix function.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \param ratio Mixing ratio.
/// \return Smoothly mixed value between lhs and rhs. Clamped if ratio is outside [0, 1].
constexpr float smooth_mix(float lhs, float rhs, float ratio) noexcept
{
    return mix(lhs, rhs, smooth_step(0.0f, 1.0f, ratio));
}

/// Modulate two unsigned integers.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Modulated value.
constexpr uint8_t modulate(uint8_t lhs, uint8_t rhs) noexcept
{
    float ret = to_fnorm(lhs) * to_fnorm(rhs);
    return static_cast<uint8_t>(iround(ret * 255.0f));
}

/// Remainder function.
///
/// \param val Value to divide.
/// \param divisor Divisor to the value.
/// \return Remainder of val / divisor.
constexpr float remainder(float val, float divisor) noexcept
{
    return detail::remainder_template(val, divisor);
}

/// Congruence function.
///
/// \param val Value to divide.
/// \param divisor Divisor, must be positive.
/// \return Value in [0, divisor[
constexpr int congr(int val, int divisor) noexcept
{
    return detail::congr_template(val, divisor);
}

/// Congruence function.
///
/// \param val Value to divide.
/// \param divisor Divisor, must be positive.
/// \return Value in [0, divisor[.
constexpr float congr(float val, float divisor) noexcept
{
    return detail::congr_template(val, divisor);
}

/// Convert RGB values to luma.
///
/// \param rr Red.
/// \param gg Green.
/// \param bb Blue.
/// \return Luma.
constexpr float rgb_to_luma(float rr, float gg, float bb) noexcept
{
    return rr * 0.2126f + gg * 0.7152f + bb * 0.0722f;
}

#if 0
/// 2-D linear interpolation.
///
/// \param x X-offset, scaled from 0 to 1.
/// \param y Y-offset, scaled from 0 to 1.
/// \param x1y1 value at (0, 0).
/// \param x2y1 value at (1, 0).
/// \param x1y2 value at (0, 1).
/// \param x2y2 value at (1, 1).
template <typename T> constexpr T lerp(float x, float y, const T& x1y1, const T& x2y1, const T& x1y2, const T& x2y2)
{
    const T ONE = static_cast<T>(1);
    return x1y1 * (ONE - x) * (ONE - y) +
        x2y1 * x * (ONE - y) +
        x1y2 * (ONE - x) * y +
        x2y2 * x * y;
}
#endif

/// Floor wrapper.
///
/// \param val Value to round.
/// \return Rounded integer value.
VGL_MATH_CONSTEXPR float floor(float val) noexcept
{
#if defined(VGL_IS_CONSTANT_EVALUATED)
    if(is_constant_evaluated())
    {
        return detail::floor_template(val);
    }
#endif
    return dnload_floorf(val);
}

/// Cosine wrapper.
///
/// \param op Value in radians.
/// \return Cosine of value.
VGL_MATH_CONSTEXPR float cos(float op) noexcept
{
#if defined(VGL_IS_CONSTANT_EVALUATED)
    if(is_constant_evaluated())
    {
        double val = detail::congr_template(static_cast<double>(op), M_PI * 2.0);
        if(val >= M_PI)
        {
            val -= 2.0 * M_PI;
        }
        return static_cast<float>(detail::compile_time_cos(val));
    }
#endif
    return dnload_cosf(op);
}

/// Sine wrapper.
///
/// \param op Value in radians.
/// \return Sine of value.
VGL_MATH_CONSTEXPR float sin(float op) noexcept
{
#if defined(VGL_IS_CONSTANT_EVALUATED)
    if(is_constant_evaluated())
    {
        double val = detail::congr_template(static_cast<double>(op), M_PI * 2.0);
        if(val >= M_PI)
        {
            val -= 2.0 * M_PI;
        }
        return static_cast<float>(detail::compile_time_sin(val));
    }
#endif
    return dnload_sinf(op);
}

/// Square root wrapper.
///
/// \param op Value.
/// \return Square root of value.
VGL_MATH_CONSTEXPR float sqrt(float op) noexcept
{
#if defined(VGL_IS_CONSTANT_EVALUATED)
    if(is_constant_evaluated())
    {
        if((op <= 0.0f) || (op >= std::numeric_limits<float>::infinity()))
        {
            return 0.0f;
        }
        return static_cast<float>(detail::compile_time_sqrt(static_cast<double>(op), static_cast<double>(op / 2.0f)));
    }
#endif
    return dnload_sqrtf(op);
}

/// Power wrapper.
///
/// \param val Value.
/// \param power Power to raise the value to.
/// \return Power of value.
float pow(float val, float power)
{
    return dnload_powf(val, power);
}

}

#endif
