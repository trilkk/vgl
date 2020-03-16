#ifndef VGL_HPP
#define VGL_HPP

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

    /// Premultiplied alpha.
    PREMULTIPLIED,

    /// Alpha.
    ALPHA,

    /// Carmack reverse.
    CARMACK,
};

#if defined(USE_LD)

/// Converts an operation mode to string.
///
/// \param op Mode.
/// \return String representation.
std::string to_string(OperationMode op)
{
    switch(op)
    {
    case DISABLED:
        return std::string("DISABLED");

    case PREMULTIPLIED:
        return std::string("PREMULTIPLIED");

    case ALPHA:
        return std::string("ALPHA");

    case CARMACK:
        return std::string("CARMACK");

    default:
        break;
    }

    std::ostringstream sstr;
    sstr << "invalid OperationMode value: '" << static_cast<int>(op) << "'";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
}

#endif

namespace detail
{
 
/// Maximum number of attribute arrays.
const GLint MAX_ATTRIB_ARRAYS = 6;

/// Attribute array enabled statuses.
bool g_attrib_arrays_enabled[MAX_ATTRIB_ARRAYS] =
{
    false,
    false,
    false,
    false,
    false,
    false,
};

/// Is blending enabled?
bool g_blend_enabled = false;
/// Current blend mode.
OperationMode g_blend_mode = DISABLED;
/// Current clear color.
uvec4 g_clear_color(0, 0, 0, 0);
/// Current clear depth.
float g_clear_depth = 1.0f;
/// Current clear stencil.
uint8_t g_clear_stencil = 0u;
/// Current color write state.
bool g_color_write = true;
/// Is face culling enabled.
bool g_cull_face_enabled = false;
/// Current face cull mode.
GLenum g_cull_face = GL_BACK;
/// Is depth test enabled?
bool g_depth_test = false;
/// Current depth test function.
GLenum g_depth_func = GL_LESS;
/// Is depth write enabled?
bool g_depth_write = true;
/// Is polygon offset enabled?
bool g_polygon_offset = false;
/// Current polygon offset in units.
int g_polygon_offset_units = 0;
/// Current stencil function.
GLenum g_stencil_func = GL_FALSE;
/// Current stencil operation mode.
OperationMode g_stencil_operation = DISABLED;
/// Is stencil test enabled?
bool g_stencil_test = false;

#if defined(USE_LD)

/// Total GPU data size spent on edges.
static unsigned g_data_size_edge = 0;
/// Total GPU data size spent on indices.
static unsigned g_data_size_index = 0;
/// Total GPU data size spent on textures.
static unsigned g_data_size_texture = 0;
/// Total GPU data size spent on vertices.
unsigned g_data_size_vertex = 0;

#endif

}

