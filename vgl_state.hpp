#ifndef VGL_STATE_HPP
#define VGL_STATE_HPP

#include "vgl_extern_opengl.hpp"
#include "vgl_optional.hpp"
#include "vgl_uvec4.hpp"

#if defined(USE_LD)
#include <sstream>
#endif

namespace vgl
{

/// Common enum for distinct modes.
///
/// Most functions only respond to select modes.
/// If other modes are passed, an error is thrown.
enum OperationMode
{
    /// Disable mode.
    DISABLED,

    /// Additive.
    ADDITIVE,

    /// Premultiplied alpha.
    PREMULTIPLIED,

    /// Carmack reverse.
    CARMACK,
};

#if defined(USE_LD)

/// Converts an operation mode to string.
///
/// \param op Mode.
/// \return String representation.
std::string to_string(OperationMode op);

#endif

namespace detail
{

/// OpenGL attribute array state abstraction.
class OpenGlAttribState
{
public:
    /// Maximum number of attribute arrays.
    static const unsigned MAX_ATTRIB_ARRAYS = 6u;

private:
    /// Attribute array enabled statuses.
    bool m_attrib_arrays_enabled[MAX_ATTRIB_ARRAYS] =
    {
        false,
        false,
        false,
        false,
        false,
        false,
    };

public:
    /// Global state.
    static OpenGlAttribState g_opengl_attrib_state;

public:
    /// Enable one vertex attribute.
    ///
    /// \param op Index of array to enable.
    void enableAttribArray(unsigned op)
    {
#if defined(USE_LD)
        if((MAX_ATTRIB_ARRAYS <= op))
        {
            std::ostringstream sstr;
            sstr << "enabling attribute index " << op << " (" << MAX_ATTRIB_ARRAYS << " supported)";
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif
        if(!m_attrib_arrays_enabled[op])
        {
            dnload_glEnableVertexAttribArray(op);
            m_attrib_arrays_enabled[op] = true;
        }
    }

    /// Disable extra vertex attribute arrays.
    ///
    /// \param op Index of first array to disable.
    void disableAttribArraysFrom(unsigned op)
    {
        for(unsigned ii = op; (ii < MAX_ATTRIB_ARRAYS); ++ii)
        {
            if(m_attrib_arrays_enabled[ii])
            {
                dnload_glDisableVertexAttribArray(ii);
                m_attrib_arrays_enabled[ii] = false;
            }
        }
    }
};

/// OpenGL blend state abstraction.
class OpenGlBlendState
{
private:
    /// Is blending enabled?
    bool m_blend_enabled = false;
    /// Current blend mode.
    OperationMode m_blend_mode = DISABLED;

public:
    /// Global state.
    static OpenGlBlendState g_opengl_blend_state;

public:
    /// Set blending mode.
    ///
    /// \param op New blending mode.
    void setBlendMode(OperationMode op)
    {
        if(DISABLED == op)
        {
            if(m_blend_enabled)
            {
                dnload_glDisable(GL_BLEND);
                m_blend_enabled = false;
            }
        }
        else
        {
            if(!m_blend_enabled)
            {
                dnload_glEnable(GL_BLEND);
                m_blend_enabled = true;
            }

            if(m_blend_mode != op)
            {
                if(op == ADDITIVE)
                {
                    dnload_glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
                }
                else // Default is premultiplied.
                {
#if defined(USE_LD)
                    if(op != PREMULTIPLIED)
                    {
                        BOOST_THROW_EXCEPTION(std::runtime_error("invalid blend mode: '" + to_string(op) + "'"));
                    }
#endif
                    dnload_glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                }
                m_blend_mode = op;
            }
        }
    }
};

/// OpenGL clear state abstraction.
class OpenGlClearState
{
private:
    /// Current clear color.
    uvec4 m_clear_color = uvec4(0u, 0u, 0u, 0u);
    /// Current clear depth.
    float m_clear_depth = 1.0f;
#if !defined(VGL_DISABLE_STENCIL)
    /// Current clear stencil.
    uint8_t m_clear_stencil = 0u;
#endif

public:
    /// Global state.
    static OpenGlClearState g_opengl_clear_state;

public:
    /// Clear current framebuffer.
    ///
    /// \param color Optional color clear.
    /// \param depth Optional depth clear.
#if !defined(VGL_DISABLE_STENCIL)
    /// \param stencil Optional stencil clear.
#endif
    void clearBuffers(optional<uvec4> color, optional<float> depth
#if !defined(VGL_DISABLE_STENCIL)
            , optional<uint8_t> stencil
#endif
            )
    {
        GLbitfield clear_mask = 0;

        if(color)
        {
            if(*color != m_clear_color)
            {
                vec4 norm_color = color->toNormVec4();
                dnload_glClearColor(norm_color.x(), norm_color.y(), norm_color.z(), norm_color.w());
                m_clear_color = *color;
            }
            clear_mask |= GL_COLOR_BUFFER_BIT;
        }

        if(depth)
        {
            if(*depth != m_clear_depth)
            {
                dnload_glClearDepthf(*depth);
                m_clear_depth = *depth;
            }
            clear_mask |= GL_DEPTH_BUFFER_BIT;
        }

#if !defined(VGL_DISABLE_STENCIL)
        if(stencil)
        {
            if(*stencil != m_clear_stencil)
            {
                dnload_glClearStencil(*stencil);
                m_clear_stencil = *stencil;
            }
            clear_mask |= GL_STENCIL_BUFFER_BIT;
        }
#endif

        dnload_glClear(clear_mask);
    }
};

/// OpenGL color write state abstraction.
class OpenGlColorWriteState
{
private:
    /// Current color write state.
    bool m_color_write = true;

public:
    /// Global state.
    static OpenGlColorWriteState g_opengl_color_write_state;

public:
    /// Set color writing mode.
    ///
    /// \param op True to enable, false to disable.
    void setColorWrite(bool op)
    {
        if(op != m_color_write)
        {
            GLboolean mask_value = op ? GL_TRUE : GL_FALSE;
            dnload_glColorMask(mask_value, mask_value, mask_value, mask_value);
            m_color_write = op;
        }
    }
};

/// OpenGL cull face state abstraction.
class OpenGlCullFaceState
{
private:
    /// Is face culling enabled.
    bool m_cull_face_enabled = false;
    /// Current face cull mode.
    GLenum m_cull_face = GL_BACK;

public:
    /// Global state.
    static OpenGlCullFaceState g_opengl_cull_face_state;

public:
    /// Set culling mode.
    ///
    /// \param op Mode, GL_FALSE to disable.
    void setCullFace(GLenum op)
    {
        if(GL_FALSE == op)
        {
            if(m_cull_face_enabled)
            {
                dnload_glDisable(GL_CULL_FACE);
                m_cull_face_enabled = false;
            }
        }
        else
        {
            if(!m_cull_face_enabled)
            {
                dnload_glEnable(GL_CULL_FACE);
                m_cull_face_enabled = true;
            }

            if(m_cull_face != op)
            {
                dnload_glCullFace(op);
                m_cull_face = op;
            }
        }
    }
};

/// OpenGL depth state abstraction.
class OpenGlDepthState
{
private:
    /// Is depth test enabled?
    bool m_depth_test = false;
    /// Current depth test function.
    GLenum m_depth_func = GL_LESS;
#if !defined(VGL_DISABLE_DEPTH_WRITE)
    /// Is depth write enabled?
    bool m_depth_write = true;
#endif

public:
    /// Global state.
    static OpenGlDepthState g_opengl_depth_state;

public:
    /// Set depth testing mode.
    ///
    /// \param op Depth testing mode, GL_FALSE to disable.
    void setDepthTest(GLenum op)
    {
        if(GL_FALSE == op)
        {
            if(m_depth_test)
            {
                dnload_glDisable(GL_DEPTH_TEST);
                m_depth_test = false;
            }
        }
        else
        {
            if(!m_depth_test)
            {
                dnload_glEnable(GL_DEPTH_TEST);
                m_depth_test = true;
            }

            if(m_depth_func != op)
            {
                dnload_glDepthFunc(op);
                m_depth_func = op;
            }
        }
    }

#if !defined(VGL_DISABLE_DEPTH_WRITE)

