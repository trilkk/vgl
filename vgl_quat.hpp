#ifndef VGL_QUAT_HPP
#define VGL_QUAT_HPP

#include "vgl_array.hpp"

namespace vgl
{

/// Quaternion.
class quat
{
private:
    /// Quaternion data.
    array<float, 4> m_data;

public:
    /// Empty constructor.
    constexpr explicit quat() noexcept :
        m_data()
    {
    }

    /// Constructor.
    ///
    /// \param op1 First value.
    /// \param op2 Second value.
    /// \param op3 Third value.
    /// \param op4 Fourth value.
    constexpr explicit quat(float op1, float op2, float op3, float op4) noexcept :
        m_data{op1, op2, op3, op4}
    {
    }

public:
    /// Calculate the magnitude of the quaternion.
    ///
    /// Analogous to 4-component vector length.
    ///
    /// \return Magnitude.
    constexpr float magnitude() const
    {
        return sqrt(m_data[0u] * m_data[0u] +
                m_data[1u] * m_data[1u] +
                m_data[2u] * m_data[2u] +
                m_data[3u] * m_data[3u]);
    }

public:
    /// Access operator.
    ///
    /// \param idx Index.
    /// \return Value.
    constexpr float& operator[](unsigned idx)
    {
        return m_data[idx];
    }
    /// Const access operator.
    ///
    /// \param idx Index.
    /// \return Value.
    constexpr const float& operator[](unsigned idx) const
    {
        return m_data[idx];
    }
    /// Access operator.
    ///
    /// \param idx Index.
    /// \return Value.
    constexpr float& operator[](int idx)
    {
        return m_data[idx];
    }
    /// Const access operator.
    ///
    /// \param idx Index.
    /// \return Value.
    constexpr const float& operator[](int idx) const
    {
        return m_data[idx];
    }

    /// Unary minus operator.
    ///
    /// \return Result quaternion.
    constexpr quat operator-() const noexcept
    {
        return quat(-m_data[0u],
                -m_data[1u],
                -m_data[2u],
                -m_data[3u]);
    }

    /// Addition operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result quaternion.
    constexpr quat operator+(const quat &rhs) const noexcept
    {
        return quat(m_data[0u] + rhs[0u],
                m_data[1u] + rhs[1u],
                m_data[2u] + rhs[2u],
                m_data[3u] + rhs[3u]);
    }
    /// Addition into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This quaternion.
    constexpr quat& operator+=(const quat &rhs) noexcept
    {
        *this = *this + rhs;
        return *this;
    }

    /// Subtraction operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result quaternion.
    constexpr quat operator-(const quat &rhs) const noexcept
    {
        return quat(m_data[0u] - rhs[0u],
                m_data[1u] - rhs[1u],
                m_data[2u] - rhs[2u],
                m_data[3u] - rhs[3u]);
    }
    /// Subtraction into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This quaternion.
    constexpr quat& operator-=(const quat &rhs) noexcept
    {
        *this = *this - rhs;
        return *this;
    }

    /// Multiplication operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result quaternion.
    constexpr quat operator*(const quat &rhs) const noexcept
    {
        // (Q1 * Q2).w = (w1w2 - x1x2 - y1y2 - z1z2)
        // (Q1 * Q2).x = (w1x2 + x1w2 + y1z2 - z1y2)
        // (Q1 * Q2).y = (w1y2 - x1z2 + y1w2 + z1x2)
        // (Q1 * Q2).z = (w1z2 + x1y2 - y1x2 + z1w2)
        return quat((m_data[0u] * rhs[0u]) - (m_data[1u] * rhs[1u]) - (m_data[2u] * rhs[2u]) - (m_data[3u] * rhs[3u]),
                (m_data[0u] * rhs[1u]) + (m_data[1u] * rhs[0u]) + (m_data[2u] * rhs[3u]) - (m_data[3u] * rhs[2u]),
                (m_data[0u] * rhs[2u]) - (m_data[1u] * rhs[3u]) + (m_data[2u] * rhs[0u]) + (m_data[3u] * rhs[1u]),
                (m_data[0u] * rhs[3u]) + (m_data[1u] * rhs[2u]) - (m_data[2u] * rhs[1u]) + (m_data[3u] * rhs[0u]));
    }
    /// Multiplication operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result quaternion.
    constexpr quat operator*(float rhs) const noexcept
    {
        return quat(m_data[0] * rhs,
                m_data[1] * rhs,
                m_data[2] * rhs,
                m_data[3] * rhs);
    }
    /// Multiplication into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This quaternion.
    constexpr quat& operator*=(const quat &rhs) noexcept
    {
        *this = *this * rhs;
        return *this;
    }
    /// Multiplication into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This quaternion.
    constexpr quat& operator*=(float rhs) noexcept
    {
        *this = *this * rhs;
        return *this;
    }

    /// Division operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result quaternion.
    constexpr quat operator/(float rhs) const noexcept
    {
        return quat(m_data[0u] / rhs,
                m_data[1u] / rhs,
                m_data[2u] / rhs,
                m_data[3u] / rhs);
    }
    /// Division into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This quaternion.
    constexpr quat& operator/=(float rhs) noexcept
    {
        *this = *this / rhs;
        return *this;
    }

public:
    /// Multiplication operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result quaternion.
    constexpr friend quat operator*(float lhs, const quat& rhs) noexcept
    {
        return rhs * lhs;
    }

#if defined(USE_LD)
    /// Output to stream.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const quat& rhs)
    {
        return lhs << "[ " << rhs[0u] << " ; " << rhs[1u] << " ; " << rhs[2u] << " ; " << rhs[3u] << " ]";
    }
#endif

public:
    /// Mix two quaternions.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \param ratio Mixing ratio.
    constexpr friend quat mix(const quat &lhs, const quat &rhs, float ratio) noexcept
    {
        return lhs + (rhs - lhs) * ratio;
    }
};

}

#endif
