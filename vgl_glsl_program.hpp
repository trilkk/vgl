#ifndef VGL_GLSL_PROGRAM_HPP
#define VGL_GLSL_PROGRAM_HPP

#include "vgl_geometry_channel.hpp"
#include "vgl_glsl_shader.hpp"
#include "vgl_utility.hpp"
#include "vgl_mat2.hpp"
#include "vgl_mat4.hpp"
#include "vgl_vec2.hpp"
#include "vgl_vec3.hpp"
#include "vgl_vec4.hpp"
#include "vgl_vector.hpp"

namespace vgl
{

namespace detail
{

/// Glsl name <-> locatioin pair.
class GlslLocation
{
protected:
    /// Name of the uniform.
    string_view m_name;

    /// Location
    GLint m_location;

protected:
    /// Constructor.
    ///
    /// \param name Name for the location info.
    /// \param location Location.
    explicit GlslLocation(const char* name, GLint location) :
        m_name(name),
        m_location(location)
    {
    }

public:
    /// Accessor.
    ///
    /// \return Name.
    constexpr string_view getName() const noexcept
    {
        return m_name;
    }

    /// Accessor.
    ///
    /// \return Location.
    constexpr GLint getLocation() const noexcept
    {
        return m_location;
    }

#if defined(USE_LD)
    /// Is this location entry valid?
    ///
    /// \return True if location >= 0, false otherwise.
    constexpr bool isValid() const noexcept
    {
        return (m_location >= 0);
    }
#endif
};

/// GLSL attribute info.
class GlslAttribute : public GlslLocation
{
private:
    /// Channel semantic.
    GeometryChannel m_channel;

public:
    /// Constructor.
    ///
    /// \param program Program to look from.
    /// \param name Name for the location info.
    explicit GlslAttribute(GLuint program, GeometryChannel channel, const char* name) :
        GlslLocation(name, lookup(program, name)),
        m_channel(channel)
    {
    }

private:
    /// Do lookup for given name.
    ///
    /// \param program Program to look from.
    /// \param name Name for the location info.
    static GLint lookup(GLuint program, const char* name)
    {
        return dnload_glGetAttribLocation(program, name);
    }

public:
    /// Accessor.
    ///
    /// \return Channel semantic.
    constexpr GeometryChannel getChannel() const noexcept
    {
        return m_channel;
    }

#if defined(USE_LD)
    /// Refresh name
    ///
    /// \param op Program ID to refresh for.
    void refresh(GLuint op)
    {
        m_location = lookup(op, m_name.data());
    }
#endif
};

/// GLSL uniform info.
class GlslUniform : public GlslLocation
{
public:
    /// Constructor.
    ///
    /// \param program Program to look from.
    /// \param name Name for the location info.
    explicit GlslUniform(GLuint program, const char* name) :
        GlslLocation(name, lookup(program, name))
    {
    }

private:
    /// Do lookup for given name.
    ///
    /// \param program Program to look from.
    /// \param name Name for the location info.
    static GLint lookup(GLuint program, const char* name)
    {
        return dnload_glGetUniformLocation(program, name);
    }

public:
#if defined(USE_LD)
    /// Refresh name
    ///
    /// \param op Program ID to refresh for.
    void refresh(GLuint op)
    {
        m_location = lookup(op, m_name.data());
    }
#endif
};

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

    /// Uniform mapping.
    vector<detail::GlslAttribute> m_attributes;

    /// Uniform mapping.
    vector<detail::GlslUniform> m_uniforms;

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

    /// Constructor from strings.
    ///
    /// \param vert Vertex shader.
    /// \param frag Fragmment shader.
    explicit GlslProgram(const char* vert, const char* frag) :
        m_vert(GL_VERTEX_SHADER, vert),
        m_frag(GL_FRAGMENT_SHADER, frag),
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

