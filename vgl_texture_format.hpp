#ifndef VGL_TEXTURE_FORMAT_HPP
#define VGL_TEXTURE_FORMAT_HPP

#if defined(USE_LD)
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
private:
    /// OpenGL texture format.
    GLenum m_format;

    /// OpenGL texture internal format.
    GLint m_internal_format;

    /// OpenGL type.
    GLenum m_type;

#if defined(USE_LD)
    /// Size of one texel in bytes.
    unsigned m_type_size;
#endif

public:
    /// Constructor.
    ///
    /// \param channels Number of channels, 0 for depth texture.
    /// \param bpc Bytes per component.
    /// \param data Pointer to texture data.
    explicit TextureFormat(unsigned channels, unsigned bpc, void* data) :
        m_format(determine_format(channels)),
        m_internal_format(determine_internal_format(channels, bpc, data)),
        m_type(determine_type(channels, bpc))
#if defined(USE_LD)
        , m_type_size(bpc * channels)
#endif
        {
#if defined(USE_LD)
            if(m_type == GL_UNSIGNED_SHORT_5_6_5)
            {
                m_type_size = 2;
            }
#endif
        }

    /// Explicit constructor.
    ///
    /// \param format Format.
    /// \param internal_format.
    /// \param type.
    explicit TextureFormat(GLenum format, GLint internal_format, GLenum type) :
        m_format(format),
        m_internal_format(internal_format),
        m_type(type)
#if defined(USE_LD)
        , m_type_size(0)
#endif
    {
    }

private:
    /// Get GL texture format for channel number.
    ///
    /// \param channels Number of channels, 0 for depth texture.
    /// \return Texture format.
    GLenum determine_format(unsigned channels)
    {
        if(channels == 0)
        {
            return GL_DEPTH_COMPONENT;
        }
        if(channels == 1)
        {
#if defined(DNLOAD_GLESV2)
            return GL_LUMINANCE;
#else
            return GL_RED;
#endif
        }
        if(channels == 2)
        {
#if defined(DNLOAD_GLESV2)
            return GL_LUMINANCE_ALPHA;
#else
            return GL_RG;
#endif
        }
        if(channels == 3)
        {
            return GL_RGB;
        }
#if defined(USE_LD)
        if(channels != 4)
        {
            std::ostringstream sstr;
            sstr << "invalid channel count: " << channels;
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif
        return GL_RGBA;
    }

    /// Get GL texture format for channel number.
    ///
    /// \param channels Number of channels, 0 for depth texture.
    /// \param bpc Bytes per component.
    /// \param data Pointer to texture data.
    /// \return Texture internal format.
    GLint determine_internal_format(unsigned channels, unsigned bpc, void* data)
    {
        if(channels == 0)
        {
            return GL_DEPTH_COMPONENT;
        }
        if(channels == 1)
        {
            return determine_internal_format_r(bpc, data);
        }
        if(channels == 2)
        {
            return determine_internal_format_rg(bpc, data);
        }
        if(channels == 3)
        {
            return determine_internal_format_rgb(bpc, data);
        }
#if defined(USE_LD)
        if(channels != 4)
        {
            std::ostringstream sstr;
            sstr << "invalid channel count: " << channels;
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif
        return determine_internal_format_rgba(bpc, data);
    }

    /// Determine R internal format.
    ///
    /// \param bpc Bytes per component.
    /// \param data Pointer to texture data.
    /// \return Texture internal format.
    GLint determine_internal_format_r(unsigned bpc, void* data)
    {
#if defined(DNLOAD_GLESV2)
        (void)data;
#else
        if(bpc == 4)
        {
            return GL_R32F;
        }
        else
#endif
            if(bpc == 2)
        {
#if defined(DNLOAD_GLESV2)
            return GL_FLOAT;
#else
            return data ? GL_R16 : GL_R16F;
#endif
        }
#if defined(USE_LD)
        if(bpc != 1)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("invalid bytes per channel count for R format: " +
                        std::to_string(bpc)));
        }
#endif
#if defined(DNLOAD_GLESV2)
        return GL_LUMINANCE;
#else
        return GL_RED;
#endif
    }

    /// Determine RG internal format.
    ///
    /// \param bpc Bytes per component.
    /// \param data Pointer to texture data.
    /// \return Texture internal format.
    GLint determine_internal_format_rg(unsigned bpc, void* data)
    {
#if defined(DNLOAD_GLESV2)
        (void)data;
#else
        if(bpc == 4)
        {
            return GL_RG32F;
        }
        else if(bpc == 2)
        {
            return data ? GL_RG16 : GL_RG16F;
        }
#endif
#if defined(USE_LD)
        if(bpc != 1)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("invalid bytes per channel count for RG format: " +
                        std::to_string(bpc)));
        }
