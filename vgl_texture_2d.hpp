#ifndef VGL_TEXTURE_2D_HPP
#define VGL_TEXTURE_2D_HPP

#include "vgl_image_2d.hpp"
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
    explicit Texture2D(GLenum target = GL_TEXTURE_2D) :
        Texture(target)
    {
    }

private:
    /// Update texture with data.
    /// \param width Width of the texture.
    /// \param height Height of the texture.
    /// \param channels Number of channels, 0 for depth texture.
    /// \param bpc Bytes per component in texture data.
    /// \param data Pointter to texture data, must be one byte per color channel per texel.
    /// \param filtering Filtering mode to use.
    /// \param wrap Wrap mode to use.
    void update(unsigned width, unsigned height, const TextureFormat& format, void* data, FilteringMode filtering,
            WrapMode wrap)
    {
        m_width = width;
        m_height = height;

        const Texture* prev_texture = updateBegin();

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

    /// Update texture with image data.
    ///
    /// Creates a 16-bit texture.
    ///
    /// \param image Image to update with.
    /// \param bpc Bytes per component to convert the image to (default: 1).
    /// \param filtering Filtering mode (default: trilinear).
    /// \param wrap Wrap mode (defaut: wrap).
    void update(Image2D &image, unsigned bpc = 1, FilteringMode filtering = FilteringMode::TRILINEAR,
            WrapMode wrap = WrapMode::WRAP)
    {
        vector<uint8_t> export_data = image.getExportData(bpc);
        TextureFormat format(image.getChannelCount(), bpc, export_data.data());
        update(image.getWidth(), image.getHeight(), format, export_data.data(), filtering, wrap);
    }

public:
    /// Create a new texture with no data and given format.
    ///
    /// Usable for framebuffer textures with exotic formats.
    ///
    /// \param width Texture width.
    /// \param height Texture height.
    /// \param channels Number of channels, use 0 for depth texture.
    /// \param bpc Bytes per component for fragments (default: 1).
    /// \param filtering Filtering mode.
    /// \param wrap Wrap mode.
    static unique_ptr<Texture2D> create(unsigned width, unsigned height, const TextureFormat& format,
            FilteringMode filtering = FilteringMode::BILINEAR, WrapMode wrap = WrapMode::CLAMP)
    {
        unique_ptr<Texture2D> ret(new Texture2D(GL_TEXTURE_2D));
        ret->update(width, height, format, nullptr, filtering, wrap);
        return ret;
    }
    /// Create a new texture with no data.
    ///
    /// Usable for framebuffer textures. By default, 4 channels since RGB framebuffer is an extension.
    ///
    /// \param width Texture width.
    /// \param height Texture height.
    /// \param channels Number of channels, use 0 for depth texture.
    /// \param bpc Bytes per component for fragments (default: 1).
    /// \param filtering Filtering mode.
    /// \param wrap Wrap mode.
    static unique_ptr<Texture2D> create(unsigned width, unsigned height, unsigned channels = 4, unsigned bpc = 1,
            FilteringMode filtering = FilteringMode::BILINEAR, WrapMode wrap = WrapMode::CLAMP)
    {
        TextureFormat format(channels, bpc, nullptr);
        return create(width, height, format, filtering, wrap);
    }

    /// Create a new texture with image data.
    ///
    /// Usable for any kinds of textures.
    ///
    /// \param img Image.
    /// \param bpc Bytes per component for fragments (default: 1).
    /// \param filtering Filtering mode.
    /// \param wrap Wrap mode.
    static unique_ptr<Texture2D> create(Image2D& img, unsigned bpc = 1, FilteringMode filtering = FilteringMode::TRILINEAR,
            WrapMode wrap = WrapMode::WRAP)
    {
        unique_ptr<Texture2D> ret(new Texture2D(GL_TEXTURE_2D));
        ret->update(img, bpc, filtering, wrap);
        return ret;
    }
};

/// Texture2D unique pointer type.
using Texture2DUptr = unique_ptr<Texture2D>;

}

#endif
