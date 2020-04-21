#ifndef VGL_GLSL_PROGRAM_HPP
#define VGL_GLSL_PROGRAM_HPP

#include "vgl_glsl_shader.hpp"
#include "vgl_utility.hpp"
#include "vgl_vec3.hpp"
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
    string m_name;

    /// Location
    GLint m_location;

protected:
    /// Constructor.
    ///
    /// \param name Name for the location info.
    /// \param location Location.
    explicit GlslLocation(string_view name, GLint location) :
        m_name(name),
        m_location(location)
    {
    }

public:
    /// Accessor.
    ///
    /// \return Name.
    constexpr const string& getName() const
    {
        return m_name;
    }

    /// Accessor.
    ///
    /// \return Location.
    constexpr GLint getLocation() const
    {
        return m_location;
    }

#if defined(USE_LD)
    /// Is this location entry valid?
    ///
    /// \return True if location >= 0, false otherwise.
    constexpr bool isValid() const
    {
        return (m_location >= 0);
    }
#endif
};

/// GLSL attribute info.
class GlslAttribute : public GlslLocation
{
public:
    /// Constructor.
    ///
    /// \param program Program to look from.
    /// \param name Name for the location info.
    explicit GlslAttribute(GLuint program, string_view name) :
        GlslLocation(name, lookup(program, name))
    {
    }

private:
    /// Do lookup for given name.
    ///
    /// \param program Program to look from.
    /// \param name Name for the location info.
    static GLint lookup(GLuint program, string_view name)
    {
        return dnload_glGetAttribLocation(program, name.data());
    }

public:
#if defined(USE_LD)
    /// Refresh name
    ///
    /// \param op Program ID to refresh for.
    void refresh(GLuint op)
    {
        m_location = lookup(op, m_name);
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
    explicit GlslUniform(GLuint program, string_view name) :
        GlslLocation(name, lookup(program, name))
    {
    }

private:
    /// Do lookup for given name.
    ///
    /// \param program Program to look from.
    /// \param name Name for the location info.
    static GLint lookup(GLuint program, string_view name)
    {
        return dnload_glGetUniformLocation(program, name.data());
    }

public:
#if defined(USE_LD)
    /// Refresh name
    ///
    /// \param op Program ID to refresh for.
    void refresh(GLuint op)
    {
        m_location = lookup(op, m_name);
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

    /// Get an attribute location.
    ///
    /// \param name Name to check.
    GLint getAttribLocation(string_view name)
    {
        for(const auto& vv : m_attributes)
        {
            if(vv.getName() == name)
            {
                return vv.getLocation();
            }
        }
        m_attributes.emplace_back(m_id, name);
        detail::GlslAttribute& ret = m_attributes.back();
#if defined(USE_LD)
        if(!ret.isValid())
        {
            std::cerr << "WARNING: program " << m_id << " has no attribute " << name.data() << std::endl;
        }
#endif
        return ret.getLocation();
    }

    /// Get an uniform location.
    ///
    /// \param name Name to check.
    GLint getUniformLocation(string_view name)
    {
        for(const auto& vv : m_uniforms)
        {
            if(vv.getName() == name)
            {
                return vv.getLocation();
            }
        }
        m_uniforms.emplace_back(m_id, name);
        detail::GlslUniform& ret = m_uniforms.back();
#if defined(USE_LD)
        if(!ret.isValid())
        {
            std::cerr << "WARNING: program " << m_id << " has no uniform " << name.data() << std::endl;
        }
#endif
        return ret.getLocation();
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

public:
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
