#ifndef VGL_ANIMATION_FRAME_HPP
#define VGL_ANIMATION_FRAME_HPP

#include "vgl_bone_state.hpp"
#include "vgl_vector.hpp"

namespace vgl
{

/// Animation frame.
class AnimationFrame
{
private:
    /// Timestamp.
    float m_time;

    /// Bone data.
    vector<BoneState> m_bones;

public:
    /// Empty constructor.
    explicit AnimationFrame() noexcept = default;

    /// Constructor.
    ///
    /// \param data Frame data.
    /// \param bone_amount Amount of bone elements.
    /// \param scale Model scale.
    explicit AnimationFrame(const int16_t *data, unsigned bone_amount, float scale)
    {
        readRaw(data, bone_amount, scale);
    }

private:
    /// Initialize from data.
    ///
    /// \param data Frame data.
    /// \param bone_amount Amount of bone elements.
    /// \param scale Model scale.
    void readRaw(const int16_t *data, unsigned frame_amount, float scale)
    {
        m_time = fixed_8_8_to_float(data[0]);

#if defined(USE_LD)
        if((frame_amount % 7) != 0)
        {
            std::ostringstream sstr;
            sstr << "invalid frame amount: " << frame_amount;
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif

        for(unsigned ii = 1; ((frame_amount + 1) > ii); ii += 7)
        {
            vec3 pos(static_cast<float>(data[ii + 0]) * scale,
                    static_cast<float>(data[ii + 1]) * scale,
                    static_cast<float>(data[ii + 2]) * scale);
            quat rot(fixed_4_12_to_float(data[ii + 3]),
                    fixed_4_12_to_float(data[ii + 4]),
                    fixed_4_12_to_float(data[ii + 5]),
                    fixed_4_12_to_float(data[ii + 6]));
            m_bones.emplace_back(pos, rot);
        }
    }

public:
    /// Duplicate from given frame.
    ///
    /// \param op Frame to duplicate.
    void duplicate(const AnimationFrame &op)
    {
        unsigned bone_count = op.getBoneCount();

        m_time = op.getTime();
        m_bones.resize(bone_count);

        for(unsigned ii = 0; (bone_count > ii); ++ii)
        {
            m_bones[ii] = op.getBoneState(ii);
        }
    }

    /// Get number of bones.
    ///
    /// \return Number of bones.
    unsigned getBoneCount() const
    {
        return m_bones.size();
    }

    /// Accessor.
    ///
    /// \param index Index to access.
    /// \return Bone state at index.
    BoneState& getBoneState(unsigned idx)
    {
        return m_bones[idx];
    }
    /// Const accessor.
    ///
    /// \param index Index to access.
    /// \return Bone state at index.
    const BoneState& getBoneState(unsigned idx) const
    {
        return m_bones[idx];
    }

    /// Accessor.
    ///
    /// \return Timestamp.
    float getTime() const
    {
        return m_time;
    }

    /// Interpolate animation frame from two other frames.
    ///
    /// \param src Source frame (earlier).
    /// \param dst Destination frame (later).
    /// \param current_time Animation time.
    void interpolateFrom(const AnimationFrame &lhs, const AnimationFrame &rhs, float current_time)
    {
        unsigned bone_count = lhs.getBoneCount();
#if defined(USE_LD)
        if(rhs.getBoneCount() != bone_count)
        {
            std::ostringstream sstr;
            sstr << "cannot interpolate between frames of size " << bone_count << " and " << rhs.getBoneCount();
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif

        m_time = current_time;
        m_bones.resize(bone_count);

        for(unsigned ii = 0; (bone_count > ii); ++ii)
        {
            const BoneState &ll = lhs.getBoneState(ii);
            const BoneState &rr = rhs.getBoneState(ii);
            float ltime = lhs.getTime();
            float rtime = rhs.getTime();
            float mix_time = (current_time - ltime) / (rtime - ltime);

            // Linear interpolation is ok for position.
            vec3 mix_pos = mix(ll.getPosition(), rr.getPosition(), mix_time);

            // This is incorrect, since linear interpolation between quaternions is not correct.
            const quat& lq = ll.getRotation();
            const quat& rq = rr.getRotation();
            quat mix_rot = mix(ll.getRotation(), rr.getRotation(), mix_time);

            // This should be calculated from position and rotation, but works for now.
            mat4 mix_trns = mix(ll.getTransform(), rr.getTransform(), mix_time);

            m_bones[ii] = BoneState(mix_pos, mix_rot, mix_trns);
        }
    }

public:
#if defined(USE_LD)
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream &lhs, const AnimationFrame& rhs)
    {
        return lhs << "AnimationFrame(" << rhs.m_time << "): " << rhs.m_bones.size() << " bones";
    }
#endif
};

}

#endif
