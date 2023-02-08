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
#if !defined(dnload_glBindBuffer)
#define dnload_glBindBuffer glBindBuffer
#endif
#if !defined(dnload_glBufferData)
#define dnload_glBufferData glBufferData
#endif
#if !defined(dnload_glBufferSubData)
#define dnload_glBufferSubData glBufferSubData
#endif
#if !defined(dnload_glDeleteBuffers)
#define dnload_glDeleteBuffers glDeleteBuffers
#endif
#if !defined(dnload_glGenBuffers)
#define dnload_glGenBuffers glGenBuffers
#endif
/// \endcond

#endif

#endif
