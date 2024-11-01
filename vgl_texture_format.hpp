#ifndef VGL_TEXTURE_FORMAT_HPP
#define VGL_TEXTURE_FORMAT_HPP

#if defined(USE_LD)
#include "vgl_string.hpp"
#include <ios>
#endif

/// \cond
/// Defines the number of bytes used for RGB24 texture format.
/// If not defined, default assumes normal R8G8B8 format.
//#define VGL_RGB24_BYTES 3
/// \endcond

namespace vgl
{

/// Texture format information.
class TextureFormat
{
protected:
    /// OpenGL texture format.
    GLenum m_format;

    /// OpenGL texture internal format.
    GLint m_internal_format;

    /// OpenGL type.
    GLenum m_type;

#if defined(USE_LD)
    /// Size of one texel in bytes.
    unsigned m_type_size = 0;
#endif

public:
    /// Constructor.
    ///
    /// \param format Format.
    /// \param internal_format.
    /// \param type.
    constexpr explicit TextureFormat(GLenum format, GLint internal_format, GLenum type) noexcept :
        m_format(format),
        m_internal_format(internal_format),
        m_type(type)
    {
    }

public:
    /// Accessor.
    ///
    /// \return OpenGL format.
    constexpr GLenum getFormat() const noexcept
    {
        return m_format;
    }

    /// Accessor.
    ///
    /// \return OpenGL internal format.
    constexpr GLint getInternalFormat() const noexcept
    {
        return m_internal_format;
    }

    /// Accessor.
    ///
    /// \return OpenGL type.
    constexpr GLenum getType() const noexcept
    {
        return m_type;
    }

#if defined(USE_LD)
    /// Accessor.
    ///
    /// \return Data size multiplier.
    constexpr unsigned getTypeSize() const noexcept
    {
        return m_type_size;
    }
    /// Setter.
    ///
    /// \param op Type size.
    constexpr void setTypeSize(unsigned op) noexcept
    {
        m_type_size = op;
    }
#endif

public:
#if defined(USE_LD)
    /// Converts TextureFormat to a string.
    ///
    /// \param op Input.
    /// \return String output.
    friend string to_string(const TextureFormat& op)
    {
        std::stringstream sstr;
        sstr << std::hex << "TextureFormat(0x" << op.m_format << ", 0x" << op.m_internal_format << ", 0x" <<
            op.m_type << ", " << op.m_type_size << ")";
        return string(sstr.str().c_str());
    }

    /// Output to stream.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const TextureFormat& rhs)
    {
        return lhs << to_string(rhs);
    }
#endif
};

/// Color texture format.
class TextureFormatColor : public TextureFormat
{
public:
    /// Constructor.
    ///
    /// \param channels Number of channels, 0 for depth texture.
    /// \param bpc Bytes per component.
    /// \param data Pointer to texture data.
    constexpr explicit TextureFormatColor(unsigned channels, unsigned bpc, void* data) :
        TextureFormat(determine_format(channels), determine_internal_format(channels, bpc, data), determine_type(channels, bpc, data))
    {
#if defined(USE_LD)
        if (m_type == GL_UNSIGNED_SHORT_5_6_5)
        {
            m_type_size = 2;
        }
        else
        {
            m_type_size = bpc * channels;
        }
#endif
    }

private:
    /// Get GL texture format.
    ///
    /// \param channels Number of channels.
    /// \return Texture format.
    static constexpr GLenum determine_format(unsigned channels)
    {
        switch(channels)
        {
        case 1:
#if defined(DNLOAD_GLESV2)
            return GL_LUMINANCE;
#else
            return GL_RED;
#endif

        case 2:
#if defined(DNLOAD_GLESV2)
            return GL_LUMINANCE_ALPHA;
#else
            return GL_RG;
#endif

        case 3:
            return GL_RGB;

        case 4:
#if !defined(USE_LD)
        default:
#endif
            return GL_RGBA;

#if defined(USE_LD)
        default:
            VGL_THROW_RUNTIME_ERROR("invalid color channel count: " + to_string(channels));
#endif
        }
    }

