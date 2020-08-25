#ifndef VGL_ANIMATION_HPP
#define VGL_ANIMATION_HPP

#include "vgl_animation_frame.hpp"

namespace vgl
{

/// Animation path.
class Animation
{
private:
    /// Frame data.
    vector<AnimationFrame> m_frames;

    /// Is this animation hierarchical?
    bool m_hierarchical = false;

public:
    /// Empty constructor.
    constexpr explicit Animation(bool hierarchical = false) noexcept:
        m_hierarchical(hierarchical)
    {
    }

    /// Constructor.
    ///
    /// \param data Animation data.
    /// \param bone_amount Amount of bone elements.
    /// \param animation_amount Amount of animation elements.
    /// \param scale Model scale.
    /// \param hierarchical Is the animation hierarchical?
    explicit Animation(const int16_t *data, unsigned bone_amount, unsigned animation_amount, float scale,
            bool hierarchical = false) :
        m_hierarchical(hierarchical)
    {
        readRaw(data, bone_amount, animation_amount, scale);
    }

private:
    /// Constructor.
    ///
    /// \param data Animation data.
    /// \param bone_amount Amount of bone elements.
    /// \param animation_amount Amount of animation elements.
    void readRaw(const int16_t *data, unsigned bone_amount, unsigned animation_amount, float scale)
    {
        unsigned frame_amount = bone_amount / 3 * 7;

#if defined(USE_LD)
        if(animation_amount % (frame_amount + 1) != 0)
        {
            std::ostringstream sstr;
            sstr << "incompatible bone (" << bone_amount << ") and animation (" << animation_amount << ") amounts";
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif

        for(unsigned ii = 0; (ii < animation_amount); ii += frame_amount + 1)
        {
            m_frames.emplace_back(data + ii, frame_amount, scale);
        }
    }

public:
    /// Accessor.
    ///
    /// \param idx Index.
    AnimationFrame& getFrame(unsigned idx)
    {
        return m_frames[idx];
    }
    /// Const accessor.
    ///
    /// \param idx Index.
    const AnimationFrame& getFrame(unsigned idx) const
    {
        return m_frames[idx];
    }

    /// Accessor.
    ///
    /// \return Frame count.
    unsigned getFrameCount() const
    {
        return m_frames.size();
    }

    /// Tell if this animation is hierarchical.
    ///
    /// \return True if yes, false if no.
    bool isHierarchical() const
    {
        return m_hierarchical;
    }
    /// Set hierarchical status of this animation.
    ///
    /// \param op New hierarchical status.
    void setHierarchical(bool op)
    {
        m_hierarchical = op;
    }

public:
    /// Creates a new animation.
    ///
    /// \param data Animation data.
    /// \param bone_amount Amount of bone elements.
    /// \param animation_amount Amount of animation elements.
    /// \param scale Scale to multiply with.
    /// \param hierarchical Is the animation hierarchical?
    static unique_ptr<Animation> create(const int16_t *data, unsigned bone_amount, unsigned animation_amount, float scale,
            bool hierarchical = false)
    {
        return unique_ptr<Animation>(new Animation(data, bone_amount, animation_amount, scale, hierarchical));
    }

public:
#if defined(USE_LD)
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream &lhs, const Animation& rhs)
    {
        lhs << "Animation: " << rhs.m_frames.size() << " frames\n";
        for(const auto& vv : rhs.m_frames)
        {
            lhs << vv << std::endl;
        }
        return lhs;
    }
#endif
};

/// Animation unique pointer type.
using AnimationUptr = unique_ptr<Animation>;

}

#endif
