#ifndef VGL_MAT4_HPP
#define VGL_MAT4_HPP

#include "vgl_mat3.hpp"
#include "vgl_vec4.hpp"

namespace vgl
{

/// 4x4 matrix class.
class mat4 : public detail::mat<4, mat4, vec4>
{
private:
    /// Parent type.
    using base_type = detail::mat<4, mat4, vec4>;

public:
    /// Empty constructor.
    constexpr explicit mat4() = default;

    /// Constructor.
    ///
    /// \param op1 First value.
    /// \param op2 Second value.
    /// \param op3 Third value.
    /// \param op4 Fourth value.
    /// \param op5 Fifth value.
    /// \param op6 Sixth value.
    /// \param op7 Seventh value.
    /// \param op8 Eighth value.
    /// \param op9 Ninth value.
    /// \param op10 Tenth value.
    /// \param op11 Eleventh value.
    /// \param op12 Twelwth value.
    /// \param op13 Thirteenth value.
    /// \param op14 Foiurteenth value.
    /// \param op15 Fifteenth value.
    /// \param op16 Sixteenth value.
    constexpr explicit mat4(float op1, float op2, float op3, float op4, float op5, float op6, float op7, float op8,
            float op9, float op10, float op11, float op12, float op13, float op14, float op15, float op16) noexcept :
        base_type(op1, op2, op3, op4, op5, op6, op7, op8, op9, op10, op11, op12, op13, op14, op15, op16)
    {
    }

    /// Expansion constructor.
    ///
    /// \param mat 3x3 Matrix.
    /// \param pos Position.
    constexpr explicit mat4(const mat3 &rot, const vec3 &pos = vec3(0.0f, 0.0f, 0.0f)) noexcept :
        base_type(rot[0], rot[1], rot[2], 0.0f, rot[3], rot[4], rot[5], 0.0f, rot[6], rot[7], rot[8], 0.0f, pos[0],
                pos[1], pos[2], 1.0f)
    {
    }

    /// Axis construction.
    ///
    /// \param rt Right axis.
    /// \param up Up axis.
    /// \param fw Forward axis.
    /// \param pos Position.
    constexpr explicit mat4(const vec3& rt, const vec3& up, const vec3& fw,
            const vec3& pos = vec3(0.0f, 0.0f, 0.0f)) noexcept :
        base_type(rt[0], rt[1], rt[2], 0.0f, up[0], up[1], up[2], 0.0f, fw[0], fw[1], fw[2], 0.0f, pos[0], pos[1],
                pos[2], 1.0f)
    {
    }

public:
    /// Add translation component.
    ///
    /// \param tx X translation.
    /// \param ty Y translation.
    /// \param tz Z translation.
    constexpr void addTranslation(float tx, float ty, float tz) noexcept
    {
        m_data[12] += tx;
        m_data[13] += ty;
        m_data[14] += tz;
    }
    /// Add translation component.
    ///
    /// \param op Translation component to add.
    constexpr void addTranslation(const vec3& op) noexcept
    {
        addTranslation(op[0], op[1], op[2]);
    }
    /// Get translation vector.
    ///
    /// \return Translation vector part.
    constexpr vec3 getTranslation() const noexcept
    {
        return vec3(m_data[12], m_data[13], m_data[14]);
    }
    /// Set translation vector.
    ///
    /// \param tx X translation.
    /// \param ty Y translation.
    /// \param tz Z translation.
    constexpr void setTranslation(float tx, float ty, float tz) noexcept
    {
        m_data[12] = tx;
        m_data[13] = ty;
        m_data[14] = tz;
    }
    /// Set translation vector.
    ///
    /// \param op New translation vector.
    constexpr void setTranslation(const vec3 &op) noexcept
    {
        setTranslation(op[0], op[1], op[2]);
    }

