#ifndef VGL_VEC2_HPP
#define VGL_VEC2_HPP

#include "vgl_vec.hpp"

namespace vgl
{

/// 2-component floating point vector class.
class vec2 : public detail::vec<2, vec2>
{
private:
    /// Parent type.
    using base_type = detail::vec<2, vec2>;

public:
    /// Default constructor.
    constexpr explicit vec2() = default;

    /// Initialize with single float.
    ///
    /// \param op Single float.
    constexpr explicit vec2(float op) noexcept :
        base_type(op)
    {
    }

    /// Full initialization.
    ///
    /// \param px First element.
    /// \param py Second element.
    constexpr explicit vec2(float px, float py) noexcept :
        base_type(px, py)
    {
    }

public:
    /// Accessor.
    ///
    /// \return Y component.
    constexpr float y() const noexcept
    {
        return base_type::m_data[1u];
    }
};

/// Test if optional values are almost equal.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return True if almost equal, false otherwise.
constexpr bool almost_equal(const optional<vec2>& lhs, const optional<vec2>& rhs) noexcept
{
    if(lhs)
    {
        return rhs ? almost_equal(*lhs, *rhs) : false;
    }
    return rhs ? false : true;
}

/// Dot product between two vector types.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Result value.
constexpr float dot(const vec2& lhs, const vec2& rhs) noexcept
{
    return (lhs[0] * rhs[0]) +
        (lhs[1] * rhs[1]);
}

/// Length of a vector type.
///
/// \param op Vector input.
/// \return Length.
VGL_MATH_CONSTEXPR float length(const vec2& op)
{
    return sqrt(dot(op, op));
}

/// Normalize a vector type.
///
/// \param op Vector to normalize.
/// \return Result vector.
VGL_MATH_CONSTEXPR vec2 normalize(const vec2& op)
{
    float len = length(op);
    return (len <= 0.0f) ? vec2(0.0f) : (op * (1.0f / len));
}

}

#endif