    /// Determine GL internal texture format.
    ///
    /// \param channels Number of channels.
    /// \param bpc Bytes per channel.
    /// \param data Pointer to texture data.
    /// \return Texture internal format.
    static constexpr GLint determine_internal_format(unsigned channels, unsigned bpc, void* data)
    {
        switch(channels)
        {
        case 1:
            return determine_internal_format_r(bpc, data);

        case 2:
            return determine_internal_format_rg(bpc, data);

        case 3:
            return determine_internal_format_rgb(bpc, data);

        case 4:
#if !defined(USE_LD)
        default:
#endif
            return determine_internal_format_rgba(bpc, data);

#if defined(USE_LD)
        default:
            VGL_THROW_RUNTIME_ERROR("invalid color channel count: " + to_string(channels));
#endif
        }
    }

    /// Determine R internal format.
    ///
    /// \param bpc Bytes per channel.
    /// \param data Pointer to texture data.
    /// \return Texture internal format.
    static constexpr GLint determine_internal_format_r(unsigned bpc, void* data)
    {
        switch(bpc)
        {
        case 4:
#if defined(DNLOAD_GLESV2)
            return GL_LUMINANCE;
#else
            return GL_R32F;
#endif

        case 2:
#if defined(DNLOAD_GLESV2)
            (void)data;
            return GL_LUMINANCE;
#else
            return data ? GL_R16 : GL_R16F;
#endif

        case 1:
#if !defined(USE_LD)
        default:
#endif
#if defined(DNLOAD_GLESV2)
            return GL_LUMINANCE;
#else
            return GL_R8;
#endif

#if defined(USE_LD)
        default:
            VGL_THROW_RUNTIME_ERROR("invalid bytes per channel for R format: " + to_string(bpc));
#endif
        }
    }

    /// Determine RG internal format.
    ///
    /// \param bpc Bytes per component.
    /// \param data Pointer to texture data.
    /// \return Texture internal format.
    static constexpr GLint determine_internal_format_rg(unsigned bpc, void* data)
    {
        switch(bpc)
        {
        case 4:
#if defined(DNLOAD_GLESV2)
            return GL_LUMINANCE_ALPHA;
#else
            return GL_RG32F;
#endif

        case 2:
#if defined(DNLOAD_GLESV2)
            (void)data;
            return GL_LUMINANCE_ALPHA;
#else
            return data ? GL_RG16 : GL_RG16F;
#endif

        case 1:
#if !defined(USE_LD)
        default:
#endif
#if defined(DNLOAD_GLESV2)
            return GL_LUMINANCE_ALPHA;
#else
            return GL_RG8;
#endif

#if defined(USE_LD)
        default:
            VGL_THROW_RUNTIME_ERROR("invalid bytes per channel for RG format: " + to_string(bpc));
#endif
        }
    }

    /// Determine RGB internal format.
    ///
    /// \param bpc Bytes per component.
    /// \param data Pointer to texture data.
    /// \return Texture internal format.
    static constexpr GLint determine_internal_format_rgb(unsigned bpc, void* data)
    {
        switch(bpc)
        {
        case 4:
#if defined(DNLOAD_GLESV2)
            return GL_RGB;
#else
            return GL_RGB32F;
#endif

        case 2:
#if defined(DNLOAD_GLESV2)
            (void)data;
            return GL_RGB;
#else
            return data ? GL_RGB16 : GL_RGB16F;
#endif

            // Special case - packed 565 RGB texture.
        case 0:
#if defined(DNLOAD_GLESV2)
            return GL_RGB;
#else
            return GL_RGB565;
#endif

        case 1:
#if !defined(USE_LD)
        default:
#endif
#if defined(DNLOAD_GLESV2)
            return GL_RGB;
#else
            return GL_RGB8;
#endif

#if defined(USE_LD)
        default:
            VGL_THROW_RUNTIME_ERROR("invalid bytes per channel for RGB format: " + to_string(bpc));
#endif
        }
    }

    /// Determine RGBA internal format.
    ///
    /// \param bpc Bytes per component.
    /// \param data Pointer to texture data.
    /// \return Texture internal format.
    static constexpr GLint determine_internal_format_rgba(unsigned bpc, void* data)
    {
        switch(bpc)
        {
        case 4:
#if defined(DNLOAD_GLESV2)
            return GL_RGBA;
#else
            return GL_RGBA32F;
#endif

        case 2:
#if defined(DNLOAD_GLESV2)
            (void)data;
            return GL_RGBA;
#else
            return data ? GL_RGBA16 : GL_RGBA16F;
#endif

        case 1:
#if !defined(USE_LD)
        default:
#endif
#if defined(DNLOAD_GLESV2)
            return GL_RGBA;
#else
            return GL_RGBA8;
#endif

#if defined(USE_LD)
        default:
            VGL_THROW_RUNTIME_ERROR("invalid bytes per channel for RGBA format: " + to_string(bpc));
#endif
        }
    }