    /// Set depth writing mode.
    ///
    /// \param op True to enable, false to disable.
    void setDepthWrite(bool op)
    {
        if(op != m_depth_write)
        {
            GLboolean mask_value = op ? GL_TRUE : GL_FALSE;
            dnload_glDepthMask(mask_value);
            m_depth_write = op;
        }
    }

#endif
};

#if !defined(VGL_DISABLE_POLYGON_OFFSET)

/// OpenGL polygon offset state.
class OpenGlPolygonOffsetState
{
private:
    /// Is polygon offset enabled?
    bool m_polygon_offset = false;
    /// Current polygon offset in units.
    int m_polygon_offset_units = 0;

public:
    /// Global state.
    static OpenGlPolygonOffsetState g_opengl_polygon_offset_state;

public:
    /// Set polygon offset.
    ///
    /// \param op Units to offset.
    void setPolygonOffset(int op)
    {
        if(!op)
        {
            if(m_polygon_offset)
            {
                dnload_glDisable(GL_POLYGON_OFFSET_FILL);
                m_polygon_offset = false;
            }
        }
        else
        {
            if(!m_polygon_offset)
            {
                dnload_glEnable(GL_POLYGON_OFFSET_FILL);
                m_polygon_offset = true;
            }
            if(m_polygon_offset_units != op)
            {
                dnload_glPolygonOffset(1.0f, static_cast<float>(op));
                m_polygon_offset_units = op;
            }
        }
    }
};

#endif

#if !defined(VGL_DISABLE_STENCIL)

/// OpenGl stencil state abstraction.
class OpenGlStencilState
{
private:
    /// Current stencil function.
    GLenum m_stencil_func = GL_FALSE;
    /// Current stencil operation mode.
    OperationMode m_stencil_operation = DISABLED;
    /// Is stencil test enabled?
    bool m_stencil_test = false;

public:
    /// Global state.
    static OpenGlStencilState g_opengl_stencil_state;

public:
    /// Set stencil mode.
    ///
    /// \param op New mode.
    void setStencilMode(GLenum op)
    {
        if(GL_FALSE == op)
        {
            if(m_stencil_test)
            {
                dnload_glDisable(GL_STENCIL_TEST);
                m_stencil_test = false;
            }
        }
        else
        {
            if(!m_stencil_test)
            {
                dnload_glEnable(GL_STENCIL_TEST);
                m_stencil_test = true;
            }
            if(m_stencil_func != op)
            {
                dnload_glStencilFunc(op, 0, 0xFFFFFFFFu);
                m_stencil_func = op;
            }
        }
    }

