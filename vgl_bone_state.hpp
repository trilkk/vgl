#ifndef VGL_BONE_STATE_HPP
#define VGL_BONE_STATE_HPP

#include "vgl_mat4.hpp"

namespace vgl
{

/// One position of bone.
///
/// Can be neutral position.
class BoneState
{
private:
#if defined(VGL_USE_BONE_STATE_FULL_TRANSFORM)
    /// Matrix representation of the orientation.
    mat4 m_transform;
#else
    /// Position data.
    vec3 m_pos;

    /// Rotation data.
    quat m_rot;
#endif

public:
    /// Empty constructor.
    constexpr explicit BoneState() noexcept = default;

#if defined(VGL_USE_BONE_STATE_FULL_TRANSFORM)
    /// Constructor.
    ///
    /// \param pos Position.
    /// \param rot Rotation.
    /// \param trns Transform.
    constexpr explicit BoneState(const mat4& trns) noexcept :
        m_transform(trns)
    {
    }
#endif

    /// Constructor.
    ///
    /// \param pos Position.
    /// \param rot Rotation.
#if defined(VGL_USE_BONE_STATE_FULL_TRANSFORM)
    VGL_MATH_CONSTEXPR
#else
    constexpr
#endif
    explicit BoneState(const vec3& pos, const quat& rot) noexcept :
#if defined(VGL_USE_BONE_STATE_FULL_TRANSFORM)
        m_transform(mat3::rotation(rot), pos)
#else
        m_pos(pos),
        m_rot(rot)
#endif
    {
    }

public:
    /// Accessor.
    ///
    /// \return Position.
    constexpr
#if defined(VGL_USE_BONE_STATE_FULL_TRANSFORM)
    vec3
#else
    const vec3&
#endif
    getPosition() const
    {
#if defined(VGL_USE_BONE_STATE_FULL_TRANSFORM)
        return m_transform.getTranslation();
#else
        return m_pos;
#endif
    }

#if !defined(VGL_USE_BONE_STATE_FULL_TRANSFORM)
    /// Accessor.
    ///
    /// \return Rotation.
    constexpr const quat& getRotation() const
    {
        return m_rot;
    }
#endif

    /// Accessor.
    ///
    /// \return Transformation.
#if defined(VGL_USE_BONE_STATE_FULL_TRANSFORM)
    constexpr const mat4&
#else
    VGL_MATH_CONSTEXPR mat4
#endif
    getTransform() const
    {
#if defined(VGL_USE_BONE_STATE_FULL_TRANSFORM)
        return m_transform;
#else
        return mat4(mat3::rotation(m_rot), m_pos);
#endif
    }

public:

#if defined(USE_LD)
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream &lhs, const BoneState& rhs)
    {
#if defined(VGL_USE_BONE_STATE_FULL_TRANSFORM)
        return lhs << rhs.m_transform;
#else
        return lhs << rhs.m_pos << " ; " << rhs.m_rot;
#endif
    }
#endif
};

/// Mix for BoneState.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \param ratio Mixing ratio.
/// \return Mix result.
constexpr BoneState mix(const BoneState& lhs, const BoneState& rhs, float ratio)
{
#if defined(VGL_USE_BONE_STATE_FULL_TRANSFORM)
    return BoneState(mix(lhs.getTransform(), rhs.getTransform(), ratio));
#else
    return BoneState(mix(lhs.getPosition(), rhs.getPosition(), ratio), mix(lhs.getRotation(), rhs.getRotation(), ratio));
#endif
}

}

#endif
