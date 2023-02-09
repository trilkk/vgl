#ifndef VGL_EXTERN_HPP
#define VGL_EXTERN_HPP

/// \file
/// \brief External include: OpenGL

#if defined(USE_LD)

#if defined(WIN32)
#include "windows.h"
#include "GL/glew.h"
#else
#include "GL/glew.h"
#endif

/// \cond
#if !defined(dnload_glActiveTexture)
#define dnload_glActiveTexture glActiveTexture
#endif
#if !defined(dnload_glAttachShader)
#define dnload_glAttachShader glAttachShader
#endif
#if !defined(dnload_glBindBuffer)
#define dnload_glBindBuffer glBindBuffer
#endif
#if !defined(dnload_glBindTexture)
#define dnload_glBindTexture glBindTexture
#endif
#if !defined(dnload_glBlendFuncSeparate)
#define dnload_glBlendFuncSeparate glBlendFuncSeparate
#endif
#if !defined(dnload_glBufferData)
#define dnload_glBufferData glBufferData
#endif
#if !defined(dnload_glBufferSubData)
#define dnload_glBufferSubData glBufferSubData
#endif
#if !defined(dnload_glClear)
#define dnload_glClear glClear
#endif
#if !defined(dnload_glClearColor)
#define dnload_glClearColor glClearColor
#endif
#if !defined(dnload_glClearDepthf)
#define dnload_glClearDepthf glClearDepthf
#endif
#if !defined(dnload_glClearStencil)
#define dnload_glClearStencil glClearStencil
#endif
#if !defined(dnload_glColorMask)
#define dnload_glColorMask glColorMask
#endif
#if !defined(dnload_glCompileShader)
#define dnload_glCompileShader glCompileShader
#endif
#if !defined(dnload_glCreateProgram)
#define dnload_glCreateProgram glCreateProgram
#endif
#if !defined(dnload_glCreateShader)
#define dnload_glCreateShader glCreateShader
#endif
#if !defined(dnload_glCullFace)
#define dnload_glCullFace glCullFace
#endif
#if !defined(dnload_glDeleteBuffers)
#define dnload_glDeleteBuffers glDeleteBuffers
#endif
#if !defined(dnload_glDeleteProgram)
#define dnload_glDeleteProgram glDeleteProgram
#endif
#if !defined(dnload_glDeleteShader)
#define dnload_glDeleteShader glDeleteShader
#endif
#if !defined(dnload_glDeleteTextures)
#define dnload_glDeleteTextures glDeleteTextures
#endif
#if !defined(dnload_glDepthFunc)
#define dnload_glDepthFunc glDepthFunc
#endif
#if !defined(dnload_glDepthMask)
#define dnload_glDepthMask glDepthMask
#endif
#if !defined(dnload_glDisable)
#define dnload_glDisable glDisable
#endif
#if !defined(dnload_glDisableVertexAttribArray)
#define dnload_glDisableVertexAttribArray glDisableVertexAttribArray
#endif
#if !defined(dnload_glDrawElements)
#define dnload_glDrawElements glDrawElements
#endif
#if !defined(dnload_glEnable)
#define dnload_glEnable glEnable
#endif
#if !defined(dnload_glEnableVertexAttribArray)
#define dnload_glEnableVertexAttribArray glEnableVertexAttribArray
#endif
#if !defined(dnload_glGenBuffers)
#define dnload_glGenBuffers glGenBuffers
#endif
#if !defined(dnload_glGenerateMipmap)
#define dnload_glGenerateMipmap glGenerateMipmap
#endif
#if !defined(dnload_glGenTextures)
#define dnload_glGenTextures glGenTextures
#endif
#if !defined(dnload_glGetAttribLocation)
#define dnload_glGetAttribLocation glGetAttribLocation
#endif
#if !defined(dnload_glGetUniformLocation)
#define dnload_glGetUniformLocation glGetUniformLocation
#endif
#if !defined(dnload_glLinkProgram)
#define dnload_glLinkProgram glLinkProgram
#endif
#if !defined(dnload_glPolygonOffset)
#define dnload_glPolygonOffset glPolygonOffset
#endif
#if !defined(dnload_glShaderSource)
#define dnload_glShaderSource glShaderSource
#endif
#if !defined(dnload_glStencilFunc)
#define dnload_glStencilFunc glStencilFunc
#endif
#if !defined(dnload_glStencilOpSeparate)
#define dnload_glStencilOpSeparate glStencilOpSeparate
#endif
#if !defined(dnload_glTexParameterf)
#define dnload_glTexParameterf glTexParameterf
#endif
#if !defined(dnload_glTexParameteri)
#define dnload_glTexParameteri glTexParameteri
#endif
#if !defined(dnload_glUniform1fv)
#define dnload_glUniform1fv glUniform1fv
#endif
#if !defined(dnload_glUniform1iv)
#define dnload_glUniform1iv glUniform1iv
#endif
#if !defined(dnload_glUniform2fv)
#define dnload_glUniform2fv glUniform2fv
#endif
#if !defined(dnload_glUniform3fv)
#define dnload_glUniform3fv glUniform3fv
#endif
#if !defined(dnload_glUniform4fv)
#define dnload_glUniform4fv glUniform4fv
#endif
#if !defined(dnload_glUniformMatrix2fv)
#define dnload_glUniformMatrix2fv glUniformMatrix2fv
#endif
#if !defined(dnload_glUniformMatrix3fv)
#define dnload_glUniformMatrix3fv glUniformMatrix3fv
#endif
#if !defined(dnload_glUniformMatrix4fv)
#define dnload_glUniformMatrix4fv glUniformMatrix4fv
#endif
#if !defined(dnload_glUseProgram)
#define dnload_glUseProgram glUseProgram
#endif
#if !defined(dnload_glVertexAttribPointer)
#define dnload_glVertexAttribPointer glVertexAttribPointer
#endif
/// \endcond

#endif

#endif
