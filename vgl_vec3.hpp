#ifndef VGL_VEC3_HPP
#define VGL_VEC3_HPP

#include "vgl_vec.hpp"

namespace vgl
{

/// 3-component floating point vector class.
class vec3 : public detail::vec<3, vec3>
{
private:
    /// Parent type.
    using base_type = detail::vec<3, vec3>;

public:
    /// Default constructor.
    constexpr explicit vec3() = default;

    /// Initialize with single float.
    ///
    /// \param op Single float.
    constexpr explicit vec3(float op) noexcept :
        base_type(op)
    {
    }

    /// Full initialization.
    ///
    /// \param px First element.
    /// \param py Second element.
    /// \param pz Third element.
    constexpr explicit vec3(float px, float py, float pz) noexcept :
        base_type(px, py, pz)
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
};

/// Cross product.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Result vector.
constexpr vec3 cross(const vec3& lhs, const vec3& rhs) noexcept
{
    return vec3(lhs[1u] * rhs[2u] - lhs[2u] * rhs[1u],
            lhs[2u] * rhs[0u] - lhs[0u] * rhs[2u],
            lhs[0u] * rhs[1u] - lhs[1u] * rhs[0u]);
}

/// Test if optional values are almost equal.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return True if almost equal, false otherwise.
constexpr bool almost_equal(const optional<vec3>& lhs, const optional<vec3>& rhs) noexcept
{
    if(lhs)
    {
        return rhs ? almost_equal(*lhs, *rhs) : false;
    }
    return rhs ? false : true;}

/// Dot product between two vector types.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Result value.
constexpr float dot(const vec3& lhs, const vec3& rhs) noexcept
{
    return (lhs[0u] * rhs[0u]) +
        (lhs[1u] * rhs[1u]) +
        (lhs[2u] * rhs[2u]);
}

/// Length of a vector type.
///
/// \param op Vector input.
/// \return Length.
VGL_MATH_CONSTEXPR float length(const vec3& op)
{
    return sqrt(dot(op, op));
}

/// Normalize a vector type.
///
/// \param op Vector to normalize.
/// \return Result vector.
VGL_MATH_CONSTEXPR vec3 normalize(const vec3& op)
{
    float len = length(op);
    return (len <= 0.0f) ? vec3(0.0f) : (op * (1.0f / len));
}

}

#endif
