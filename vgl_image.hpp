#ifndef VGL_IMAGE_HPP
#define VGL_IMAGE_HPP

#include "vgl_limits.hpp"
#include "vgl_math.hpp"
#include "vgl_rand.hpp"
#include "vgl_vector.hpp"

#if defined(VGL_USE_LD)
#include <sstream>
#endif

namespace vgl
{

/// Base image class.
class Image
{
private:
    /// Image data.
    vector<float> m_data;

    /// Texel count.
    unsigned m_texel_count;

    /// Number of channels.
    unsigned m_channel_count;

private:
    /// Deleted copy constructor.
    Image(const Image&) = delete;
    /// Deleted assignment.
    Image& operator=(const Image&) = delete;

public:
    /// Constructor.
    ///
    /// \param texel_count Number of texels.
    /// \param channel_count Number of channels.
    explicit Image(unsigned texel_count, unsigned channel_count) :
        m_data(texel_count * channel_count),
        m_texel_count(texel_count),
        m_channel_count(channel_count)
    {
    }

protected:
    /// Accessor.
    ///
    /// \param idx Index.
    /// \return value.
    constexpr float getValue(unsigned idx) const noexcept
    {
        return m_data[idx];
    }

    /// Gets address for value at index.
    ///
    /// \return Address for value.
    constexpr float* getValueAddress(unsigned idx) noexcept
    {
        return &(m_data[idx]);
    }
    /// Gets address for value at index.
    ///
    /// \return Address for value.
    constexpr const float* getValueAddress(unsigned idx) const noexcept
    {
        return &(m_data[idx]);
    }

    /// Setter.
    ///
    /// \param idx Index.
    /// \param val Value.
    constexpr void setValue(unsigned idx, float value)
    {
        m_data[idx] = value;
    }

    /// Replace data.
    ///
    /// Data vector must be of same size as existing data.
    ///
    /// \param op New data.
    constexpr void replaceData(vector<float>&& op) noexcept
    {
#if defined(VGL_USE_LD)
        if(m_data.size() != op.size())
        {
            VGL_THROW_RUNTIME_ERROR("replacing data size " + to_string(op.size()) + " does not match data size " +
                    to_string(m_data.size()));
        }
#endif
        m_data = move(op);
    }

public:
    /// Clears a channel to a value.
    ///
    /// \param channel Channel to clear.
    /// \param value Value to clear to (default: 0.0f).
    void clear(unsigned channel, float value = 0.0f)
    {
#if defined(VGL_USE_LD)
        if(channel > m_channel_count)
        {
            VGL_THROW_RUNTIME_ERROR("trying to clear channel " + to_string(channel) + " in " +
                    to_string(m_channel_count + 1) + "-channel image");
        }
#endif
        for(unsigned ii = channel, ee = getElementCount(); (ii < ee); ii += m_channel_count)
        {
            m_data[ii] = value;
        }
    }

    /// Recreates the export data array as UNORM data.
    ///
    /// \param bpc Bytes per component to convert to (default: 1).
    /// \return Vector containing the image data.
    vector<uint8_t> getExportData(unsigned bpc = 1) const
    {
        unsigned element_count = getElementCount();

        // Floats do not need to be converted.
        if(bpc == 4)
        {
            vector<uint8_t> ret(element_count * 4);
            void* erase_align_data = ret.data();
            float* export_data = reinterpret_cast<float*>(erase_align_data);

            for(unsigned ii = 0; (ii < element_count); ++ii)
            {
                export_data[ii] = m_data[ii];
            }

            return ret;
        }

        if(bpc == 2)
        {
            vector<uint8_t> ret(element_count * 2);
            void* erase_align_data = ret.data();
            uint16_t* export_data = reinterpret_cast<uint16_t*>(erase_align_data);

            for(unsigned ii = 0; (ii < element_count); ++ii)
            {
                export_data[ii] = static_cast<uint16_t>(0.5f + clamp(m_data[ii], 0.0f, 1.0f) * 65535.0f);
            }

            return ret;
        }

#if defined(VGL_USE_LD)
        if(bpc != 1)
        {
            VGL_THROW_RUNTIME_ERROR("invalid bpc value for UNORM conversion: " + to_string(bpc));
        }
#endif

        vector<uint8_t> ret(element_count);

        for(unsigned ii = 0; (ii < element_count); ++ii)
        {
            ret[ii] = static_cast<uint8_t>(0.5f + clamp(m_data[ii], 0.0f, 1.0f) * 255.0f);
        }

        return ret;
    }

    /// Accessor.
    ///
    /// \return Texel count.
    constexpr unsigned getTexelCount() const noexcept
    {
        return m_texel_count;
    }

    /// Accessor.
    ///
    /// \return Number of channels.
    constexpr unsigned getChannelCount() const noexcept
    {
        return m_channel_count;
    }

    /// Gets the number of elements in the image.
    ///
    /// Element count is the number of texels times the number of channels.
    ///
    /// \return Number of elements.
    constexpr unsigned getElementCount() const noexcept
    {
        return m_texel_count * m_channel_count;
    }

    /// Normalize color level.
    ///
    /// \param channel Channel
    /// \param ambient Ambient level (default: 0.0f).
    void normalize(unsigned channel, float ambient = 0.0f)
    {
        unsigned element_count = m_texel_count * m_channel_count;
        float min_value = numeric_limits<float>::max();
        float max_value = numeric_limits<float>::lowest();

        for(unsigned ii = channel; (element_count > ii); ii += m_channel_count)
        {
            float val = m_data[ii];

            min_value = min(val, min_value);
            max_value = max(val, max_value);
        }

        // If all values are identical, skip normalization.
        if(max_value != min_value)
        {
            float mul = (1.0f - ambient) / (max_value - min_value);

            for(unsigned ii = channel; (element_count > ii); ii += m_channel_count)
            {
                float val = m_data[ii];

                m_data[ii] = (mul * (val - min_value)) + ambient;
            }
        }
    }

#if !defined(VGL_DISABLE_RAND)
    /// Fill image with noise.
    ///
    /// \param nfloor Noise floor.
    /// \param nceil Noise ceiling.
    void noise(float nfloor = 0.0f, float nceil = 1.0f)
    {
        for(unsigned ii = 0, ee = getElementCount(); (ii < ee); ++ii)
        {
            m_data[ii] = frand(nfloor, nceil);
        }
    }
#endif
};

}

#endif
