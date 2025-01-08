#ifndef VGL_IMAGE_2D_HPP
#define VGL_IMAGE_2D_HPP

#include "vgl_image.hpp"
#include "vgl_vec2.hpp"

namespace vgl
{

/// Base 2-dimensional image class.
class Image2D : public Image
{
private:
    /// Width.
    unsigned m_width;

    /// Height.
    unsigned m_height;

public:
    /// Constructor.
    ///
    /// \param width Image width.
    /// \param height Image height.
    /// \param channels Number of channels.
    explicit Image2D(unsigned width, unsigned height, unsigned channels) :
        Image(width * height, channels),
        m_width(width),
        m_height(height)
    {
    }

private:
    /// Check that accessed index is valid.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param ch Channel index.
    constexpr void accessCheck(unsigned px, unsigned py, unsigned ch) const
    {
#if defined(VGL_USE_LD) && defined(DEBUG)
        if((px >= m_width) || (py >= m_height) || (ch >= getChannelCount()))
        {
            VGL_THROW_RUNTIME_ERROR("image " + to_string(m_width) + ";" + to_string(m_height) + ";" +
                    to_string(getChannelCount()) + ": accessing " + to_string(px) + ";" + to_string(py) + ";" +
                    to_string(ch));
        }
#else
        (void)px;
        (void)py;
        (void)ch;
#endif
    }

    /// Get index for coordinates.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \return Index.
    constexpr unsigned getIndex(unsigned px, unsigned py) const noexcept
    {
        return ((py * m_width) + px) * getChannelCount();
    }

protected:
    /// Sample from the image.
    ///
    /// Note that this happens at the origin of texels as opposed to center.
    ///
    /// \param px X coordinate [0, 1[.
    /// \param py Y coordinate [0, 1[.
    /// \param pc Channel.
    /// \param nearest True to sample nearest as opposed to linear.
    /// \return Sampled color.
    VGL_MATH_CONSTEXPR float sample(float px, float py, unsigned pc, bool nearest) const noexcept
    {
        float fwidth = static_cast<float>(m_width);
        float fheight = static_cast<float>(m_height);
        float cx = congr(px, 1.0f) * fwidth;
        float cy = congr(py, 1.0f) * fheight;

        unsigned ux = static_cast<unsigned>(cx);
        unsigned uy = static_cast<unsigned>(cy);
        float fract_x = cx - static_cast<float>(ux);
        float fract_y = cy - static_cast<float>(uy);

        // Wrap ux/uy to 0 if they have, due to floating point inaccuracy ended up as width/height.
        if(ux >= m_width)
        {
            ux = 0;
            fract_x = 0.0f;
        }
        if(uy >= m_height)
        {
            uy = 0;
            fract_y = 0.0f;
        }

        unsigned x1 = ux;
        unsigned x2 = (ux + 1) % m_width;
        unsigned y1 = uy;
        unsigned y2 = (uy + 1) % m_height;

        if(nearest)
        {
            if(fract_x < 0.5f)
            {
                if(fract_y < 0.5f)
                {
                    return getValue(x1, y1, pc);
                }
                return getValue(x1, y2, pc);
            }
            if(fract_y < 0.5f)
            {
                return getValue(x2, y1, pc);
            }
            return getValue(x2, y2, pc);
        }

        return smooth_mix(
                smooth_mix(getValue(x1, y1, pc), getValue(x2, y1, pc), fract_x),
                smooth_mix(getValue(x1, y2, pc), getValue(x2, y2, pc), fract_x),
                fract_y);
    }

public:
    /// Apply a low-pass filter over the texture.
    ///
    /// This low-pass filter will wrap around the texture edges.
    ///
    /// \param op Kernel size.
    void filterLowpass(int op)
    {
#if defined(VGL_USE_LD)
        if(4 < getChannelCount())
        {
            VGL_THROW_RUNTIME_ERROR("cannot filter texture with " + to_string(getChannelCount()) + " channels");
        }
#endif
        unsigned element_count = getWidth() * getHeight() * getChannelCount();
        vector<float> replacement_data(element_count);
        int iwidth = static_cast<int>(getWidth());
        int iheight = static_cast<int>(getHeight());
        int ichannels = static_cast<int>(getChannelCount());

        for(int ii = 0; (ii < iwidth); ++ii)
        {
            for(int jj = 0; (jj < iheight); ++jj)
            {
                float values[] = { 0.0f, 0.0f, 0.0f, 0.0f };
                int idx = (jj * iwidth + ii) * ichannels;
                int divisor = 0;

                for(int kk = -op; (kk <= op); ++kk)
                {
                    int rx = ii + kk;

                    while(rx < 0)
                    {
                        rx += iwidth;
                    }
                    while(rx >= iwidth)
                    {
                        rx -= iwidth;
                    }

                    for(int ll = -op; (ll <= op); ++ll)
                    {
                        int ry = jj + ll;

                        while(ry < 0)
                        {
                            ry += iheight;
                        }
                        while(ry >= iheight)
                        {
                            ry -= iheight;
                        }

                        unsigned ux = static_cast<unsigned>(rx);
                        unsigned uy = static_cast<unsigned>(ry);

                        for(unsigned mm = 0; (getChannelCount() > mm); ++mm)
                        {
                            values[mm] += getValue(ux, uy, mm);
                        }

                        ++divisor;
                    }
                }

                for(int mm = 0; (ichannels > mm); ++mm)
                {
                    replacement_data[idx + mm] = values[mm] / static_cast<float>(divisor);
                }
            }
        }

        // Replace existing data with the filtered data.
        replaceData(move(replacement_data));
    }