    /// Get an uniform location.
    ///
    /// \param op Name.
    GLint getUniformLocation(string_view op) const
    {
        for(const auto& vv : m_uniforms)
        {
            if(vv.getName() == op)
            {
                return vv.getLocation();
            }
        }
#if defined(USE_LD)
        std::cerr << "WARNING: program " << m_id << " has no uniform " << op << std::endl;
#endif
        return -1;
    }

#if defined(USE_LD)
    /// Refresh attribute and uniform locations.
    void refreshLocations()
    {
        for(auto& vv : m_attributes)
        {
            vv.refresh(m_id);
        }

        for(auto& vv : m_uniforms)
        {
            vv.refresh(m_id);
        }
    }
#endif

public:
    /// Accessor.
    ///
    /// \return OpenGL ID.
    constexpr GLuint getId() const
    {
        return m_id;
    }

    /// Add an attribute.
    ///
    /// \param channel Semantic channel associated with the attribute.
    /// \param name Name of the attribute.
    void addAttribute(GeometryChannel channel, const char* name)
    {
#if defined(USE_LD) && defined(DEBUG)
        for(const auto& vv : m_attributes)
        {
            if(vv.getChannel() == channel)
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("trying to add multiple channels of of type " +
                            to_string(channel) + " into the same program"));
            }
        }
#endif
        m_attributes.emplace_back(m_id, channel, name);
#if defined(USE_LD)
        if(!m_attributes.back().isValid())
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("cannot add uniform " + std::string(name) + " to program " +
                       std::to_string(m_id)));
        }
#endif
    }

    /// Get an attribute location.
    ///
    /// \param op Channel semantic.
    GLint getAttribLocation(GeometryChannel op) const
    {
        for(const auto& vv : m_attributes)
        {
            if(vv.getChannel() == op)
            {
                return vv.getLocation();
            }
        }
        return -1;
    }

    /// Add an uniform.
    ///
    /// \param name Name of the uniform.
    void addUniform(const char* name)
    {
        m_uniforms.emplace_back(m_id, name);
#if defined(USE_LD)
        if(!m_attributes.back().isValid())
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("cannot add uniform " + std::string(name) + " to program " +
                       std::to_string(m_id)));
        }
#endif
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
        if(m_id != 0)
        {
            refreshLocations();
            return true;
        }
        return false;
    }
#endif

    /// Feed uniform to the program.
    ///
    /// \param name Uniform name.
    /// \param value Uniform value.
    void uniform(string_view name, const vec2& value)
    {
        GLint location = getUniformLocation(name);
        dnload_glUniform2fv(location, 1, value.data());
    }

    /// Feed uniform to the program.
    ///
    /// \param name Uniform name.
    /// \param value Uniform value.
    void uniform(string_view name, const vec3& value)
    {
        GLint location = getUniformLocation(name);
        dnload_glUniform3fv(location, 1, value.data());
    }

    /// Feed uniform to the program.
    ///
    /// \param name Uniform name.
    /// \param value Uniform value.
    void uniform(string_view name, const vec4& value)
    {
        GLint location = getUniformLocation(name);
        dnload_glUniform4fv(location, 1, value.data());
    }

    /// Feed uniform to the program.
    ///
    /// \param name Uniform name.
    /// \param value Uniform value.
    void uniform(string_view name, const mat2& value)
    {
        GLint location = getUniformLocation(name);
        dnload_glUniformMatrix2fv(location, 1, GL_FALSE, value.data());
    }

    /// Feed uniform to the program.
    ///
    /// \param name Uniform name.
    /// \param value Uniform value.
    void uniform(string_view name, const mat3& value)
    {
        GLint location = getUniformLocation(name);
        dnload_glUniformMatrix3fv(location, 1, GL_FALSE, value.data());
    }

    /// Feed uniform to the program.
    ///
    /// \param name Uniform name.
    /// \param value Uniform value.
    void uniform(string_view name, const mat4& value)
    {
        GLint location = getUniformLocation(name);
        dnload_glUniformMatrix4fv(location, 1, GL_FALSE, value.data());
    }

    /// Feed uniform to the program.
    ///
    /// \param ptr Pointer to uniform data.
    /// \param count Number of entries.
    void uniform(string_view name, const vec3* ptr, unsigned count)
    {
        GLint location = getUniformLocation(name);
        dnload_glUniform3fv(location, count, ptr->data());
    }

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
