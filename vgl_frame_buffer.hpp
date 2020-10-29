#ifndef VGL_FRAME_BUFFER_HPP
#define VGL_FRAME_BUFFER_HPP

#include "vgl_texture_2d.hpp"

namespace vgl
{

/// Framebuffer.
class FrameBuffer
{
private:
    /// Currently active framebuffer.
    static const FrameBuffer* g_current_frame_buffer;

    /// Default frame buffer.
    static FrameBuffer g_default_frame_buffer;

private:
    /// Framebuffer id.
    GLuint m_id = 0;

    /// Attached render target texture.
    Texture2DUptr m_color_texture;

#if defined(VGL_DISABLE_DEPTH_TEXTURE)
    GLuint m_depth_buffer_id = 0;
#else
    /// Attached render target texture.
    Texture2DUptr m_depth_texture;
#endif

    /// Width of this framebuffer.
    unsigned m_width = 0;

    /// Height of this framebuffer.
    unsigned m_height = 0;

private:
    /// Deleted copy constructor.
    FrameBuffer(const FrameBuffer&) = delete;
    /// Deleted assignment.
    FrameBuffer& operator=(const FrameBuffer&) = delete;

private:
    /// Default constructor.
    constexpr explicit FrameBuffer() noexcept
    {
    }

public:
    /// Constructor.
    ///
    /// \param width Framebuffer width.
    /// \param height Framebuffer height.
    /// \param colorFormat Optional color format.
    /// \param depthFormat Optional depth format.
    /// \param filtering Filtering mode.
    /// \param wrap Wrap mode.
    explicit FrameBuffer(unsigned width, unsigned height, const optional<TextureFormatColor>& colorFormat,
            const optional<TextureFormatDepth>& depthFormat, FilteringMode filtering, WrapMode wrap) :
        m_width(width),
        m_height(height)
    {
#if defined(USE_LD)
        if((m_width <= 0) || (m_height <= 0))
        {
            std::ostringstream sstr;
            sstr << "invalid framebuffer dimensions: " << width << "x" << height;
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif

        dnload_glGenFramebuffers(1, &m_id);
        dnload_glBindFramebuffer(GL_FRAMEBUFFER, m_id);

        if(colorFormat)
        {
            m_color_texture = Texture2D::create(width, height, *colorFormat, filtering, wrap);
            dnload_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                    m_color_texture->getId(), 0);
        }

        if(depthFormat)
        {
#if defined(VGL_DISABLE_DEPTH_TEXTURE)
            dnload_glGenRenderbuffers(1, &m_depth_buffer_id);
            dnload_glBindRenderbuffer(GL_RENDERBUFFER, m_depth_buffer_id);
            dnload_glRenderbufferStorage(GL_RENDERBUFFER, depthFormat->getInternalFormat(), width, height);
            dnload_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth_buffer_id);
#else
            m_depth_texture = Texture2D::create(width, height, *depthFormat, filtering, wrap);
            dnload_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                    m_depth_texture->getId(), 0);
#endif
        }

#if defined(USE_LD)
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::ostringstream sstr;
            sstr << *this << " not complete: " << std::hex << status << std::endl;
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif

        // Restore earlier framebuffer.
        dnload_glBindFramebuffer(GL_FRAMEBUFFER, get_current_frame_buffer_id());
    }

    /// Destructor.
    ~FrameBuffer()
    {
#if defined(USE_LD)
        if(m_id)
        {
            glDeleteFramebuffers(1, &m_id);
        }
#if defined(VGL_DISABLE_DEPTH_TEXTURE)
        if(m_depth_buffer_id)
        {
            glDeleteRenderbuffers(1, &m_depth_buffer_id);
        }
#endif
#endif
    }

public:
    /// Bind this framebuffer to use.
    void bind() const
    {
        if(g_current_frame_buffer != this)
        {
            dnload_glBindFramebuffer(GL_FRAMEBUFFER, m_id);
            dnload_glViewport(0, 0, static_cast<GLsizei>(m_width), static_cast<GLsizei>(m_height));
            g_current_frame_buffer = this;
        }
    }

    /// Accessor.
    ///
    /// \return Framebuffer id.
    constexpr unsigned getId() const noexcept
    {
        return m_id;
    }

    /// Accessor.
    ///
    /// \return Color texture.
    constexpr const Texture2D* getTextureColor() const noexcept
    {
        return m_color_texture.get();
    }

