#ifndef VGL_TEXTURE_2D_HPP
#define VGL_TEXTURE_2D_HPP

#include "vgl_image_2d.hpp"
#include "vgl_state.hpp"
#include "vgl_task_dispatcher.hpp"
#include "vgl_texture.hpp"
#include "vgl_texture_format.hpp"
#include "vgl_unique_ptr.hpp"

namespace vgl
{

/// 2-dimensional texture.
class Texture2D : public Texture
{
private:
    /// Texture create settings.
    class TextureCreateSettings
    {
    private:
        /// Width.
        unsigned m_width;

        /// Height.
        unsigned m_height;

        /// Pointer to texture format.
        const TextureFormat& m_format;

        /// Pointer to data.
        const void* m_data;

        /// Filtering mode.
        FilteringMode m_filter_mode;

        /// Wrap mode.
        WrapMode m_wrap_mode;

    public:
        /// Constructor.
        ///
        /// \param width Width.
        /// \param height Height.
        /// \param format Format.
        /// \param data Data.
        /// \param filter Filtering mode.
        /// \param wrap Wrap mode.
        constexpr explicit TextureCreateSettings(unsigned width, unsigned height, const TextureFormat& format, const void* data,
                FilteringMode filter, WrapMode wrap) noexcept :
            m_width(width),
            m_height(height),
            m_format(format),
            m_data(data),
            m_filter_mode(filter),
            m_wrap_mode(wrap)
        {
        }

    public:
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

        /// Accessor.
        ///
        /// \return Texture format.
        constexpr const TextureFormat& getFormat() const noexcept
        {
            return m_format;
        }

        /// Accessor.
        ///
        /// \return Data.
        constexpr const void* getData() const noexcept
        {
            return m_data;
        }

        /// Accessor.
        ///
        /// \return Filter mode.
        constexpr FilteringMode getFilterMode() const noexcept
        {
            return m_filter_mode;
        }

        /// Accessor.
        ///
        /// \return Wrap mode.
        constexpr WrapMode getWrapMode() const noexcept
        {
            return m_wrap_mode;
        }
    };

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

    /// Update texture with data.
    /// \param width Width of the texture.
    /// \param height Height of the texture.
    /// \param channels Number of channels, 0 for depth texture.
    /// \param bpc Bytes per component in texture data.
    /// \param data Pointer to texture data, must be one byte per color channel per texel.
    /// \param filtering Filtering mode to use.
    /// \param wrap Wrap mode to use.
    void update(unsigned width, unsigned height, const TextureFormat& format, const void* data, FilteringMode filtering,
            WrapMode wrap)
    {
        m_width = width;
        m_height = height;

        const Texture* prev_texture = updateBegin();

        dnload_glTexImage2D(getType(), 0, format.getInternalFormat(),
                static_cast<GLsizei>(width), static_cast<GLsizei>(height),
                0, format.getFormat(), format.getType(), data);

#if defined(VGL_USE_LD)
        unsigned data_size = width * height * format.getTypeSize();
        if(setFiltering(data, filtering))
        {
            data_size = (data_size * 4) / 3;
        }
#else
        setFiltering(data, filtering);
#endif
        setWrapMode(wrap);

#if defined(VGL_USE_LD)
        vgl::increment_data_size_texture(data_size);
#endif

        updateEnd(prev_texture);
    }

private:
    /// Parallel texture creation function.
    ///
    /// \param width Texture width.
    /// \param height Texture height.
    /// \param format Format.
    /// \param data Data, may be nullptr.
    /// \param filtering Filtering mode.
    /// \param wrap Wrap mode.
    static unique_ptr<Texture2D> createInternal(unsigned width, unsigned height, const TextureFormat& format, void* data,
            FilteringMode filtering, WrapMode wrap)
    {
        TextureCreateSettings settings(width, height, format, data, filtering, wrap);
        Fence ret = TaskDispatcher::wait_main(task_create_texture, &settings);
        return unique_ptr<Texture2D>(static_cast<Texture2D*>(ret.getReturnValue()));
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
        return createInternal(width, height, format, nullptr, filtering, wrap);
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
        TextureFormatColor format(channels, bpc, nullptr);
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
        auto export_data = img.getExportData(bpc);
        TextureFormatColor format(img.getChannelCount(), bpc, export_data.data());
        return createInternal(img.getWidth(), img.getHeight(), format, export_data.data(), filtering, wrap);
    }

private:
    /// Export a texture from this image.
    ///
    /// \param op Pointer to creation settings.
    static void* task_create_texture(void* op)
    {
        Texture2D* ret = new Texture2D();
        const TextureCreateSettings* settings = static_cast<const TextureCreateSettings*>(op);
        ret->update(settings->getWidth(), settings->getHeight(), settings->getFormat(), settings->getData(),
                settings->getFilterMode(), settings->getWrapMode());
        return ret;
    }
};

/// Texture2D unique pointer type.
using Texture2DUptr = unique_ptr<Texture2D>;

}

#endif
