#ifndef VGL_ANIMATION_STATE_HPP
#define VGL_ANIMATION_STATE_HPP

#include "vgl_animation.hpp"
#include "vgl_mat4.hpp"

namespace vgl
{

/// Represents animation state, blended from distinct animations.
class AnimationState
{
private:
    /// Mixing frame for temporary calculation.
    AnimationFrame m_mix_frame;

    /// Matrix data (final).
    vector<mat4> m_matrices;

public:
    /// Empty constructor.
    explicit AnimationState() noexcept = default;

    /// Initializing constructor.
    ///
    /// \param anima Animation to interpolate.
    /// \param time Current time.
    explicit AnimationState(const Animation &anim)
    {
        identityFrame(anim);
    }

    /// Initializing constructor.
    ///
    /// \param anima Animation to interpolate.
    /// \param time Current time.
    explicit AnimationState(const Animation &anim, float current_time)
    {
        interpolateFrom(anim, current_time);
    }

public:
    /// Accessor.
    ///
    /// \return Matrix data.
    const mat4* getBoneData() const
    {
        return m_matrices.data();
    }

    /// Accessor.
    ///
    /// \return Matrix count.
    unsigned getBoneCount() const
    {
        return m_matrices.size();
    }

    /// Create identiry frame.
    ///
    /// \param op Number of identity matrices.
    void identityFrame(unsigned op)
    {
        m_matrices.resize(op);
        for(unsigned ii = 0; (ii < op); ++ii)
        {
            m_matrices[ii] = mat4::identity();
        }
    }
    /// Create identiry frame.
    ///
    /// \param op Animation to extract number of identity matrices from.
    void identityFrame(const Animation& op)
    {
        identityFrame(op.getBoneCount());
    }

    /// Interpolate animation data from two frames.
    ///
    /// Animations loop after the end time.
    ///
    /// \param anima Animation to interpolate.
    /// \param time Current time.
    void interpolateFrom(const Animation &anim, float current_time)
    {
        unsigned frame_count = anim.getFrameCount();
#if defined(USE_LD)
        if(0 >= frame_count)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("can't animate animation without frames"));
        }
#endif

        if(1 == frame_count)
        {
            m_mix_frame.duplicate(anim.getFrame(0));
        }
        else
        {
            float end_time = anim.getFrame(frame_count - 1).getTime();
            float bounded_time = congr(current_time, end_time);
            unsigned ii = 1;

            for(;;)
            {
                const AnimationFrame &ll = anim.getFrame(ii - 1);
                const AnimationFrame &rr = anim.getFrame(ii);
                float rtime = rr.getTime();

#if defined(USE_LD)
                if(ll.getTime() >= rtime)
                {
                    std::ostringstream sstr;
                    sstr << "animation index " << ii << " has time " << ll.getTime() <<
                        " which is not smaller than frame " << (ii + 1) << " with time " << rr.getTime();
                    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
                }
#endif

                if(rtime >= bounded_time)
                {
                    m_mix_frame.interpolateFrom(ll, rr, bounded_time);
                    break;
                }

                ++ii;
#if defined(USE_LD)
                if(ii >= frame_count)
                {
                    std::ostringstream sstr;
                    sstr << "could not find frames to interpolate for time " << bounded_time;
                    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
                }
#endif
            }
        }

        unsigned bone_count = m_mix_frame.getBoneCount();
        m_matrices.resize(bone_count);

#if 0
        vector<mat3> rotations(bone_count);
        for(unsigned ii = 0; (bone_count > ii); ++ii)
        {
            BoneState &st = m_mix_frame.getBoneState(ii);
            quat rot = st.getRotation();
            rotations[ii] = mat3::rotation(rot);
        }

        for(unsigned ii = 0; (bone_count > ii); ++ii)
        {
            const BoneState &st = m_mix_frame.getBoneState(ii);
            mat4 trns_positive(rotations[ii], st.getPosition());
#if 0
            // Create negative translation using the bone's original position.
            // This approach is unnecessary if the bone transform has been baked into the export.
            const Bone &bn = arm.getBone(ii);
            mat4 trns_negative = mat4::translation(-bn.getPosition());
            m_matrices[ii] = trns_positive * trns_negative;
#else
            m_matrices[ii] = trns_positive;
#endif
        }
#else
        // Use the matrix already calculated into bone state.
        for(unsigned ii = 0; (bone_count > ii); ++ii)
        {
            m_matrices[ii] = m_mix_frame.getBoneState(ii).getTransform();
        }
#endif
    }
};

}

#endif