    /// Set stencil operation.
    ///
    /// \param op New mode.
    void setStencilOperation(OperationMode op)
    {
        if(m_stencil_operation != op)
        {
            if(op == CARMACK)
            {
                dnload_glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
                dnload_glStencilOpSeparate(GL_BACK, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
            }
            else // Default is nothing.
            {
#if defined(USE_LD)
                if(op != DISABLED)
                {
                    BOOST_THROW_EXCEPTION(std::runtime_error("invalid stencil operation: '" + to_string(op) + "'"));
                }
#endif
                dnload_glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
            }
            m_stencil_operation = op;
        }
    }
};

#endif

#if defined(USE_LD)

/// OpenGL state abstraction.
class OpenGlDiagnosticsState
{
private:
#if !defined(VGL_DISABLE_EDGE)
    /// Total GPU data size spent on edges.
    unsigned m_data_size_edge = 0u;
#endif
    /// Total GPU data size spent on indices.
    unsigned m_data_size_index = 0u;
    /// Total GPU data size spent on textures.
    unsigned m_data_size_texture = 0u;
    /// Total GPU data size spent on vertices.
    unsigned m_data_size_vertex = 0u;

public:
    /// Global state.
    static OpenGlDiagnosticsState g_opengl_diagnostics_state;

public:

#if !defined(VGL_DISABLE_EDGE)

