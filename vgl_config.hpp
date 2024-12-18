#ifndef VGL_CONFIG_HPP
#define VGL_CONFIG_HPP

/// \file Configuration macro parsing.
///
/// This top-level header should be the first in inclusion chain from all other headers.

#if defined(USE_LD) && USE_LD
/// Compile expecting a linker will be used.
#define VGL_USE_LD
#endif

#if defined(DNLOAD_GLESV2) && DNLOAD_GLESV2
/// Use OpenGL ES as opposed to OpenGL.
#define VGL_USE_GLES
#endif

#endif
