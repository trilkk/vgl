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
    /// Position data (neutral).
    vec3 m_pos;

    /// Quaternion data (neutral).
    quat m_rot;

public:
    /// Empty constructor.
    BoneState()
    {
    }

    /// Constructor.
    ///
    /// \param pos Position.
    /// \param rot Rotation.
    BoneState(const vec3& pos, const quat& rot) :
        m_pos(pos),
        m_rot(rot)
    {
    }

public:
    /// Accessor.
    ///
    /// \return Position.
    vec3& getPosition()
    {
        return m_pos;
    }
    /// Const accessor.
    ///
    /// \return Position.
    const vec3& getPosition() const
    {
        return m_pos;
    }

    /// Accessor.
    ///
    /// \return Rotation.
    quat& getRotation()
    {
        return m_rot;
    }
    /// Const accessor.
    ///
    /// \return Rotation.
    const quat& getRotation() const
    {
        return m_rot;
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
