#ifndef VGL_TEXTURE_2D_HPP
#define VGL_TEXTURE_2D_HPP

#include "vgl_state.hpp"
#include "vgl_texture.hpp"
#include "vgl_texture_format.hpp"
#include "vgl_unique_ptr.hpp"

namespace vgl
{

/// 2-dimensional texture.
class Texture2D : public Texture
{
private:
    /// Texture width.
    unsigned m_width;

    /// Texture height.
    unsigned m_height;

public:
    /// Constructor.
    ///
    /// \param target Texture target.
    /// \param width Width of the texture.
    /// \param height Height of the texture.
    /// \param channels Number of channels, 0 for depth texture.
    /// \param bpc Bytes per component in texture data.
    /// \param data Pointter to texture data, must be one byte per color channel per texel.
    /// \param wrap Wrap mode to use.
    /// \param filtering Filtering mode to use.
    explicit Texture2D(GLenum target, unsigned width, unsigned height, unsigned channels, unsigned bpc, void* data,
            WrapMode wrap, FilteringMode filtering) :
        Texture(target),
        m_width(width),
        m_height(height)
    {
        const Texture* prev_texture = updateBegin();
        TextureFormat format(channels, bpc, data);

        dnload_glTexImage2D(getType(), 0, format.getInternalFormat(),
                static_cast<GLsizei>(width), static_cast<GLsizei>(height),
                0, format.getFormat(), format.getType(), data);

#if defined(USE_LD)
        unsigned data_size = width * height * format.getTypeSize();
        if(setFiltering(data, filtering))
        {
            data_size = (data_size * 4) / 3;
        }
#else
        setFiltering(data, filtering);
#endif
        setWrapMode(wrap);

#if defined(USE_LD)
        vgl::increment_data_size_texture(data_size);
#endif

        updateEnd(prev_texture);
    }

public:
    /// Accessor.
    ///
    /// \return Texture width.
    constexpr unsigned getWidth() const noexcept
    {
        return m_width;
    }

    /// Accessor.
    ///
    /// \return Texture height.
    constexpr unsigned getHeight() const noexcept
    {
        return m_height;
    }

public:
    /// Update contents with nothing.
    ///
    /// Usable for framebuffer textures. By default, 4 channels since RGB framebuffer is an extension.
    ///
    /// \param width Texture width.
    /// \param height Texture height.
    /// \param channels Number of channels, use 0 for depth texture.
    /// \param bpc Bytes per component for fragments (default: 1).
    /// \param filtering Filtering mode.
    static unique_ptr<Texture2D> create(unsigned width, unsigned height, unsigned channels = 4, unsigned bpc = 1,
            FilteringMode filtering = FilteringMode::BILINEAR, WrapMode wrap = WrapMode::CLAMP)
    {
        return unique_ptr<Texture2D>(new Texture2D(GL_TEXTURE_2D, width, height, channels, bpc, nullptr, wrap,
                    filtering));
    }
};

/// Texture2D unique pointer type.
using Texture2DUptr = unique_ptr<Texture2D>;

}

#endif
