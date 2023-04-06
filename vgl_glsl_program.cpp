#include "vgl_glsl_program.hpp"

namespace vgl
{

const GeometryBuffer* GlslProgram::g_current_geometry_buffer = nullptr;
const GlslProgram* GlslProgram::g_current_program = nullptr;

#if defined(USE_LD)

std::string to_string(UniformSemantic op)
{
    switch(op)
    {
    case UniformSemantic::PROJECTION_MATRIX:
        return std::string("PROJECTION_MATRIX");

    case UniformSemantic::CAMERA_MATRIX:
        return std::string("CAMERA_MATRIX");

    case UniformSemantic::MODELVIEW_MATRIX:
        return std::string("MODELVIEW_MATRIXC");

    case UniformSemantic::NORMAL_MATRIX:
        return std::string("NORMAL_MATRIX");

    case UniformSemantic::PROJECTION_CAMERA_MATRIX:
        return std::string("PROJECTION_CAMERA_MATRIX");

    case UniformSemantic::CAMERA_MODELVIEW_MATRIX:
        return std::string("CAMERA_MODELVIEW_MATRIX");
    
    case UniformSemantic::PROJECTION_CAMERA_MODELVIEW_MATRIX:
        return std::string("PROJECTION_CAMERA_MODELVIEW_MATRIX");

    case UniformSemantic::CAMERA_POSITION:
        return std::string("CAMERA_POSITION");

    default:
        return std::string("NONE");
    }
}

namespace detail
{

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
    ret.resize(static_cast<unsigned>(len));
    glGetProgramInfoLog(op, len, &acquired, const_cast<GLchar*>(ret.data()));
    return ret;
}

bool get_program_link_status(GLuint op)
{
    GLint ret;
    glGetProgramiv(op, GL_LINK_STATUS, &ret);
    return (GL_FALSE != ret);
}

}

#endif

}

