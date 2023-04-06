#include "vgl_state.hpp"

namespace vgl
{

OpenGlAttribState OpenGlAttribState::g_opengl_attrib_state;
OpenGlBlendState OpenGlBlendState::g_opengl_blend_state;
OpenGlClearState OpenGlClearState::g_opengl_clear_state;
OpenGlColorWriteState OpenGlColorWriteState::g_opengl_color_write_state;
OpenGlCullFaceState OpenGlCullFaceState::g_opengl_cull_face_state;
OpenGlDepthState OpenGlDepthState::g_opengl_depth_state;
#if !defined(VGL_DISABLE_POLYGON_OFFSET)
OpenGlPolygonOffsetState OpenGlPolygonOffsetState::g_opengl_polygon_offset_state;
#endif
#if !defined(VGL_DISABLE_STENCIL)
OpenGlStencilState OpenGlStencilState::g_opengl_stencil_state;
#endif
#if defined(USE_LD)
OpenGlDiagnosticsState OpenGlDiagnosticsState::g_opengl_diagnostics_state;

std::string gl_extension_string(unsigned align, unsigned indent)
{
    std::string extension_string(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)));
    std::vector<std::string> extensions;
    for(unsigned ii = 0; (ii < extension_string.length());)
    {
        int cc = extension_string[ii];
        if(!cc || (cc == ' ') || (cc == '\t') || (cc == '\r') || (cc == '\n'))
        {
            if(ii != 0)
            {
                extensions.push_back(extension_string.substr(0, ii));
            }
            extension_string = extension_string.substr(ii + 1);
            ii = 0;
            continue;
        }
        ++ii;
    }
    if(!extension_string.empty())
    {
        extensions.push_back(extension_string);
    }

    std::string istr;
    for(unsigned ii = 0; (ii < indent); ++ii)
    {
        istr += " ";
    }

    std::string ret;
    std::string line;
    for(const auto& vv : extensions)
    {
        // If line would exceed length, append it to return value.
        if(!line.empty() && align && ((istr.length() + line.length() + 1 + vv.length()) > align))
        {
            if(!ret.empty())
            {
                ret += "\n";
            }
            ret += line;
            line.clear();
        }

        // May need to add indent to line.
        if(line.empty())
        {
            if(ret.empty())
            {
                line += vv;
            }
            else
            {
                line += istr + vv;
            }
        }
        else
        {
            line += " " + vv;
        }
    }

    // Add last line.
    if(!line.empty())
    {
        if(!ret.empty())
        {
            ret += "\n";
        }
        ret += line;
    }

    return ret;
}

std::string gl_vendor_string()
{
    return reinterpret_cast<const char*>(glGetString(GL_VENDOR)) + std::string(" ") +
        reinterpret_cast<const char*>(glGetString(GL_RENDERER));
}

std::string gl_version_string()
{
    return reinterpret_cast<const char*>(glGetString(GL_VERSION)) + std::string(" GLSL ") +
        reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void error_check(const char* str)
{
    GLenum err = glGetError();
    if(GL_NO_ERROR != err)
    {
        std::ostringstream sstr;
        sstr << "GL error " << gl_error_string(err);
        if(str)
        {
            sstr << " at '" << str << "'";
        }
        sstr << ": " << gl_error_string(err);
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
}

#endif

}

