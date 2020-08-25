#ifndef VGL_ANIMATION_FRAME_HPP
#define VGL_ANIMATION_FRAME_HPP

#include "vgl_bone_state.hpp"
#include "vgl_vector.hpp"
#include "vgl_unique_ptr.hpp"

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

        //std::cout << m_time << std::endl;

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
            vec3 pos(static_cast<float>(data[ii + 0]),
                    static_cast<float>(data[ii + 1]),
                    static_cast<float>(data[ii + 2]));
            quat rot(fixed_4_12_to_float(data[ii + 3]),
                    fixed_4_12_to_float(data[ii + 4]),
                    fixed_4_12_to_float(data[ii + 5]),
                    fixed_4_12_to_float(data[ii + 6]));

            //std::cout << pos << " ; " << rot << std::endl;

            m_bones.emplace_back(pos * scale, rot);
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

        //std::cout << "resizing bones or not? " << m_bones.size() << " vs " << bone_count << std::endl;

        m_time = current_time;
        m_bones.resize(bone_count);

        for(unsigned ii = 0; (bone_count > ii); ++ii)
        {
            const BoneState &ll = lhs.getBoneState(ii);
            const BoneState &rr = rhs.getBoneState(ii);
            float ltime = lhs.getTime();
            float rtime = rhs.getTime();
            float mix_time = (current_time - ltime) / (rtime - ltime);
            vec3 mix_pos = mix(ll.getPosition(), rr.getPosition(), mix_time);
            quat mix_rot = mix(ll.getRotation(), rr.getRotation(), mix_time);

            //std::cout << "mix time: " << mix_time << ": " << mix_pos << " ; " << mix_rot << std::endl;

            m_bones[ii] = BoneState(mix_pos, mix_rot);
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

/// Smart pointer type.
typedef unique_ptr<AnimationFrame> AnimationFrameUptr;

}

#endif
