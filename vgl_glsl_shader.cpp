#include "vgl_glsl_shader.hpp"

namespace vgl
{

namespace detail
{

#if defined(USE_LD)

string get_shader_info_log(GLuint op)
{
    GLint len;
    glGetShaderiv(op, GL_INFO_LOG_LENGTH, &len);
    if(!len)
    {
        return string();
    }

    GLsizei acquired;
    string ret;
    ret.resize(static_cast<unsigned>(len));
    glGetShaderInfoLog(op, len, &acquired, const_cast<GLchar*>(ret.data()));
    return ret;
}

bool get_shader_compile_status(GLuint op)
{
    GLint ret;
    glGetShaderiv(op, GL_COMPILE_STATUS, &ret);
    return (GL_FALSE != ret);
}

#endif

}

}