#endif
#if defined(DNLOAD_GLESV2)
        return GL_LUMINANCE_ALPHA;
#else
        return GL_RG;
#endif
    }

    /// Determine RGB internal format.
    ///
    /// \param bpc Bytes per component.
    /// \param data Pointer to texture data.
    /// \return Texture internal format.
    GLint determine_internal_format_rgb(unsigned bpc, void* data)
    {
#if defined(DNLOAD_GLESV2)
        (void)data;
#else
        if(bpc == 4)
        {
            return GL_RGB32F;
        }
        else if(bpc == 2)
        {
            return data ? GL_RGB16 : GL_RGB16F;
        }
#endif
#if defined(USE_LD)
        if(bpc >= 3)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("invalid bytes per channel count for RGB format: " +
                        std::to_string(bpc)));
        }
#endif
        return GL_RGB;
    }

    /// Determine RGBA internal format.
    ///
    /// \param bpc Bytes per component.
    /// \param data Pointer to texture data.
    /// \return Texture internal format.
    GLint determine_internal_format_rgba(unsigned bpc, void* data)
    {
#if defined(DNLOAD_GLESV2)
        (void)data;
#else
        if(bpc == 4)
        {
            return GL_RGBA32F;
        }
        else if(bpc == 2)
        {
            return data ? GL_RGBA16 : GL_RGBA16F;
        }
#endif
#if defined(USE_LD)
        if(bpc != 1)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("invalid bytes per channel count for RGBA format: " +
                        std::to_string(bpc)));
        }
#endif
        return GL_RGBA;
    }

    /// Get GL data type for bits per pixel.
    ///
    /// \param channels Number of channels, 0 for depth texture.
    /// \param bpc Bytes per component.
    /// \param data Pointer to texture data.
    /// \return Texture data type.
    GLenum determine_type(unsigned channels, unsigned bpc)
    {
        if(bpc == 4)
        {
#if defined(DNLOAD_GLESV2)
            if(channels == 0)
            {
                return GL_UNSIGNED_INT;
            }
#endif
            return GL_FLOAT;
        }
        if(bpc == 2)
        {
            return GL_UNSIGNED_SHORT;
        }
        if(bpc == 1)
        {
            return GL_UNSIGNED_BYTE;
        }
#if defined(USE_LD)
        if((bpc != 0) || (channels != 3))
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("invalid channel and bpc combination to determine type: " +
                        std::to_string(channels) + " / " + std::to_string(bpc)));
        }      
#endif
        return GL_UNSIGNED_SHORT_5_6_5;
    }

public:
    /// Accessor.
    ///
    /// \return OpenGL format.
    GLenum getFormat() const
    {
        return m_format;
    }

    /// Accessor.
    ///
    /// \return OpenGL internal format.
    GLint getInternalFormat() const
    {
        return m_internal_format;
    }

    /// Accessor.
    ///
    /// \return OpenGL type.
    GLenum getType() const
    {
        return m_type;
    }

#if defined(USE_LD)
    /// Accessor.
    ///
    /// \return Data size multiplier.
    unsigned getTypeSize() const
    {
        return m_type_size;
    }
    /// Setter.
    ///
    /// \param op Type size.
    void setTypeSize(unsigned op)
    {
        m_type_size = op;
    }
#endif

public:
#if defined(USE_LD)
    /// Output to stream.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const TextureFormat& rhs)
    {
        std::ios::fmtflags stored_flags = lhs.flags();
        lhs << std::hex << "TextureFormat(0x" << rhs.m_format << ", 0x" << rhs.m_internal_format << ", 0x" <<
            rhs.m_type;
        lhs.flags(stored_flags);
        return lhs << ", " << rhs.m_type_size << ")";
    }
#endif
};

}

#endif
