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
    constexpr explicit vec3(float op) :
        base_type(op)
    {
    }

    /// Full initialization.
    ///
    /// \param px First element.
    /// \param py Second element.
    /// \param pz Third element.
    constexpr explicit vec3(float px, float py, float pz) :
        base_type(px, py, pz)
    {
    }

public:
    /// Accessor.
    ///
    /// \return Y component.
    constexpr float y()
    {
        return base_type::m_data[1];
    }

    /// Accessor.
    ///
    /// \return Z component.
    constexpr float z()
    {
        return base_type::m_data[2];
    }
};

/// Cross product.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Result vector.
constexpr vec3 cross(const vec3& lhs, const vec3& rhs)
{
    return vec3(lhs[1] * rhs[2] - lhs[2] * rhs[1],
            lhs[2] * rhs[0] - lhs[0] * rhs[2],
            lhs[0] * rhs[1] - lhs[1] * rhs[0]);
}

}

#endif