    /// Gets the address for a pixel.
    ///
    /// \param px X position.
    /// \param py Y position.
    /// \param channel Channel to access.
    /// \return Address of the closest pixel to given location.
    VGL_MATH_CONSTEXPR float* getClosestPixelAddress(float px, float py, unsigned channel) noexcept
    {
        float fwidth = static_cast<float>(m_width);
        float fheight = static_cast<float>(m_height);
        float cx = congr(px, 1.0f) * fwidth;
        float cy = congr(py, 1.0f) * fheight;

        return getValueAddress(static_cast<unsigned>(cx), static_cast<unsigned>(cy), channel);
    }
    /// Gets the address for a pixel wrapper.
    ///
    /// \param pos Position.
    /// \param channel Channel to access.
    /// \return Address of the closest pixel to given location.
    VGL_MATH_CONSTEXPR float* getClosestPixelAddress(const vec2& pos, unsigned channel) noexcept
    {
        return getClosestPixelAddress(pos.x(), pos.y(), channel);
    }

    /// Accessor.
    ///
    /// \return Width.
    constexpr unsigned getWidth() const noexcept
    {
        return m_width;
    }

    /// Accessor.
    ///
    /// \return Height.
    constexpr unsigned getHeight() const noexcept
    {
        return m_height;
    }

    /// Get value.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param ch Channel index.
    /// \return Value.
    constexpr float getValue(unsigned px, unsigned py, unsigned ch) const
    {
        accessCheck(px, py, ch);
        unsigned idx = getIndex(px, py);
        return Image::getValue(idx + ch);
    }
    /// Get value address.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param ch Channel index.
    /// \return Value address.
    constexpr float* getValueAddress(unsigned px, unsigned py, unsigned ch)
    {
        accessCheck(px, py, ch);
        unsigned idx = getIndex(px, py);
        return Image::getValueAddress(idx + ch);
    }
    /// Set value.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param ch Channel index.
    /// \param val Value.
    constexpr void setValue(unsigned px, unsigned py, unsigned ch, float val)
    {
        accessCheck(px, py, ch);
        unsigned idx = getIndex(px, py);
        Image::setValue(idx + ch, val);
    }

    /// Sample (in a bilinear fashion) from the image.
    ///
    /// \param px X coordinate [0, 1[.
    /// \param py Y coordinate [0, 1[.
    /// \param pc Channel.
    VGL_MATH_CONSTEXPR float sampleLinear(float px, float py, unsigned pc) const noexcept
    {
        return sample(px, py, pc, false);
    }
    /// Sample linear wrapper.
    ///
    /// \param pos Sampling coordinates (components: [0, 1[).
    /// \param pc Channel.
    VGL_MATH_CONSTEXPR float sampleLinear(const vec2& pos, unsigned pc) const noexcept
    {
        return sampleLinear(pos.x(), pos.y(), pc);
    }
    /// Sample (in a nearest fashion) from the image.
    ///
    /// \param px X coordinate [0, 1[.
    /// \param py Y coordinate [0, 1[.
    /// \param pc Channel.
    VGL_MATH_CONSTEXPR float sampleNearest(float px, float py, unsigned pc) const noexcept
    {
        return sample(px, py, pc, true);
    }
    /// Sample nearest wrapper.
    ///
    /// \param pos Sampling coordinates (components: [0, 1[).
    /// \param pc Channel.
    VGL_MATH_CONSTEXPR float sampleNearest(const vec2& pos, unsigned pc) const noexcept
    {
        return sampleNearest(pos.x(), pos.y(), pc);
    }
};

}

#endif
