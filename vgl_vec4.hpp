#ifndef VGL_VEC4_HPP
#define VGL_VEC4_HPP

#include "vgl/vgl_vec.hpp"

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
    explicit vec4() = default;

    /// Initialize with single float.
    ///
    /// \param op Single float.
    constexpr explicit vec4(float op) :
        base_type(op)
    {
    }

    /// Full initialization.
    ///
    /// \param px First element.
    /// \param py Second element.
    /// \param pz Third element.
    /// \param pw Fourth element.
    constexpr explicit vec4(float px, float py, float pz, float pw) :
        base_type(px, py, pz, pw)
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

    /// Accessor.
    ///
    /// \return W component.
    constexpr float w()
    {
        return base_type::m_data[3];
    }
};

}

#endif
