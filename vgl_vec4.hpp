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

}

#endif