    /// Accessor.
    ///
    /// \return Total edge data size used.
    constexpr unsigned getDataSizeEdge() const
    {
        return m_data_size_edge;
    }
    /// Increment data size.
    ///
    /// \param op Edge data size used.
    constexpr unsigned incrementDataSizeEdge(unsigned op)
    {
        return m_data_size_edge += op;
    }

#endif

    /// Accessor.
    ///
    /// \return Total index data size used.
    constexpr unsigned getDataSizeIndex() const
    {
        return m_data_size_index;
    }
    /// Increment data size.
    ///
    /// \param op Index data size used.
    constexpr unsigned incrementDataSizeIndex(unsigned op)
    {
        return m_data_size_index += op;
    }

    /// Accessor.
    ///
    /// \return Total index data size used.
    constexpr unsigned getDataSizeTexture() const
    {
        return m_data_size_texture;
    }
    /// Increment data size.
    ///
    /// \param op Texture data size used.
    constexpr unsigned incrementDataSizeTexture(unsigned op)
    {
        return m_data_size_texture += op;
    }

    /// Accessor.
    ///
    /// \return Total vertex data size used.
    constexpr unsigned getDataSizeVertex() const
    {
        return m_data_size_vertex;
    }
    /// Increment data size.
    ///
    /// \param op Edge data size used.
    constexpr unsigned incrementDataSizeVertex(unsigned op)
    {
        return m_data_size_vertex += op;
    }
};

#endif

}

/// Enable one vertex attribute.
///
/// \param op Index of array to enable.
inline void attrib_array_enable(unsigned op)
{
    detail::OpenGlAttribState::g_opengl_attrib_state.enableAttribArray(op);
}

/// Disable extra vertex attribute arrays.
///
/// \param op Index of first array to disable.
inline void attrib_array_disable_from(unsigned op)
{
    detail::OpenGlAttribState::g_opengl_attrib_state.disableAttribArraysFrom(op);
}

/// Set blending mode.
///
/// \param op New blending mode.
inline void blend_mode(OperationMode op)
{
    detail::OpenGlBlendState::g_opengl_blend_state.setBlendMode(op);
}

/// Clear current framebuffer.
///
/// \param color Optional color clear.
/// \param depth Optional depth clear.
#if !defined(VGL_DISABLE_STENCIL)
/// \param stencil Optional stencil clear.
#endif
inline void clear_buffers(optional<uvec4> color, optional<float> depth
#if !defined(VGL_DISABLE_STENCIL)
        , optional<uint8_t> stencil = nullopt
#endif
        )
{
    detail::OpenGlClearState::g_opengl_clear_state.clearBuffers(color, depth
#if !defined(VGL_DISABLE_STENCIL)
            , stencil
#endif
            );
}

/// Set color writing mode.
///
/// \param op True to enable, false to disable.
inline void color_write(bool op)
{
    detail::OpenGlColorWriteState::g_opengl_color_write_state.setColorWrite(op);
}

/// Set culling mode.
///
/// \param op Mode, GL_FALSE to disable.
inline void cull_face(GLenum op)
{
    detail::OpenGlCullFaceState::g_opengl_cull_face_state.setCullFace(op);
}

/// Set depth testing mode.
///
/// \param op Depth testing mode, GL_FALSE to disable.
inline void depth_test(GLenum op)
{
    detail::OpenGlDepthState::g_opengl_depth_state.setDepthTest(op);
}

#if !defined(VGL_DISABLE_DEPTH_WRITE)

/// Set depth writing mode.
///
/// \param op True to enable, false to disable.
inline void depth_write(bool op)
{
    detail::OpenGlDepthState::g_opengl_depth_state.setDepthWrite(op);
}

#endif

#if !defined(VGL_DISABLE_POLYGON_OFFSET)

/// Set polygon offset.
///
/// \param op Units to offset.
inline void polygon_offset(int op)
{
    detail::OpenGlPolygonOffsetState::g_opengl_polygon_offset_state.setPolygonOffset(op);
}

#endif

#if !defined(VGL_DISABLE_STENCIL)

/// Set stencil mode.
///
/// \param op New mode.
inline void stencil_mode(GLenum op)
{
    detail::OpenGlStencilState::g_opengl_stencil_state.setStencilMode(op);
}

/// Set stencil operation.
///
/// \param op New mode.
inline void stencil_operation(OperationMode op)
{
    detail::OpenGlStencilState::g_opengl_stencil_state.setStencilOperation(op);
}

#endif

#if defined(USE_LD)

#if !defined(VGL_DISABLE_EDGE)

/// Accessor.
///
/// \return Total edge data size used.
constexpr unsigned get_data_size_edge()
{
    return detail::OpenGlDiagnosticsState::g_opengl_diagnostics_state.getDataSizeEdge();
}
/// Increment data size.
///
/// \param op Edge data size used.
constexpr unsigned increment_data_size_edge(unsigned op)
{
    return detail::OpenGlDiagnosticsState::g_opengl_diagnostics_state.incrementDataSizeEdge(op);
}

#endif

/// Accessor.
///
/// \return Total index data size used.
constexpr unsigned get_data_size_index()
{
    return detail::OpenGlDiagnosticsState::g_opengl_diagnostics_state.getDataSizeIndex();
}
/// Increment data size.
///
/// \param op Index data size used.
constexpr unsigned increment_data_size_index(unsigned op)
{
    return detail::OpenGlDiagnosticsState::g_opengl_diagnostics_state.incrementDataSizeIndex(op);
}

/// Accessor.
///
/// \return Total index data size used.
constexpr unsigned get_data_size_texture()
{
    return detail::OpenGlDiagnosticsState::g_opengl_diagnostics_state.getDataSizeTexture();
}
/// Increment data size.
///
/// \param op Texture data size used.
constexpr unsigned increment_data_size_texture(unsigned op)
{
    return detail::OpenGlDiagnosticsState::g_opengl_diagnostics_state.incrementDataSizeTexture(op);
}

/// Accessor.
///
/// \return Total vertex data size used.
constexpr unsigned get_data_size_vertex()
{
    return detail::OpenGlDiagnosticsState::g_opengl_diagnostics_state.getDataSizeVertex();
}
/// Increment data size.
///
/// \param op Edge data size used.
constexpr unsigned increment_data_size_vertex(unsigned op)
{
    return detail::OpenGlDiagnosticsState::g_opengl_diagnostics_state.incrementDataSizeVertex(op);
}

/// Get an error string corresponding to a GL error.
///
/// \param op GL error.
/// \return Error string.
constexpr const char* gl_error_string(GLenum err)
{
    switch(err)
    {
    case GL_NO_ERROR:
        return "GL_NO_ERROR";

    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";

    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";

    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";

    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";

    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";

    default:
        break;
    }

    return "unknown error";
}

/// Gets GL string in safe manner.
///
/// If the returned string is nullptr, returns an empty string.
///
/// \param op String enumeration to get.
/// \return Result of glGetString() packed into a string.
std::string gl_get_string(GLenum op);

/// Gets OpenGL extension string.
///
/// \param align Lines are at most this long (0: infinite).
/// \param indent Lines after the first one have this many spaces as indentation.
/// \return Result string.
std::string gl_extension_string(unsigned align = 78, unsigned indent = 0);

/// Gets OpenGL vendor string.
///
/// \return OpenGL vendor.
std::string gl_vendor_string();

/// Gets OpenGL version string.
///
/// \return OpenGL version.
std::string gl_version_string();

/// Perform error check.
///
/// Throws an error on failure.
void error_check(const char* str = NULL);

#endif

}

#if !defined(USE_LD)
#include "vgl_state.cpp"
#endif

#endif