#if defined(VGL_DISABLE_DEPTH_TEXTURE)
    /// Accessor.
    ///
    /// \return Depth renderbuffer id.
    constexpr GLuint getDepthBufferId() const noexcept
    {
        return m_depth_buffer_id;
    }
#else
    /// Accessor.
    ///
    /// \return Depth texture.
    constexpr const Texture2D* getTextureDepth() const noexcept
    {
        return m_depth_texture.get();
    }
#endif

    /// Accessor.
    ///
    /// \return Render target width.
    constexpr unsigned getWidth() const noexcept
    {
        return m_width;
    }

    /// Accessor.
    ///
    /// \return Render target height.
    constexpr unsigned getHeight() const noexcept
    {
        return m_height;
    }

private:
    /// Get current render target id.
    ///
    /// \return ID of currently bound render target or 0.
    static unsigned get_current_frame_buffer_id() noexcept
    {
        return g_current_frame_buffer->getId();
    }

public:
    /// Bind default framebuffer.
    ///
    /// \param screen_width Width of default framebuffer.
    /// \param screen_height Height of default framebuffer.
    static void bind_default_frame_buffer(unsigned screen_width, unsigned screen_height)
    {
        if(g_current_frame_buffer)
        {
            dnload_glBindFramebuffer(GL_FRAMEBUFFER, 0);
            dnload_glViewport(0, 0, static_cast<GLsizei>(screen_width), static_cast<GLsizei>(screen_height));
            g_current_frame_buffer = NULL;
        }
    }

    /// Create a new framebuffer.
    ///
    /// \param width Framebuffer width.
    /// \param height Framebuffer height.
    /// \param colorFormat Optional color format.
    /// \param depthFormat Optional depth format.
    /// \param filtering Filtering mode (default: BILINEAR).
    /// \param wrap Wrap mode (default: CLAMP).
    static unique_ptr<FrameBuffer> create(unsigned width, unsigned height, const optional<TextureFormatColor>& colorFormat,
            const optional<TextureFormatDepth>& depthFormat, FilteringMode filtering = FilteringMode::BILINEAR,
            WrapMode wrap = WrapMode::CLAMP)
    {
        return unique_ptr<FrameBuffer>(new FrameBuffer(width, height, colorFormat, depthFormat, filtering, wrap));
    }
    /// Create a new framebuffer.
    ///
    /// \param width Framebuffer width.
    /// \param height Framebuffer height.
    /// \param bpc Bytes per color channel (default: 1).
    /// \param bpd Bytes per depth channel (default: 2).
    /// \param filtering Filtering mode (default: BILINEAR).
    /// \param wrap Wrap mode (default: CLAMP).
    static unique_ptr<FrameBuffer> create(unsigned width, unsigned height, unsigned bpc = 1, unsigned bpd = 2,
            FilteringMode filtering = FilteringMode::BILINEAR, WrapMode wrap = WrapMode::CLAMP)
    {
        optional<TextureFormatColor> colorFormat;
        if(bpc)
        {
            colorFormat = TextureFormatColor(4, bpc, nullptr);
        }
        optional<TextureFormatDepth> depthFormat;
        if(bpd)
        {
            depthFormat = TextureFormatDepth(bpd);
        }
        return create(width, height, colorFormat, depthFormat, filtering, wrap);
    }

    /// Accesses the default frame buffer.
    ///
    /// \return Frame buffer reference.
    static const FrameBuffer& get_default()
    {
        return g_default_frame_buffer;
    }

    /// Initialize the default frame buffer.
    ///
    /// \param width Width.
    /// \param height Height.
    static void initialize_default(unsigned width, unsigned height)
    {
        g_default_frame_buffer.m_width = width;
        g_default_frame_buffer.m_height = height;
    }

public:
#if defined(USE_LD)
    /// Output to stream.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const FrameBuffer& rhs)
    {
        lhs << "FrameBuffer(" << rhs.m_width << "x" << rhs.m_height << (rhs.m_color_texture ? "C" : "");
#if !defined(VGL_DISABLE_DEPTH_TEXTURE)
        lhs  << (rhs.m_depth_texture ? "D" : "");
#endif
        return lhs << ")";
    }
#endif
};

const FrameBuffer* FrameBuffer::g_current_frame_buffer = &g_default_frame_buffer;
FrameBuffer FrameBuffer::g_default_frame_buffer;

/// FrameBuffer unique pointer type.
using FrameBufferUptr = unique_ptr<FrameBuffer>;

}

#endif
