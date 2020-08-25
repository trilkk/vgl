#ifndef VGL_ANIMATION_STATE_HPP
#define VGL_ANIMATION_STATE_HPP

#include "vgl_animation.hpp"
#include "vgl_armature.hpp"

namespace vgl
{

/// Represents animation state, blended from distinct animations.
class AnimationState
{
private:
    /// Mixing frame for temporary calculation.
    AnimationFrame m_mix_frame;

    /// Matrix data (rotation-only).
    vector<mat3> m_rotations;

    /// Matrix data (final).
    vector<mat4> m_matrices;

public:
    /// Empty constructor.
    explicit AnimationState() noexcept = default;

    /// Initializing constructor.
    ///
    /// \param arm Armature base.
    /// \param anima Animation to interpolate.
    /// \param time Current time.
    explicit AnimationState(const Armature &arm, const Animation &anim, float current_time)
    {
        interpolateFrom(arm, anim, current_time);
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

    /// Interpolate animation data from two frames.
    ///
    /// Animations loop after the end time.
    ///
    /// \param arm Armature base.
    /// \param anima Animation to interpolate.
    /// \param time Current time.
    void interpolateFrom(const Armature &arm, const Animation &anim, float current_time)
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
            //std::cout << "duplicating only animation frame " << anim.getFrame(0) << std::endl;
            m_mix_frame.duplicate(anim.getFrame(0));
        }
        else
        {
            float end_time = anim.getFrame(frame_count - 1).getTime();
            float bounded_time = congr(current_time, end_time);
            unsigned ii = 0;

            for(;;)
            {
                const AnimationFrame &ll = anim.getFrame(ii + 0);
                const AnimationFrame &rr = anim.getFrame(ii + 1);
                float rtime = rr.getTime();

                //std::cout << ll << std::endl << rr << std::endl;

#if defined(USE_LD)
                if(ll.getTime() >= rtime)
                {
                    std::ostringstream sstr;
                    sstr << "animation index " << ii << " has time " << ll.getTime() <<
                        " which is not smaller than frame " << (ii + 1) << " with time " << rr.getTime();
                    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
                }
#endif

                //std::cout << "time: " << bounded_time << " between " << (ii + 0) << " and " << (ii + 1) << std::endl;

                if(rtime >= bounded_time)
                {
                    m_mix_frame.interpolateFrom(ll, rr, bounded_time);
                    break;
                }

                ++ii;
#if defined(USE_LD)
                if(ii + 1 >= frame_count)
                {
                    std::ostringstream sstr;
                    sstr << "could not find frames to interpolate for time " << bounded_time;
                    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
                }
#endif
            }
        }

        unsigned bone_count = m_mix_frame.getBoneCount();
#if defined(USE_LD)
        if(arm.getBoneCount() != bone_count)
        {
            std::ostringstream sstr;
            sstr << "can't animate armature with " << arm.getBoneCount() << " bones with animation that has " <<
                bone_count << " bones";
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif

        m_rotations.resize(bone_count);
        m_matrices.resize(bone_count);

        for(unsigned ii = 0; (bone_count > ii); ++ii)
        {
            BoneState &st = m_mix_frame.getBoneState(ii);
            quat rot = st.getRotation();

#if 0
            if(ii == 1)
            {
                //[ 0.92388 ; 0.382683 ; 0 ; 0 ]
                rot = quat(0.92388f, -0.382683f, 0.0f, 0.0f);
            }
            else if(ii == 2)
            {
                //[ 0.906308 ; -0.422618 ; -0 ; -0 ]
                rot = quat(0.906308f, 0.422618f, -0.0f, -0.0f);
            }
#endif

#if 0
            std::cout << "bone rotation:  " << bn.getRotation() << std::endl;
            std::cout << "mixed rotation: " << st.getRotation() << std::endl;
            std::cout << "rotation: " << rot << std::endl;

            m_rotations[ii] = mat3::identity();

            std::cout << "Bone " << ii << " (" << st << "):\n" << m_rotations[ii] << std::endl;
#else
            m_rotations[ii] = mat3::rotation(rot);
#endif
        }

        // Hierarchical transforms are not necessarily on.
        if(anim.isHierarchical())
        {
            arm.hierarchicalTransform(m_rotations.data());
        }

        for(unsigned ii = 0; (bone_count > ii); ++ii)
        {
            const Bone &bn = arm.getBone(ii);
            const BoneState &st = m_mix_frame.getBoneState(ii);
            mat4 trn = mat4::translation(-bn.getPosition());
            mat4 trp_rot(m_rotations[ii], st.getPosition()); // Positive translate and rotation.

#if 0
            std::cout << "bone position:  " << bn.getPosition() << std::endl;
            std::cout << "mixed position: " << st.getPosition() << std::endl;
            std::cout << "diff: " << (st.getPosition() - bn.getPosition()) << std::endl;

            //m_matrices[ii] = mat4::identity();
            m_matrices[ii] = trp * rot * trn;

            std::cout << "Bone " << ii << " (" << st << "):\n" << m_matrices[ii] << std::endl;
#else
            m_matrices[ii] = trp_rot * trn;
#endif
        }
    }
};

}

#endif