    /// Determine GL data type.
    ///
    /// \param channels Number of channels, 0 for depth texture.
    /// \param bpc Bytes per component.
    /// \param data Pointer to texture data.
    /// \return Texture data type.
    static constexpr GLenum determine_type(unsigned channels, unsigned bpc, void* data)
    {
        switch(bpc)
        {
        case 4:
            return GL_FLOAT;

        case 2:
            return data ? GL_UNSIGNED_SHORT : GL_FLOAT;

            // Special case: packed data.
        case 0:
#if defined(USE_LD)
            if(channels == 3)
#endif
            {
                return GL_UNSIGNED_SHORT_5_6_5;
            }
#if defined(USE_LD)
            VGL_THROW_RUNTIME_ERROR("invalid channel count for special packed format: " + to_string(channels));
#endif

        case 1:
#if !defined(USE_LD)
        default:
#endif
            return GL_UNSIGNED_BYTE;

#if defined(USE_LD)
        default:
            VGL_THROW_RUNTIME_ERROR("bytes per channel for determining type: " + to_string(bpc));
#endif
        }
    }
};

/// Depth texture format.
class TextureFormatDepth : public TextureFormat
{
public:
    /// Constructor.
    ///
    /// \param bpc Bytes per channel.
    constexpr explicit TextureFormatDepth(unsigned bpc) :
        TextureFormat(determine_format(), determine_internal_format(bpc), determine_type(bpc))
    {
#if defined(USE_LD)
        m_type_size = bpc;
#endif
    }

private:
    /// Get GL texture format.
    ///
    /// \return Texture format.
    static constexpr GLenum determine_format()
    {
        return GL_DEPTH_COMPONENT;
    }

    /// Get GL internal texture format.
    ///
    /// \param bpc Bytes per channel.
    /// \return Texture internal format.
    static constexpr GLint determine_internal_format(unsigned bpc)
    {
        switch(bpc)
        {
        case 2:
#if defined(DNLOAD_GLESV2) && !defined(VGL_DISABLE_DEPTH_TEXTURE)
            return GL_DEPTH_COMPONENT;
#else
            return GL_DEPTH_COMPONENT16;
#endif

        case 3:
#if defined(DNLOAD_GLESV2) && !defined(VGL_DISABLE_DEPTH_TEXTURE)
            return GL_DEPTH_COMPONENT;
#elif !defined(GL_DEPTH_COMPONENT24)
            return 0x81A6;
#else
            return GL_DEPTH_COMPONENT24;
#endif

        case 4:
#if !defined(USE_LD)
        default:
#endif
#if defined(DNLOAD_GLESV2) && !defined(VGL_DISABLE_DEPTH_TEXTURE)
            return GL_DEPTH_COMPONENT;
#elif !defined(GL_DEPTH_COMPONENT32F)
            return 0x8CAC;
#else
            return GL_DEPTH_COMPONENT32F;
#endif

#if defined(USE_LD)
        default:
            VGL_THROW_RUNTIME_ERROR("invalid bytes per channel for depth format: " + to_string(bpc));
#endif
        }
    }

    /// Get GL data type.
    ///
    /// \param bpc Bytes per channel.
    /// \return Texture data type.
    static constexpr GLenum determine_type(unsigned bpc)
    {
        switch(bpc)
        {
        case 2:
            return GL_UNSIGNED_SHORT;

        case 3:
            return GL_UNSIGNED_INT;

        case 4:
#if !defined(USE_LD)
        default:
#endif
#if defined(DNLOAD_GLESV2) && !defined(VGL_DISABLE_DEPTH_TEXTURE)
            return GL_UNSIGNED_INT;
#else
            return GL_FLOAT;
#endif

#if defined(USE_LD)
        default:
            VGL_THROW_RUNTIME_ERROR("invalid bytes per channel for depth format: " + to_string(bpc));
#endif
        }
    }
};

}

#endif
