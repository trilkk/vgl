#ifndef VGL_IVEC3_HPP
#define VGL_IVEC3_HPP

#include "vgl_vec3.hpp"
#include "vgl_vec4.hpp"

namespace vgl
{

namespace detail
{

/// Convert float between -1 and 1 into an ivec3 element.
///
/// \param op Value.
/// \return Element value.
constexpr int16_t normalized_float_to_ivec3_element(float op) noexcept
{
    return static_cast<int16_t>(iround((op + 1.0f) * (65535.0f / 2.0f)) - 32768);
}

}

/// 3-component unsigned integer vector class.
///
/// Practically, represents normal.
class ivec3
{
private:
    /// Data.
    array<int16_t, 3> m_data;

public:
    /// Default constructor.
    constexpr explicit ivec3() noexcept :
        m_data()
    {
    }

    /// Constructor.
    ///
    /// \param px X component.
    /// \param py Y component.
    /// \param pz Z component.
    constexpr explicit ivec3(int16_t px, int16_t py, int16_t pz) noexcept :
        m_data{px, py, pz}
    {
    }

    /// Constructor.
    ///
    /// \param op Input vector.
    constexpr explicit ivec3(const vec3& op) noexcept :
        m_data{detail::normalized_float_to_ivec3_element(op[0u]),
            detail::normalized_float_to_ivec3_element(op[1u]),
            detail::normalized_float_to_ivec3_element(op[2u])}
    {
    }

public:
    /// Accessor.
    ///
    /// \return Data.
    constexpr int16_t* data() noexcept
    {
        return m_data.data();
    }
    /// Accessor.
    ///
    /// \return Data.
    constexpr const int16_t* data() const noexcept
    {
        return m_data.data();
    }

    /// Accessor.
    ///
    /// \return X component.
    constexpr int16_t x() const noexcept
    {
        return m_data[0u];
    }

    /// Accessor.
    ///
    /// \return Y component.
    constexpr int16_t y() const noexcept
    {
        return m_data[1u];
    }

    /// Accessor.
    ///
    /// \return Z component.
    constexpr int16_t z() const noexcept
    {
        return m_data[2u];
    }

    /// Conversion operator.
    ///
    /// \return Normalized vec3 representation.
    constexpr vec3 toNormVec3() const noexcept
    {
        return vec3(to_fnorm(m_data[0u]),
                to_fnorm(m_data[1u]),
                to_fnorm(m_data[2u]));
    }

public:
    /// Access operator.
    ///
    /// \return Value.
    constexpr int16_t& operator[](unsigned idx)
    {
        return m_data[idx];
    }
    /// Access operator.
    ///
    /// \return Value.
    constexpr const int16_t& operator[](unsigned idx) const
    {
        return m_data[idx];
    }
    /// Access operator.
    ///
    /// \return Value.
    constexpr int16_t& operator[](int idx)
    {
        return m_data[idx];
    }
    /// Access operator.
    ///
    /// \return Value.
    constexpr const int16_t& operator[](int idx) const
    {
        return m_data[idx];
    }

    /// Equals operator.
    ///
    /// \param rhs Right-hand-side operand.
    constexpr bool operator==(const ivec3& rhs) const noexcept
    {
        return (m_data[0u] ==  rhs[0u]) &&
            (m_data[1u] == rhs[1u]) &&
            (m_data[2u] == rhs[2u]);
    }
    /// Not equals operator.
    ///
    /// \param rhs Right-hand-side operand.
    constexpr bool operator!=(const ivec3& rhs) const noexcept
    {
        return !(*this == rhs);
    }

#if defined(USE_LD)
    /// Stream output operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const ivec3& rhs)
    {
        lhs << "[ " << static_cast<int>(rhs[0u]);
        for(unsigned ii = 1; (ii < 3); ++ii)
        {
            lhs << " ; " << static_cast<int>(rhs[ii]);
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
    friend ivec3 mix(const ivec3& lhs, const ivec3& rhs, float ratio) noexcept
    {
        return ivec3(mix(lhs[0], rhs[0], ratio),
                mix(lhs[1], rhs[1], ratio),
                mix(lhs[2], rhs[2], ratio));
    }
};

}

#endif