/// Enable one vertex attribute.
///
/// \param name Name to enable.
void attrib_array_enable(GLint name)
{
#if defined(USE_LD)
    if((detail::MAX_ATTRIB_ARRAYS <= name) || (0 > name))
    {
        std::ostringstream sstr;
        sstr << "enabling attribute index " << name << " (" << detail::MAX_ATTRIB_ARRAYS << " supported)";
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
#endif
    if(!detail::g_attrib_arrays_enabled[name])
    {
        dnload_glEnableVertexAttribArray(name);
    }
}

/// Disable extra vertex attribute arrays.
///
/// \param op Index of first array to disable.
void disable_excess_attrib_arrays(unsigned op)
{
    for(unsigned ii = op; (ii < detail::MAX_ATTRIB_ARRAYS); ++ii)
    {
        if(detail::g_attrib_arrays_enabled[ii])
        {
            dnload_glDisableVertexAttribArray(ii);
            detail::g_attrib_arrays_enabled[ii] = false;
        }
    }
}

/// Set blending mode.
///
/// \param op New blending mode.
void blend_mode(OperationMode op)
{
    if(DISABLED == op)
    {
        if(detail::g_blend_enabled)
        {
            dnload_glDisable(GL_BLEND);
            detail::g_blend_enabled = false;
        }
    }
    else
    {
        if(!detail::g_blend_enabled)
        {
            dnload_glEnable(GL_BLEND);
            detail::g_blend_enabled = true;
        }

        if(detail::g_blend_mode != op)
        {
            if(op == PREMULTIPLIED)
            {
                dnload_glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            }
            else // Default is alpha.
            {
#if defined(USE_LD)
                if(op != ALPHA)
                {
                    BOOST_THROW_EXCEPTION(std::runtime_error("invalid blend mode: '" + to_string(op) + "'"));
                }
#endif
                dnload_glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            }
            detail::g_blend_mode = op;
        }
    }
}

/// Clear current framebuffer.
///
/// \param op Bit mask of buffers to clear.
void clear_buffers(optional<uvec4> color, optional<float> depth, optional<uint8_t> stencil)
{
    GLbitfield clear_mask = 0;

    if(color)
    {
        if(*color != detail::g_clear_color)
        {
            vec4 norm_color = color->toNormVec4();
            dnload_glClearColor(norm_color.x(), norm_color.y(), norm_color.z(), norm_color.w());
            detail::g_clear_color = *color;
        }
        clear_mask |= GL_COLOR_BUFFER_BIT;
    }

    if(depth)
    {
        if(*depth != detail::g_clear_depth)
        {
            dnload_glClearDepthf(*depth);
            detail::g_clear_depth = *depth;
        }
        clear_mask |= GL_DEPTH_BUFFER_BIT;
    }

    if(stencil)
    {
        if(*stencil != detail::g_clear_stencil)
        {
            dnload_glClearStencil(*stencil);
            detail::g_clear_stencil = *stencil;
        }
        clear_mask |= GL_STENCIL_BUFFER_BIT;
    }

    dnload_glClear(clear_mask);
}

/// Set color writing mode.
///
/// \param op True to enable, false to disable.
void color_write(bool op)
{
    if(op != detail::g_color_write)
    {
        GLboolean mask_value = op ? GL_TRUE : GL_FALSE;
        dnload_glColorMask(mask_value, mask_value, mask_value, mask_value);
        detail::g_color_write = op;
    }
}

/// Set culling mode.
///
/// \param op Mode, GL_FALSE to disable.
void cull_face(GLenum op)
{
    if(GL_FALSE == op)
    {
        if(detail::g_cull_face_enabled)
        {
            dnload_glDisable(GL_CULL_FACE);
            detail::g_cull_face_enabled = false;
        }
    }
    else
    {
        if(!detail::g_cull_face_enabled)
        {
            dnload_glEnable(GL_CULL_FACE);
            detail::g_cull_face_enabled = true;
        }

        if(detail::g_cull_face != op)
        {
            dnload_glCullFace(op);
            detail::g_cull_face = op;
        }
    }
}

/// Set depth testing mode.
///
/// \param op Depth testing mode, GL_FALSE to disable.
void depth_test(GLenum op)
{
    if(GL_FALSE == op)
    {
        if(detail::g_depth_test)
        {
            dnload_glDisable(GL_DEPTH_TEST);
            detail::g_depth_test = false;
        }
    }
    else
    {
        if(!detail::g_depth_test)
        {
            dnload_glEnable(GL_DEPTH_TEST);
            detail::g_depth_test = true;
        }

        if(detail::g_depth_func != op)
        {
            dnload_glDepthFunc(op);
            detail::g_depth_func = op;
        }
    }
}

/// Set depth writing mode.
///
/// \param op True to enable, false to disable.
void depth_write(bool op)
{
    if(op != detail::g_depth_write)
    {
        GLboolean mask_value = op ? GL_TRUE : GL_FALSE;
        dnload_glDepthMask(mask_value);
        detail::g_depth_write = op;
    }
}

/// Set polygon offset.
///
/// \param op Units to offset.
void polygon_offset(int op)
{
    if(!op)
    {
        if(detail::g_polygon_offset)
        {
            dnload_glDisable(GL_POLYGON_OFFSET_FILL);
            detail::g_polygon_offset = false;
        }
    }
    else
    {
        if(!detail::g_polygon_offset)
        {
            dnload_glEnable(GL_POLYGON_OFFSET_FILL);
            detail::g_polygon_offset = true;
        }
        if(detail::g_polygon_offset_units != op)
        {
            dnload_glPolygonOffset(1.0f, static_cast<float>(op));
            detail::g_polygon_offset_units = op;
        }
    }
}

/// Set stencil mode.
///
/// \param op New mode.
static void stencil_mode(GLenum op)
{
    if(GL_FALSE == op)
    {
        if(detail::g_stencil_test)
        {
            dnload_glDisable(GL_STENCIL_TEST);
            detail::g_stencil_test = false;
        }
    }
    else
    {
        if(!detail::g_stencil_test)
        {
            dnload_glEnable(GL_STENCIL_TEST);
            detail::g_stencil_test = true;
        }
        if(detail::g_stencil_func != op)
        {
            dnload_glStencilFunc(op, 0, 0xFFFFFFFFU);
            detail::g_stencil_func = op;
        }
    }
}

/// Set stencil operation.
///
/// \param op New mode.
static void stencil_operation(OperationMode op)
{
    if(detail::g_stencil_operation != op)
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
        detail::g_stencil_operation = op;
    }
}
  
#if defined(USE_LD)

/// Accessor.
///
/// \return Total edge data size used.
static unsigned get_data_size_edge()
{
    return detail::g_data_size_edge;
}
/// Increment data size.
///
/// \param op Edge data size used.
static unsigned increment_data_size_edge(unsigned op)
{
    return detail::g_data_size_edge += op;
}

/// Accessor.
///
/// \return Total index data size used.
static unsigned get_data_size_index()
{
    return detail::g_data_size_index;
}
/// Increment data size.
///
/// \param op Index data size used.
static unsigned increment_data_size_index(unsigned op)
{
    return detail::g_data_size_index += op;
}

/// Accessor.
///
/// \return Total index data size used.
static unsigned get_data_size_texture()
{
    return detail::g_data_size_texture;
}
/// Increment data size.
///
/// \param op Texture data size used.
static unsigned increment_data_size_texture(unsigned op)
{
    return detail::g_data_size_texture += op;
}

/// Accessor.
///
/// \return Total vertex data size used.
unsigned get_data_size_vertex()
{
    return detail::g_data_size_vertex;
}
/// Increment data size.
///
/// \param op Edge data size used.
unsigned increment_data_size_vertex(unsigned op)
{
    return detail::g_data_size_vertex += op;
}

/// Get an error string corresponding to a GL error.
///
/// \param op GL error.
/// \return Error string.
const char* error_str(GLenum err)
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

/// Perform error check.
///
/// Throws an error on failure.
static void error_check(const char* str = NULL)
{
    GLenum err = glGetError();
    if(GL_NO_ERROR != err)
    {
        std::ostringstream sstr;
        sstr << "GL error " << error_str(err);
        if(str)
        {
            sstr << " at '" << str << "'";
        }
        sstr << ": " << error_str(err);
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
}

#endif

}

#endif
