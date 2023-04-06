#ifndef VGL_GLSL_SHADER_HPP
#define VGL_GLSL_SHADER_HPP

#include "vgl_extern_opengl.hpp"
#include "vgl_string_view.hpp"

#if defined(USE_LD)
#include "vgl_vector.hpp"
#include "vgl_wave.hpp"
#include <iostream>
#endif

namespace vgl
{

namespace detail
{

#if defined(USE_LD)

/// Get shader info log.
///
/// \param op Shader ID.
/// \return Info string.
string get_shader_info_log(GLuint op);

/// Get compile status for a shader.
///
/// \param op Shader ID.
/// \return True if compilation was successful, false otherwise.
bool get_shader_compile_status(GLuint op);

#endif

}

/// GLSL shader.
class GlslShader
{
private:
#if defined(USE_LD)
    /// Vector containing all source files.
    vector<string> m_files;
#endif

    /// OpenGL type.
    GLenum m_type = 0;

    /// OpenGL ID.
    GLuint m_id = 0;

private:
    /// Deleted copy constructor.
    GlslShader(const GlslShader&) = delete;
    /// Deleted assignment.
    GlslShader& operator=(const GlslShader&) = delete;

public:
    /// Constructor.
    constexpr explicit GlslShader() noexcept = default;

    /// Constructor with source.
    ///
    /// \param type Shader type.
    /// \param op Source input.
    explicit GlslShader(GLenum type, string_view op) :
        m_type(type),
        m_id(compile(op))
    {
    }

    /// Constructor with two-part source.
    ///
    /// \param type Shader type.
    /// \param op1 Source input.
    /// \param op2 Source input.
    explicit GlslShader(GLenum type, string_view op1, string_view op2) :
        m_type(type),
        m_id(compile(op1, op2))
    {
    }

    /// Move constructor.
    ///
    /// \param op Source shader.
    constexpr GlslShader(GlslShader&& op) noexcept :
#if defined(USE_LD)
        m_files(move(op.m_files)),
#endif
        m_type(op.m_type),
        m_id(op.m_id)
    {
        op.m_id = 0;
    }

    /// Destructor.
    ~GlslShader()
    {
        destruct();
    }

private:
    /// Compile the shader.
    ///
    /// \param type Shader type.
    /// \param sources Table of source files.
    /// \param count Number of source files.
    /// \return Compiled source.
    static GLuint compile(GLenum type, const GLchar** sources, unsigned count)
    {
        GLuint id = dnload_glCreateShader(type);
        dnload_glShaderSource(id, static_cast<GLsizei>(count), sources, NULL);
        dnload_glCompileShader(id);
#if defined(USE_LD)
        static const char* shader_delim = "----\n";
        if(!detail::get_shader_compile_status(id))
        {
            for(unsigned ii = 0; (ii < count); ++ii)
            {
                std::cout << shader_delim << sources[ii];
            }
            std::cout << ((count > 0) ? shader_delim : "") << detail::get_shader_info_log(id) << shader_delim;
            glDeleteShader(id);
            return 0;
        }
#endif
        return id;
    }

private:
    /// Destroy the shader ID.
    void destruct()
    {
        if(m_id)
        {
            dnload_glDeleteShader(m_id);
        }
    }

    /// Compile shader from two parts.
    ///
    /// \param op Source input.
    /// \return Compiled source.
    GLuint compile(string_view op)
    {
#if defined(USE_LD)
        m_files.clear();
        m_files.emplace_back(op);
        return compile(m_files);
#else
        const GLchar* source[1] =
        {
            op.data()
        };
        return compile(m_type, source, 1);
#endif
    }

    /// Compile shader from two parts.
    ///
    /// \param op1 Source input.
    /// \param op2 Source input.
    /// \return Compiled source.
    GLuint compile(string_view op1, string_view op2)
    {
#if defined(USE_LD)
        m_files.clear();
        m_files.emplace_back(op1);
        m_files.emplace_back(op2);
        return compile(m_files);
#else
        const GLchar* source[2] =
        {
            op1.data(), op2.data()
        };
        return compile(m_type, source, 2);
#endif
    }

#if defined(USE_LD)
    /// Compile from a source file vector.
    ///
    /// \param source_files Source files.
    /// \return Compiled source.
    GLuint compile(const vector<string>& source_files)
    {
        vector<string> sources;
        for(const auto& fname : source_files)
        {
            sources.emplace_back(wave_preprocess_glsl(fname.data()).data());
        }
        vector<const GLchar*> cstr_source;
        for(const auto& vv : sources)
        {
            cstr_source.push_back(vv.c_str());
        }
        return compile(m_type, cstr_source.data(), cstr_source.size());
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

#if defined(USE_LD)
    /// Recompile from existing source files.
    ///
    /// \return Compiled source.
    bool recompile()
    {
        destruct();
        m_id = compile(m_files);
        return (m_id != 0);
    }
#endif

public:
    /// Move operator.
    ///
    /// \param op Source shader.
    /// \return This object.
    constexpr GlslShader& operator=(GlslShader&& op) noexcept
    {
#if defined(USE_LD)
        m_files = move(op.m_files);
#endif
        m_type = op.m_type;
        m_id = op.m_id;
        op.m_id = 0;
        return *this;
    }
};

}

#endif
