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
    /// Position data.
    vec3 m_pos;

    /// Rotation data.
    quat m_rot;

    /// Matrix representation of the orientation.
    mat4 m_transform;

public:
    /// Empty constructor.
    explicit BoneState() noexcept = default;

    /// Constructor.
    ///
    /// \param pos Position.
    /// \param rot Rotation.
    explicit BoneState(const vec3& pos, const quat& rot) noexcept :
        m_pos(pos),
        m_rot(rot),
        m_transform(mat3::rotation(rot), pos)
    {
    }

    /// Constructor.
    ///
    /// \param pos Position.
    /// \param rot Rotation.
    /// \param trns Transform.
    explicit BoneState(const vec3& pos, const quat& rot, const mat4& trns) noexcept :
        m_pos(pos),
        m_rot(rot),
        m_transform(trns)
    {
    }

public:
    /// Accessor.
    ///
    /// \return Position.
    const vec3& getPosition() const
    {
        return m_pos;
    }

    /// Accessor.
    ///
    /// \return Rotation.
    const quat& getRotation() const
    {
        return m_rot;
    }

    /// Accessor.
    ///
    /// \return Transformation.
    const mat4& getTransform() const
    {
        return m_transform;
    }

public:
#if defined(USE_LD)
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream &lhs, const BoneState& rhs)
    {
        return lhs << rhs.m_pos << " ; " << rhs.m_rot;
    }
#endif
};

}

#endif
