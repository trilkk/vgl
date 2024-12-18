#include "vgl_state.hpp"

#include "vgl_string_view.hpp"
#include "vgl_vector.hpp"

namespace vgl
{

namespace detail
{

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
#endif

}

#if defined(USE_LD)

string to_string(OperationMode op)
{
    switch(op)
    {
    case DISABLED:
        return string("DISABLED");

    case ADDITIVE:
        return string("ADDITIVE");

    case PREMULTIPLIED:
        return string("PREMULTIPLIED");

    case CARMACK:
        return string("CARMACK");

    default:
        break;
    }

    VGL_THROW_RUNTIME_ERROR("invalid OperationMode value: '" + to_string(static_cast<int>(op)) + "'");
}

string gl_get_string(GLenum op)
{
    const GLubyte* ret = glGetString(op);
    if(!ret)
    {
        return string();
    }
    return string(reinterpret_cast<const char*>(ret));
}

string gl_extension_string(unsigned align, unsigned indent)
{
    string extension_string = gl_get_string(GL_EXTENSIONS);
    string_view extension_view(extension_string.data(), static_cast<unsigned>(extension_string.length()));
    vector<string_view> extensions;
    for(unsigned ii = 0; (ii < extension_view.length());)
    {
        int cc = extension_view[ii];
        if(!cc || (cc == ' ') || (cc == '\t') || (cc == '\r') || (cc == '\n'))
        {
            if(ii != 0)
            {
                extensions.emplace_back(extension_view.data(), ii);
            }
            unsigned skip = ii + 1;
            extension_view = string_view(extension_view.data() + skip, extension_view.length() - skip);
            ii = 0;
            continue;
        }
        ++ii;
    }
    if(!extension_view.empty())
    {
        extensions.emplace_back(extension_view);
    }

    string istr;
    for(unsigned ii = 0; (ii < indent); ++ii)
    {
        istr += " ";
    }

    string ret;
    string line;
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
                line += istr + string(vv);
            }
        }
        else
        {
            line += " " + string(vv);
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

string gl_vendor_string()
{
    return gl_get_string(GL_VENDOR) + " " + gl_get_string(GL_RENDERER);
}

string gl_version_string()
{
    return gl_get_string(GL_VERSION) + " GLSL " + gl_get_string(GL_SHADING_LANGUAGE_VERSION);
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
        VGL_THROW_RUNTIME_ERROR(sstr.str().c_str());
    }
}

#endif

}

