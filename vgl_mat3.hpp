#ifndef VGL_MAT3_HPP
#define VGL_MAT3_HPP

#include "vgl_mat2.hpp"
#include "vgl_quat.hpp"
#include "vgl_vec3.hpp"

namespace vgl
{

/// 3x3 matrix class.
class mat3 : public detail::mat<3, mat3, vec3>
{
private:
    /// Parent type.
    using base_type = detail::mat<3, mat3, vec3>;

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
    VGL_MATH_CONSTEXPR static mat3 rotation_zxy(float rx, float ry, float rz) noexcept
    {
        float sx = sin(rx);
        float sy = sin(ry);
        float sz = sin(rz);
        float cx = cos(rx);
        float cy = cos(ry);
        float cz = cos(rz);

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
    VGL_MATH_CONSTEXPR static mat3 rotation(const quat& op) noexcept
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
    /// Rotation applicationm order is z-x-y (i.e. Quake).
    ///
    /// \param pitch Pitch rotation (X axis).
    /// \param yaw Yaw rotation (Y axis).
    /// \param roll Roll rotation (Z axis).
    VGL_MATH_CONSTEXPR static mat3 rotation_euler(float pitch, float yaw, float roll) noexcept
    {
        return rotation_zxy(pitch, yaw, roll);
    }

#if defined(VGL_USE_LD)
public:
    /// Output to stream.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const mat3& rhs)
    {
        return lhs << "[ " <<
            rhs[0u] << " ; " << rhs[3u] << " ; " << rhs[6u] << "\n  " <<
            rhs[1u] << " ; " << rhs[4u] << " ; " << rhs[7u] << "\n  " <<
            rhs[2u] << " ; " << rhs[5u] << " ; " << rhs[8u] << " ]";
    }
#endif
};

/// Transpose a matrix.
///
/// \param op Input matrix.
/// \return Transposed matrix.
constexpr mat3 transpose(const mat3& op) noexcept
{
    return mat3(op[0u], op[3u], op[6u],
            op[1u], op[4u], op[7u],
            op[2u], op[5u], op[8u]);
}

/// Invert a 3x3 matrix.
///
/// Using explanation from Wolfram Mathworld:
/// https://mathworld.wolfram.com/MatrixInverse.html
///
/// \param op Input matrix.
/// \return Inverted matrix.
constexpr mat3 inverse(const mat3& op) noexcept
{
    // Here a, b and c denote top row indices 0, 3 and 6 of the original matrix.
    // Determinants are for the cofactors of those elements.
    float det_a = determinant(mat2(op[4u], op[5u], op[7u], op[8u]));
    float neg_det_b = determinant(mat2(op[7u], op[8u], op[1u], op[2u]));
    float det_c = determinant(mat2(op[1u], op[2u], op[4u], op[5u]));
    float inv_det = 1.0f / ((op[0u] * det_a) + (op[3u] * neg_det_b) + (op[6u] * det_c));

    // Note that the order here is transpose of what would be got for just calculating all cofactors.
    // Some elements calculate the cofactor in the wrong order to get the negative value.
    return mat3(det_a, neg_det_b, det_c,
            determinant(mat2(op[6u], op[8u], op[3u], op[5u])),
            determinant(mat2(op[0u], op[2u], op[6u], op[8u])),
            determinant(mat2(op[3u], op[5u], op[0u], op[2u])),
            determinant(mat2(op[3u], op[4u], op[6u], op[7u])),
            determinant(mat2(op[6u], op[7u], op[0u], op[1u])),
            determinant(mat2(op[0u], op[1u], op[3u], op[4u]))) * inv_det;
}

}

#endif