    /// Get rotation matrix part as 3x3 matrix.
    ///
    /// \return Rotation matrix part.
    constexpr mat3 getRotation() const noexcept
    {
        return mat3(m_data[0], m_data[1], m_data[2],
                m_data[4], m_data[5], m_data[6],
                m_data[8], m_data[9], m_data[10]);
    }

    /// Get forward component.
    ///
    /// This is the negative of the actual forward vector in the matrix, as z+ disappears into the camera.
    ///
    /// \return Forward vector.
    constexpr vec3 getForward() const noexcept
    {
        return vec3(-m_data[8], -m_data[9], -m_data[10]);
    }  

    /// Scale the 3x3 part of matrix directly.
    ///
    /// \param sx X scale.
    /// \param sy Y scale.
    /// \param sz Z scale.
    constexpr void applyScale(float sx, float sy, float sz) noexcept
    {
        m_data[0] *= sx;
        m_data[1] *= sy;
        m_data[2] *= sz;
        m_data[4] *= sx;
        m_data[5] *= sy;
        m_data[6] *= sz;
        m_data[8] *= sx;
        m_data[9] *= sy;
        m_data[10] *= sz;
    }
    /// Scale wrapper.
    ///
    /// \param op Scaling vector.
    constexpr void applyScale(const vec3 &op) noexcept
    {
        applyScale(op[0], op[1], op[2]);
    }

public:
    /// Vector multiplication operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr vec3 operator*(const vec3& rhs) const noexcept
    {
        mat3 lhs(m_data[0], m_data[1], m_data[2],
                m_data[4], m_data[5], m_data[6],
                m_data[8], m_data[9], m_data[10]);
        return lhs * rhs;
    }

public:
    /// Create an identity matrix.
    ///
    /// \return Result matrix.
    constexpr static mat4 identity() noexcept
    {
        return mat4(1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
    }

    /// Create a 'look at' -matrix.
    ///
    /// \param pos Position.
    /// \param eye Eye focal point.
    /// \param up Up direction.
    /// \return Result matrix.
    static mat4 lookat(const vec3& pos, const vec3& eye, const vec3& up = vec3(0.0f, 1.0f, 0.0f)) noexcept
    {
        vec3 unit_fw = normalize(pos - eye);
        vec3 unit_up = normalize(up);

        if(abs(dot(unit_fw, unit_up)) > 0.999f)
        {
            unit_up = vec3(unit_fw[1], unit_fw[0], unit_fw[2]);
        }

        vec3 unit_rt = normalize(cross(unit_up, unit_fw));
        unit_up = normalize(cross(unit_fw, unit_rt));

        return mat4(unit_rt, unit_up, unit_fw, pos);
    }

    /// Create a projection matrix.
    ///
    /// \param xfov Horizontal field of view in radians.
    /// \param width Viewport width.
    /// \param height Viewport height.
    /// \param znear Near clip plane.
    /// \param zfar Near clip plane.
    /// \return Result matrix.
    static mat4 projection(float xfov, unsigned width, unsigned height, float znear, float zfar) noexcept
    {
        float ff = dnload_tanf(static_cast<float>(M_PI * 0.5) - xfov * 0.5f);
        float n_f = 1.0f / (znear - zfar);

        return mat4(ff, 0.0f, 0.0f, 0.0f,
                0.0f, ff * (static_cast<float>(width) / static_cast<float>(height)), 0.0f, 0.0f,
                0.0f, 0.0f, (znear + zfar) * n_f, -1.0f,
                0.0f, 0.0f, 2.0f * znear * zfar * n_f, 0.0f);
    }

    /// Create a rotation matrix.
    ///
    /// \param rot Input quaternion.
    /// \param pos Translation vector.
    constexpr static mat4 rotation(const quat &rot, const vec3 &pos = vec3(0.0f, 0.0f, 0.0f)) noexcept
    {
        return mat4(mat3::rotation(rot), pos);
    }

    /// Create rotation matrix.
    ///
    /// \param yaw Yaw rotation.
    /// \param pitch Pitch rotation.
    /// \param roll Roll rotation.
    /// \param pos Translation vector.
    /// \return Result matrix.
    static mat4 rotation_euler(float yaw, float pitch, float roll, const vec3& pos = vec3(0.0f, 0.0f, 0.0f)) noexcept
    {
        return mat4(mat3::rotation_euler(yaw, pitch, roll), pos);
    }

    /// Create scaling matrix.
    ///
    /// \param sx X scale.
    /// \param sy Y scale.
    /// \param sz Z scale.
    /// \return Result matrix.
    constexpr static mat4 scale(float sx, float sy, float sz) noexcept
    {
        return mat4(sx, 0.0f, 0.0f, 0.0f,
                0.0f, sy, 0.0f, 0.0f,
                0.0f, 0.0f, sz, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
    }
    /// Create scaling matrix wrapper.
    ///
    /// \param op Scale.
    /// \return Result matrix.
    constexpr static mat4 scale(float op) noexcept
    {
        return scale(op, op, op);
    }

    /// Create full transformation matrix.
    ///
    /// \param yaw Yaw rotation.
    /// \param pitch Pitch rotation.
    /// \param roll Roll rotation.
    /// \param translation Translation vector.
    /// \return Result matrix.
    static mat4 transformation_euler(const vec3 &sca, const vec3 &rot, const vec3 &tra) noexcept
    {
        mat3 rpart = mat3::rotation_euler(rot[0], rot[1], rot[2]);
        return mat4(
                rpart[0] * sca[0], rpart[1] * sca[0], rpart[2] * sca[0], 0.0f,
                rpart[3] * sca[1], rpart[4] * sca[1], rpart[5] * sca[1], 0.0f,
                rpart[6] * sca[2], rpart[7] * sca[2], rpart[8] * sca[2], 0.0f,
                tra[0], tra[1], tra[2], 1.0f);
    }

    /// Create a translation matrix.
    ///
    /// \param tx X translation.
    /// \param ty Y translation.
    /// \param tz Z translation.
    /// \return Translation matrix
    constexpr static mat4 translation(float tx, float ty, float tz) noexcept
    {
        return mat4(1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                tx, ty, tz, 1.0f);
    }
    /// Create a translation matrix.
    ///
    /// \param pos Position vector.
    /// \return Translation matrix
    constexpr static mat4 translation(const vec3 &pos) noexcept
    {
        return translation(pos[0], pos[1], pos[2]);
    }

public:
#if defined(USE_LD)
    /// Output to stream.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const mat4& rhs)
    {
        return lhs << "[ " <<
            rhs[0] << " ;  " << rhs[4] << " ; " << rhs[8] << " ; " << rhs[12] << "\n  " <<
            rhs[1] << " ;  " << rhs[5] << " ; " << rhs[9] << " ; " << rhs[13] << "\n  " <<
            rhs[2] << " ;  " << rhs[6] << " ; " << rhs[10] << " ; " << rhs[14] << "\n  " <<
            rhs[3] << " ;  " << rhs[7] << " ; " << rhs[11] << " ; " << rhs[15] << " ]";
    }
#endif
};

/// Transpose a matrix.
///
/// \param op Input matrix.
/// \return Transposed matrix.
constexpr mat4 transpose(const mat4& op) noexcept
{
    return mat4(op[0], op[4], op[8], op[12],
            op[1], op[5], op[9], op[13],
            op[2], op[6], op[10], op[14],
            op[3], op[7], op[11], op[15]);
}

/// Convert to camera matrix.
///
/// \param op Input matrix.
/// \return View matrix from the given input.
constexpr mat4 viewify(const mat4 &op) noexcept
{
    mat3 rotation = transpose(op.getRotation());
    vec3 translation = rotation * (-op.getTranslation());
    return mat4(rotation, translation);
}

}

#endif
