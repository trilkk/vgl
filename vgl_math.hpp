#ifndef VGL_MATH_HPP
#define VGL_MATH_HPP

#include "vgl_algorithm.hpp"
#include "vgl_assert.hpp"
#include "vgl_extern_math.hpp"
#include "vgl_limits.hpp"
#include "vgl_type_traits.hpp"

#include <cstdint>

#if defined(VGL_IS_CONSTANT_EVALUATED)
/// Math functions have constexpr path.
#define VGL_MATH_CONSTEXPR constexpr
#else
/// No constexpr path for math functions. Inline for consistency.
#define VGL_MATH_CONSTEXPR inline
#endif

namespace vgl
{

using std::abs;

namespace detail
{

/// Floor wrapper for double.
///
/// \param val Value.
/// \return Floating point value rounded towards negative infinity.
constexpr double compile_time_floor(double val) noexcept
{
    if(val < 0.0)
    {
        int cast_value = static_cast<int>(val);
        if(static_cast<double>(cast_value) == val)
        {
            return val;
        }
        return static_cast<double>(cast_value - 1);
    }
    return static_cast<double>(static_cast<int>(val));
}

/// Floor wrapper for double.
///
/// \param val Value.
/// \return Floating point value rounded towards positive infinity.
constexpr double compile_time_ceil(double val) noexcept
{
    if(val < 0.0)
    {
        return static_cast<double>(static_cast<int>(val));
    }
    int cast_value = static_cast<int>(val);
    if(static_cast<double>(cast_value) == val)
    {
        return val;
    }
    return static_cast<double>(cast_value + 1);
}

/// Remainder function.
///
/// \param val Value to divide.
/// \param divisor Divisor to the value.
/// \return Remainder of val / divisor.
constexpr double compile_time_remainder(double val, double divisor) noexcept
{
    return val - (compile_time_floor(val / divisor) * divisor);
}

/// Congruence function.
///
/// \param val Value to divide.
/// \param divisor Divisor, must be positive.
/// \return Value in [0, divisor[.
constexpr double compile_time_congr(double val, double divisor) noexcept
{
    VGL_ASSERT(divisor > 0.0);
    if(0.0 <= val)
    {
        return compile_time_remainder(val, divisor);
    }
    double ret = divisor - compile_time_remainder(-val, divisor);
    return (ret < divisor) ? ret : 0.0;
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
constexpr double compile_time_sqrt(double tgt, double curr)
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
constexpr bool almost_equal(float lhs, float rhs) noexcept
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

/// Component-wise almost_equal.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return True if almost equal, false otherwise.
template<typename T> constexpr bool almost_equal(const T& lhs, const T& rhs) noexcept
{
    for(unsigned ii = 0; (ii < T::data_size); ++ii)
    {
        if(!almost_equal(lhs[ii], rhs[ii]))
        {
            return false;
        }
    }
    return true;
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
constexpr float to_fnorm(int8_t op) noexcept
{
    return static_cast<float>(static_cast<int>(op) + 128) * (2.0f / 255.0f) - 1.0f;
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
/// Overflows if ratio is not within [0, 1].
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \param ratio Ratio.
/// \return Mixing result.
constexpr float mix(float lhs, float rhs, float ratio) noexcept
{
    return lhs + (rhs - lhs) * ratio;
}

namespace detail
{

/// Mix two integers.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \param ratio Mixing ratio.
/// \return Mixing result.
template<typename T> constexpr T mix_integers(T lhs, T rhs, float ratio) noexcept
{
    float c1 = static_cast<float>(lhs);
    float c2 = static_cast<float>(rhs);
    float ret = clamp(mix(c1, c2, ratio),
            static_cast<float>(numeric_limits<T>::min()),
            static_cast<float>(numeric_limits<T>::max()));
    return static_cast<T>(iround(ret));
}

}

/// Mix two signed integers.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \param ratio Mixing ratio.
/// \return Mixing result.
constexpr int8_t mix(int8_t lhs, int8_t rhs, float ratio) noexcept
{
    return detail::mix_integers(lhs, rhs, ratio);
}

/// Mix two signed integers.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \param ratio Mixing ratio.
/// \return Mixing result.
constexpr int16_t mix(int16_t lhs, int16_t rhs, float ratio) noexcept
{
    return detail::mix_integers(lhs, rhs, ratio);
}

/// Mix two signed integers.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \param ratio Mixing ratio.
/// \return Mixing result.
constexpr int32_t mix(int32_t lhs, int32_t rhs, float ratio) noexcept
{
    return detail::mix_integers(lhs, rhs, ratio);
}

/// Mix two unsigned integers.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \param ratio Mixing ratio.
/// \return Mixing result.
constexpr uint8_t mix(uint8_t lhs, uint8_t rhs, float ratio) noexcept
{
    return detail::mix_integers(lhs, rhs, ratio);
}

/// Mix two unsigned integers.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \param ratio Mixing ratio.
/// \return Mixing result.
constexpr uint16_t mix(uint16_t lhs, uint16_t rhs, float ratio) noexcept
{
    return detail::mix_integers(lhs, rhs, ratio);
}

/// Mix two unsigned integers.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \param ratio Mixing ratio.
/// \return Mixing result.
constexpr uint32_t mix(uint32_t lhs, uint32_t rhs, float ratio) noexcept
{
    return detail::mix_integers(lhs, rhs, ratio);
}

/// Generic fallback implementation of mix().
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \param ratio Mixing ratio.
/// \return Mixing result.
template<typename T> constexpr typename T::CrtpType mix(const T& lhs, const T& rhs, float ratio) noexcept
{
    return lhs + (rhs - lhs) * ratio;
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
constexpr int remainder(int val, int divisor) noexcept
{
    return val % divisor;
}

/// Congruence function.
///
/// \param val Value to divide.
/// \param divisor Divisor, must be positive.
/// \return Value in [0, divisor[
constexpr int congr(int val, int divisor) noexcept
{
    VGL_ASSERT(divisor > 0);
    if(0 <= val)
    {
        return remainder(val, divisor);
    }
    int ret = divisor - remainder(-val, divisor);
    return (ret < divisor) ? ret : 0;
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
/// \param val Value.
/// \return Floating point value rounded towards negative infinity.
VGL_MATH_CONSTEXPR float floor(float val) noexcept
{
#if defined(VGL_IS_CONSTANT_EVALUATED)
    if(is_constant_evaluated())
    {
        return static_cast<float>(detail::compile_time_floor(static_cast<double>(val)));
    }
#endif
    return dnload_floorf(val);
}

#if !defined(VGL_DISABLE_CEIL)
/// Ceil wrapper.
///
/// \param val Value.
/// \return Floating point value rounded towards positive infinity.
VGL_MATH_CONSTEXPR float ceil(float val) noexcept
{
#if defined(VGL_IS_CONSTANT_EVALUATED)
    if(is_constant_evaluated())
    {
        return static_cast<float>(detail::compile_time_ceil(static_cast<double>(val)));
    }
#endif
    return dnload_ceilf(val);
}
#endif

/// Remainder function.
///
/// \param val Value to divide.
/// \param divisor Divisor to the value.
/// \return Remainder of val / divisor.
VGL_MATH_CONSTEXPR float remainder(float val, float divisor) noexcept
{
    return val - (floor(val / divisor) * divisor);
}

/// Congruence function.
///
/// \param val Value to divide.
/// \param divisor Divisor, must be positive.
/// \return Value in [0, divisor[.
VGL_MATH_CONSTEXPR float congr(float val, float divisor) noexcept
{
    VGL_ASSERT(divisor > 0.0f);
    if(0.0f <= val)
    {
        return remainder(val, divisor);
    }
    float ret = divisor - remainder(-val, divisor);
    return (ret < divisor) ? ret : 0.0f;
}

/// Cosine wrapper.
///
/// \param val Value in radians.
/// \return Cosine of value.
VGL_MATH_CONSTEXPR float cos(float val) noexcept
{
#if defined(VGL_IS_CONSTANT_EVALUATED)
    if(is_constant_evaluated())
    {
        double congr_value = detail::compile_time_congr(static_cast<double>(val), M_PI * 2.0);
        if(congr_value >= M_PI)
        {
            congr_value -= 2.0 * M_PI;
        }
        return static_cast<float>(detail::compile_time_cos(congr_value));
    }
#endif
    return dnload_cosf(val);
}

/// Sine wrapper.
///
/// \param val Value in radians.
/// \return Sine of value.
VGL_MATH_CONSTEXPR float sin(float val) noexcept
{
#if defined(VGL_IS_CONSTANT_EVALUATED)
    if(is_constant_evaluated())
    {
        double congr_value = detail::compile_time_congr(static_cast<double>(val), M_PI * 2.0);
        if(congr_value >= M_PI)
        {
            congr_value -= 2.0 * M_PI;
        }
        return static_cast<float>(detail::compile_time_sin(congr_value));
    }
#endif
    return dnload_sinf(val);
}

/// Square root wrapper.
///
/// \param val Value.
/// \return Square root of value.
VGL_MATH_CONSTEXPR float sqrt(float val) noexcept
{
#if defined(VGL_IS_CONSTANT_EVALUATED)
    if(is_constant_evaluated())
    {
        if((val <= 0.0f) || (val >= std::numeric_limits<float>::infinity()))
        {
            return 0.0f;
        }
        return static_cast<float>(detail::compile_time_sqrt(static_cast<double>(val), static_cast<double>(val / 2.0f)));
    }
#endif
    return dnload_sqrtf(val);
}

/// Power wrapper.
///
/// \param val Value.
/// \param power Power to raise the value to.
/// \return Power of value.
inline float pow(float val, float power)
{
    return dnload_powf(val, power);
}

}

#endif
