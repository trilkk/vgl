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
    explicit vec2() = default;

    /// Initialize with single float.
    ///
    /// \param op Single float.
    constexpr explicit vec2(float op) :
        base_type(op)
    {
    }

    /// Full initialization.
    ///
    /// \param px First element.
    /// \param py Second element.
    constexpr explicit vec2(float px, float py) :
        base_type(px, py)
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
};

}

#endif
