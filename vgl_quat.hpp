#ifndef VGL_QUAT_HPP
#define VGL_QUAT_HPP

#include "vgl_array.hpp"

namespace vgl
{

/// Quaternion.
class quat
{
public:
    /// Publicly visible data size.
    static const unsigned data_size = 4;

    /// Publicly visible CRTP type.
    using CrtpType = quat;

private:
    /// Quaternion data.
    array<float, data_size> m_data;

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
    VGL_MATH_CONSTEXPR float magnitude() const
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
        return quat(-(*this)[0u],
                -(*this)[1u],
                -(*this)[2u],
                -(*this)[3u]);
    }

    /// Addition operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result quaternion.
    constexpr quat operator+(const quat &rhs) const noexcept
    {
        return quat((*this)[0u] + rhs[0u],
                (*this)[1u] + rhs[1u],
                (*this)[2u] + rhs[2u],
                (*this)[3u] + rhs[3u]);
    }
    /// Addition into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This quaternion.
    constexpr quat& operator+=(const quat &rhs) noexcept
    {
        return (*this) = (*this) + rhs;
    }

    /// Subtraction operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result quaternion.
    constexpr quat operator-(const quat &rhs) const noexcept
    {
        return quat((*this)[0u] - rhs[0u],
                (*this)[1u] - rhs[1u],
                (*this)[2u] - rhs[2u],
                (*this)[3u] - rhs[3u]);
    }
    /// Subtraction into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This quaternion.
    constexpr quat& operator-=(const quat &rhs) noexcept
    {
        return (*this) = (*this) - rhs;
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
        return quat(((*this)[0u] * rhs[0u]) - ((*this)[1u] * rhs[1u]) - ((*this)[2u] * rhs[2u]) - ((*this)[3u] * rhs[3u]),
                ((*this)[0u] * rhs[1u]) + ((*this)[1u] * rhs[0u]) + ((*this)[2u] * rhs[3u]) - ((*this)[3u] * rhs[2u]),
                ((*this)[0u] * rhs[2u]) - ((*this)[1u] * rhs[3u]) + ((*this)[2u] * rhs[0u]) + ((*this)[3u] * rhs[1u]),
                ((*this)[0u] * rhs[3u]) + ((*this)[1u] * rhs[2u]) - ((*this)[2u] * rhs[1u]) + ((*this)[3u] * rhs[0u]));
    }
    /// Multiplication operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result quaternion.
    constexpr quat operator*(float rhs) const noexcept
    {
        return quat((*this)[0u] * rhs,
                (*this)[1u] * rhs,
                (*this)[2u] * rhs,
                (*this)[3u] * rhs);
    }
    /// Multiplication into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This quaternion.
    constexpr quat& operator*=(const quat &rhs) noexcept
    {
        return (*this) = (*this) * rhs;
    }
    /// Multiplication into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This quaternion.
    constexpr quat& operator*=(float rhs) noexcept
    {
        return (*this) = (*this) * rhs;
    }

    /// Division operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result quaternion.
    constexpr quat operator/(float rhs) const noexcept
    {
        return quat((*this)[0u] / rhs,
                (*this)[1u] / rhs,
                (*this)[2u] / rhs,
                (*this)[3u] / rhs);
    }
    /// Division into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This quaternion.
    constexpr quat& operator/=(float rhs) noexcept
    {
        return (*this) = (*this) / rhs;
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

#if defined(VGL_USE_LD)
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
};

}

#endif
