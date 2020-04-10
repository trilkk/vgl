#ifndef VGL_GLSL_PROGRAM_HPP
#define VGL_GLSL_PROGRAM_HPP

#include "vgl_glsl_shader.hpp"
#include "vgl_utility.hpp"

namespace vgl
{

namespace detail
{

#if defined(USE_LD)
/// Get program info log.
///
/// \param op Program ID.
/// \return Info string.
string get_program_info_log(GLuint op)
{
    GLint len;

    glGetProgramiv(op, GL_INFO_LOG_LENGTH, &len);
    if(!len)
    {
        return string();
    }

    GLsizei acquired;
    string ret;
    ret.resize(len);
    glGetProgramInfoLog(op, len, &acquired, const_cast<GLchar*>(ret.data()));
    return ret;
}

/// Get link status for a program.
///
/// \param op Program ID.
/// \return True if link successful, false otherwise.
bool get_program_link_status(GLuint op)
{
    GLint ret;
    glGetProgramiv(op, GL_LINK_STATUS, &ret);
    return (GL_FALSE != ret);
}
#endif

}

/// GLSL program.
class GlslProgram
{
private:
    /// Vertex shader.
    GlslShader m_vert;

    /// Fragment shader.
    GlslShader m_frag;

    /// Program ID.
    GLuint m_id = 0;

public:
    /// Default constructor.
    constexpr explicit GlslProgram() noexcept = default;

    /// Constructor from shaders.
    ///
    /// \param vert Vertex shader.
    /// \param frag Fragment shader.
    explicit GlslProgram(GlslShader&& vert, GlslShader&& frag) :
        m_vert(move(vert)),
        m_frag(move(frag)),
        m_id(link())
    {
    }

    /// Move constructor.
    ///
    /// \param op Source program.
    GlslProgram(GlslProgram&& op) :
        m_vert(move(op.m_vert)),
        m_frag(move(op.m_frag)),
        m_id(op.m_id)
    {
        op.m_id = 0;
    }

    /// Destructor.
    ~GlslProgram()
    {
        destruct();
    }

private:
    /// Link the program.
    static GLuint link(GLuint vert, GLuint frag)
    {
        GLuint id = dnload_glCreateProgram();
        dnload_glAttachShader(id, vert);
        dnload_glAttachShader(id, frag);
        dnload_glLinkProgram(id);
#if defined(USE_LD)
        if(!detail::get_program_link_status(id))
        {
            std::cout << detail::get_program_info_log(id);
            glDeleteShader(id);
            return 0;
        }
#endif
        return id;
    }

private:
    /// Destroy the program ID.
    void destruct()
    {
        if(m_id)
        {
            dnload_glDeleteProgram(m_id);
        }
    }

    /// Link the program.
    GLuint link()
    {
        return link(m_vert.getId(), m_frag.getId());
    }

    /// Check for shader statuses and link.
    void linkCheck()
    {
        GLuint vid = m_vert.getId();
        GLuint fid = m_frag.getId();
        if(vid && fid)
        {
            destruct();
            m_id = link(vid, fid);
        }
    }

    /// Select shader location for assignment.
    ///
    /// \param type Shader type.
    GlslShader& selectShaderAssignment(GLenum type)
    {
        if(type == GL_VERTEX_SHADER)
        {
            return m_vert;
        }
#if defined(USE_LD)
        if(type != GL_FRAGMENT_SHADER)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("invalid shader type: " + std::to_string(type)));
        }
#endif
        return m_frag;
    }

public:
    /// Accessor.
    ///
    /// \return OpenGL ID.
    constexpr GLuint getId() const
    {
        return m_id;
    }

    /// Gets the uniform location for given uniform in this program.
    ///
    /// \param op Name of the uniform.
    /// \return Uniform location.
    GLint getUniformLocation(const char* op) const
    {
        return dnload_glGetUniformLocation(m_id, op);
    }
    /// Gets the uniform location for given uniform in this program.
    ///
    /// \param op Name of the uniform.
    /// \return Uniform location.
    GLint getUniformLocation(string_view op) const
    {
        return getUniformLocation(op.data());
    }

    /// Bind for use.
    void bind() const
    {
        dnload_glUseProgram(m_id);
    }

    /// Set shader.
    ///
    /// Implictly links program if both shaders are set.
    ///
    /// \param type Shader type.
    /// \param src Source input.
    void setShader(GLenum type, string_view src)
    {
        GlslShader& dst = selectShaderAssignment(type);
        dst = GlslShader(type, src);

        linkCheck();
    }
    /// Set shader.
    ///
    /// Implictly links program if both shaders are set.
    ///
    /// \param type Shader type.
    /// \param src1 Source input.
    /// \param src2 Source input.
    void setShader(GLenum type, string_view src1, string_view src2)
    {
        GlslShader& dst = selectShaderAssignment(type);
        dst = GlslShader(type, src1, src2);

        linkCheck();
    }

#if defined(USE_LD)
    /// Re-link the program.
    bool relink()
    {
        m_vert.recompile();
        m_frag.recompile();
        linkCheck();
        return (m_id != 0);
    }
#endif

public:
    /// Move operator.
    ///
    /// \param op Source shader.
    /// \return This object.
    GlslProgram& operator=(GlslProgram&& op) noexcept
    {
        m_vert = move(op.m_vert);
        m_frag = move(op.m_frag);
        m_id = op.m_id;
        op.m_id = 0;
        return *this;
    }
};

}

#endif
