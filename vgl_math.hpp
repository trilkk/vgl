#ifndef VGL_MATH_HPP
#define VGL_MATH_HPP

#include "vgl_algorithm.hpp"
#include "vgl_assert.hpp"

#include <cmath>

#if defined(__GNUC__) && !defined(__clang__)
#define VGL_MATH_CONSTEXPR constexpr
#else
#define VGL_MATH_CONSTEXPR
#endif

namespace vgl
{

using std::abs;

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

/// Rounds floating point value towards negative infinity.
///
/// \param val Value to round.
/// \return Rounded integer value.
constexpr int ifloor(float val) noexcept
{
    if(val < 0.0f)
    {
        return static_cast<int>(val) - 1;
    }
    return static_cast<int>(val);
}
/// Rounds floating point value towards negative infinity.
///
/// \param val Value to round.
/// \return Rounded integer value.
constexpr float floor(float val) noexcept
{
    return static_cast<float>(ifloor(val));
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
    return val - (floor(val / divisor) * divisor);
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
        return val % divisor;
    }
    int ret = divisor - ((-val) % divisor);
    return (ret < divisor) ? ret : 0;
}

/// Congruence function.
///
/// \param val Value to divide.
/// \param divisor Divisor, must be positive.
/// \return Value in [0, divisor[.
constexpr float congr(float val, float divisor) noexcept
{
    VGL_ASSERT(divisor > 0.0f);
    if(0.0f <= val)
    {
        return remainder(val, divisor);
    }
    float ret = divisor - remainder(-val, divisor);
    return (ret < divisor) ? ret : 0.0f;
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

/// Cosine wrapper.
///
/// \param op Value in radians.
/// \return Cosine of value.
float cos(float op) noexcept
{
    return dnload_cosf(op);
}

/// Sine wrapper.
///
/// \param op Value in radians.
/// \return Sine of value.
float sin(float op) noexcept
{
    return dnload_sinf(op);
}

/// Square root wrapper.
///
/// \param op Value.
/// \return Square root of value.
VGL_MATH_CONSTEXPR float sqrt(float op) noexcept
{
    return dnload_sqrtf(op);
}

/// Power wrapper.
///
/// \param value Value.
/// \param power Power to raise the value to.
/// \return Power of value.
float pow(float value, float power)
{
    return dnload_powf(value, power);
}

}

#endif
