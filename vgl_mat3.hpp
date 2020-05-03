#ifndef VGL_MAT3_HPP
#define VGL_MAT3_HPP

#include "vgl_mat.hpp"
#include "vgl_quat.hpp"

namespace vgl
{

/// 3x3 matrix class.
class mat3 : public detail::mat<3, mat3>
{
private:
    /// Parent type.
    using base_type = detail::mat<3, mat3>;

public:
    /// Empty constructor.
    constexpr explicit mat3() = default;

    /// Constructor.
    ///
    /// \param op1 First element.
    /// \param op2 Second element.
    /// \param op3 Third element.
    /// \param op4 Fourth element.
    /// \param op5 Fifth element.
    /// \param op6 Sixth element.
    /// \param op7 Seventh element.
    /// \param op8 Eight element.
    /// \param op9 Ninth element.
    constexpr explicit mat3(float op1, float op2, float op3, float op4, float op5, float op6, float op7, float op8,
            float op9) noexcept :
        base_type(op1, op2, op3, op4, op5, op6, op7, op8, op9)
    {
    }

private:
    /// Create zxy rotation matrix in this.
    ///
    /// \param rx Rotation X (radians).
    /// \param ry Rotation Y (radians).
    /// \param rz Rotation Z (radians).
    /// \return Rotation matrix.
    static mat3 rotation_zxy(float rx, float ry, float rz) noexcept
    {
        float sx = dnload_sinf(rx);
        float sy = dnload_sinf(ry);
        float sz = dnload_sinf(rz);
        float cx = dnload_cosf(rx);
        float cy = dnload_cosf(ry);
        float cz = dnload_cosf(rz);

        return mat3(sx * sy * sz + cy * cz,
                sz * cx,
                sx * sz * cy - sy * cz,
                sx * sy * cz - sz * cy,
                cx * cz,
                sx * cy * cz + sy * sz,
                sy * cx,
                -sx,
                cx * cy);
    }

public:
    /// Multiply vector.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr vec3 operator*(const vec3& rhs) const noexcept
    {
        return vec3(m_data[0] * rhs[0] + m_data[3] * rhs[1] + m_data[6] * rhs[2],
                m_data[1] * rhs[0] + m_data[4] * rhs[1] + m_data[7] * rhs[2],
                m_data[2] * rhs[0] + m_data[5] * rhs[1] + m_data[8] * rhs[2]);
    }

public:
#if defined(USE_LD)
    /// Output to stream.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const mat3& rhs)
    {
        return ostr << "[ " <<
            m_data[0] << " ;  " << m_data[3] << " ; " << m_data[6] << "\n  " <<
            m_data[1] << " ; " << m_data[4] << " ; " << m_data[7] << "\n  " <<
            m_data[2] << " ; " << m_data[5] << " ; " << m_data[8] <<
            " ]";
    }
#endif

public:
    /// Create an identity matrix.
    ///
    /// \return Result matrix.
    constexpr static mat3 identity() noexcept
    {
        return mat3(1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f);
    }

    /// Create a rotation matrix.
    ///
    /// \param op Input quaternion.
    /// \return Rotation matrix.
    constexpr static mat3 rotation(const quat& op) noexcept
    {
        float mag = op.magnitude();
        float w = op[0] / mag;
        float x = op[1] / mag;
        float y = op[2] / mag;
        float z = op[3] / mag;
        float wx2 = 2.0f * w * x;
        float wy2 = 2.0f * w * y;
        float wz2 = 2.0f * w * z;
        float xy2 = 2.0f * x * y;
        float xz2 = 2.0f * x * z;
        float yz2 = 2.0f * y * z;
        float xx2 = 2.0f * x * x;
        float yy2 = 2.0f * y * y;
        float zz2 = 2.0f * z * z;

        // Using unit quaternion to matrix algo by J.M.P. van Wavaren at Id Software:
        // http://fabiensanglard.net/doom3_documentation/37726-293748.pdf
        return mat3(1.0f - yy2 - zz2,
                xy2 + wz2,
                xz2 - wy2,
                xy2 - wz2,
                1.0f - xx2 - zz2,
                yz2 + wx2,
                xz2 + wy2,
                yz2 - wx2,
                1.0f - xx2 - yy2);
    }

    /// Create an euler rotation matrix.
    ///
    /// \param yaw Yaw rotation.
    /// \param pitch Pitch rotation.
    /// \param roll Roll rotation.
    static mat3 rotation_euler(float yaw, float pitch, float roll) noexcept
    {
        return rotation_zxy(pitch, yaw, roll);
    }

public:
    /// Transpose a matrix.
    ///
    /// \param op Input matrix.
    /// \return Transposed matrix.
    constexpr friend mat3 transpose(const CrtpType& op) noexcept
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < N); ii += A + 1)
        {
            ret[ii] = op[ii]; 
        }
        for(unsigned ii = 1; (ii < A); ++ii)
        {
            ret[ii]
        }
        return ret;
    }
};

}

#endif
