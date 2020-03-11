#ifndef VGL_UVEC4_HPP
#define VGL_UVEC4_HPP

#include "vgl_vec.hpp"

namespace vgl
{

/// 4-component unsigned integer vector class.
///
/// Practically, represents color.
class uvec4
{
private:
    /// Data.
    array<uint8_t, 4> m_data;

public:
    /// Default constructor.
    explicit uvec4()
    {
    }

    /// Constructor.
    ///
    /// \param cr Red component.
    /// \param cg Green component.
    /// \param cb Blue component.
    /// \param ca Alpha component.
    constexpr explicit uvec4(uint8_t cr, uint8_t cg, uint8_t cb, uint8_t ca) :
        m_data{cr, cg, cb, ca}
    {
    }

    /// Constructor.
    ///
    /// \param op Input vector.
    constexpr explicit uvec4(const vec3 &op) :
        m_data{static_cast<uint8_t>(iround(op[0u] * 255.0f)),
            static_cast<uint8_t>(iround(op[1u] * 255.0f)),
            static_cast<uint8_t>(iround(op[2u] * 255.0f)),
            static_cast<uint8_t>(255)}
    {
    }

public:
    /// Accessor.
    ///
    /// \return Data.
    constexpr uint8_t* data()
    {
        return m_data.data();
    }
    /// Accessor.
    ///
    /// \return Data.
    constexpr const uint8_t* data() const
    {
        return m_data.data();
    }

public:
    /// Access operator.
    ///
    /// \return Value.
    constexpr uint8_t& operator[](unsigned idx)
    {
        return m_data[idx];
    }
    /// Access operator.
    ///
    /// \return Value.
    constexpr const uint8_t& operator[](unsigned idx) const
    {
        return m_data[idx];
    }
    /// Access operator.
    ///
    /// \return Value.
    constexpr uint8_t& operator[](int idx)
    {
        return m_data[idx];
    }
    /// Access operator.
    ///
    /// \return Value.
    constexpr const uint8_t& operator[](int idx) const
    {
        return m_data[idx];
    }

    /// Equals operator.
    ///
    /// \param rhs Right-hand-side operand.
    constexpr bool operator==(const uvec4& rhs) const
    {
        return (m_data[0u] ==  rhs[0u]) &&
            (m_data[1u] == rhs[1u]) &&
            (m_data[2u] == rhs[2u]) &&
            (m_data[3u] == rhs[3u]);
    }
    /// Not equals operator.
    ///
    /// \param rhs Right-hand-side operand.
    constexpr bool operator!=(const uvec4& rhs) const
    {
        return !(*this == rhs);
    }

#if defined(USE_LD)
    /// Stream output operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const uvec4& rhs)
    {
        lhs << "[ " << static_cast<unsigned>(rhs[0u]);
        for(unsigned ii = 1; (ii < 4); ++ii)
        {
            lhs << " ; " << static_cast<unsigned>(rhs[ii]);
        }
        return lhs << " ]";
    }
#endif

public:
    /// Mix two vectors.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \param ratio Mixing ratio.
    /// \return Result color.
    friend uvec4 mix(const uvec4& lhs, const uvec4& rhs, float ratio)
    {
        return uvec4(mix(lhs[0], rhs[0], ratio),
                mix(lhs[1], rhs[1], ratio),
                mix(lhs[2], rhs[2], ratio),
                mix(lhs[3], rhs[3], ratio));
    }

    /// Modulate two vectors.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result color.
    friend uvec4 modulate(const uvec4& lhs, const uvec4& rhs)
    {
        return uvec4(modulate(lhs[0], rhs[0]),
                modulate(lhs[1], rhs[1]),
                modulate(lhs[2], rhs[2]),
                modulate(lhs[3], rhs[3]));
    }
};

}

#endif
