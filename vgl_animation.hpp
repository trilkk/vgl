#ifndef VGL_ANIMATION_HPP
#define VGL_ANIMATION_HPP

#include "vgl_animation_frame.hpp"
#include "vgl_unique_ptr.hpp"

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
    /// \param animation_data_size Animation data size.
    /// \param scale Model scale.
    /// \param hierarchical Is the animation hierarchical?
    explicit Animation(const int16_t *data, unsigned bone_amount, unsigned animation_data_size, float scale,
            bool hierarchical = false) :
        m_hierarchical(hierarchical)
    {
        readRaw(data, bone_amount, animation_data_size, scale);
    }

private:
    /// Constructor.
    ///
    /// \param data Animation data.
    /// \param bone_amount Amount of bone elements.
    /// \param animation_data_size Animation data array size.
    void readRaw(const int16_t *data, unsigned bone_amount, unsigned animation_data_size, float scale)
    {
        unsigned frame_amount = bone_amount * 7;

#if defined(VGL_USE_LD)
        if(animation_data_size % (frame_amount + 1) != 0)
        {
            VGL_THROW_RUNTIME_ERROR("incompatible bone (" + to_string(bone_amount) + ") and animation (" +
                   to_string(animation_data_size) + ") amounts");
        }
#endif

        for(unsigned ii = 0; (ii < animation_data_size); ii += frame_amount + 1)
        {
            m_frames.emplace_back(data + ii, frame_amount, scale);
        }
    }

public:
    /// Accessor.
    ///
    /// \param idx Index.
    constexpr AnimationFrame& getFrame(unsigned idx)
    {
        return m_frames[idx];
    }
    /// Const accessor.
    ///
    /// \param idx Index.
    constexpr const AnimationFrame& getFrame(unsigned idx) const
    {
        return m_frames[idx];
    }

    /// Accessor.
    ///
    /// \return Bone count.
    constexpr unsigned getBoneCount() const
    {
        return m_frames.empty() ? 0 : m_frames[0].getBoneCount();
    }

    /// Accessor.
    ///
    /// \return Frame count.
    constexpr unsigned getFrameCount() const
    {
        return m_frames.size();
    }

    /// Tell if this animation is hierarchical.
    ///
    /// \return True if yes, false if no.
    constexpr bool isHierarchical() const
    {
        return m_hierarchical;
    }
    /// Set hierarchical status of this animation.
    ///
    /// \param op New hierarchical status.
    constexpr void setHierarchical(bool op)
    {
        m_hierarchical = op;
    }

public:
    /// Creates a new animation.
    ///
    /// \param data Animation data.
    /// \param bone_amount Amount of bone elements.
    /// \param animation_data_size Animation data array size.
    /// \param scale Scale to multiply with.
    /// \param hierarchical Is the animation hierarchical?
    /// \return New Animation.
    static unique_ptr<Animation> create(const int16_t *data, unsigned bone_amount, unsigned animation_data_size, float scale,
            bool hierarchical = false)
    {
        return unique_ptr<Animation>(new Animation(data, bone_amount, animation_data_size, scale, hierarchical));
    }

#if defined(VGL_USE_LD)
public:
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
