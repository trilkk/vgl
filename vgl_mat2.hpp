#ifndef VGL_MAT2_HPP
#define VGL_MAT2_HPP

#include "vgl_mat.hpp"
#include "vgl_vec2.hpp"

namespace vgl
{

/// 2x2 matrix class.
class mat2 : public detail::mat<2, mat2, vec2>
{
private:
    /// Parent type.
    using base_type = detail::mat<2, mat2, vec2>;

public:
    /// Empty constructor.
    constexpr explicit mat2() = default;

    /// \brief Full constructor.
    ///
    /// \param op1 First element.
    /// \param op2 Second element.
    /// \param op3 Third element.
    /// \param op4 Fourth element.
    constexpr explicit mat2(float op1, float op2, float op3, float op4) noexcept :
        base_type(op1, op2, op3, op4)
    {
    }

public:
    /// Create an identity matrix.
    ///
    /// \return Result matrix.
    constexpr static mat2 identity() noexcept
    {
        return mat2(1.0f, 0.0f,
                0.0f, 1.0f);
    }

    /// Create a rotation matrix.
    ///
    /// \param op Rotation.
    VGL_MATH_CONSTEXPR static mat2 rotation(float op)
    {
        float sr = sin(op);
        float cr = cos(op);

        return mat2(cr, sr,
                -sr, cr);
    }

#if defined(VGL_USE_LD)
public:
    /// Output to stream.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const mat2& rhs)
    {
        return lhs << "[ " <<
            rhs[0u] << " ; " << rhs[2u] << "\n  " <<
            rhs[1u] << " ; " << rhs[3u] << " ]";
    }
#endif
};

/// Calculates the determinant of a 2x2 matrix.
///
/// \param op Input matrix.
/// \return Determinant of the matrix.
constexpr float determinant(const mat2 &op) noexcept
{
        return (op[0u] * op[3u]) - (op[1u] * op[2u]);
}

/// Transpose a matrix.
///
/// \param op Input matrix.
/// \return Transposed matrix.
constexpr mat2 transpose(const mat2 &op) noexcept
{
    return mat2(op[0u], op[2u],
            op[1u], op[3u]);
}

}

#endif
