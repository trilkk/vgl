#ifndef VGL_VEC4_HPP
#define VGL_VEC4_HPP

#include "vgl_vec.hpp"

namespace vgl
{

/// 4-component floating point vector class.
class vec4 : public detail::vec<4, vec4>
{
private:
    /// Parent type.
    using base_type = detail::vec<4, vec4>;

public:
    /// Default constructor.
    constexpr explicit vec4() = default;

    /// Initialize with single float.
    ///
    /// \param op Single float.
    constexpr explicit vec4(float op) noexcept :
        base_type(op)
    {
    }

    /// Full initialization.
    ///
    /// \param px First element.
    /// \param py Second element.
    /// \param pz Third element.
    /// \param pw Fourth element.
    constexpr explicit vec4(float px, float py, float pz, float pw) noexcept :
        base_type(px, py, pz, pw)
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

    /// Accessor.
    ///
    /// \return Z component.
    constexpr float z() const noexcept
    {
        return base_type::m_data[2u];
    }

    /// Accessor.
    ///
    /// \return W component.
    constexpr float w() const noexcept
    {
        return base_type::m_data[3u];
    }
};

/// Test if optional values are almost equal.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return True if almost equal, false otherwise.
constexpr bool almost_equal(const optional<vec4>& lhs, const optional<vec4>& rhs) noexcept
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
constexpr float dot(const vec4& lhs, const vec4& rhs) noexcept
{
    return (lhs[0] * rhs[0]) +
        (lhs[1] * rhs[1]) +
        (lhs[2] * rhs[2]) +
        (lhs[3] * rhs[3]);
}

/// Length of a vector type.
///
/// \param op Vector input.
/// \return Length.
VGL_MATH_CONSTEXPR float length(const vec4& op)
{
    return sqrt(dot(op, op));
}

/// Normalize a vector type.
///
/// \param op Vector to normalize.
/// \return Result vector.
VGL_MATH_CONSTEXPR vec4 normalize(const vec4& op)
{
    float len = length(op);
    return (len <= 0.0f) ? vec4(0.0f) : (op * (1.0f / len));
}

}

#endif
