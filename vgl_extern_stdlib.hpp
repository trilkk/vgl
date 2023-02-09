#ifndef VGL_EXTERN_STDLIB
#define VGL_EXTERN_STDLIB

/// \file
/// \brief External include: stdlib

#include <cstdlib>

#if defined(USE_LD)

/// \cond
#if !defined(dnload_free)
#define dnload_free free
#endif
#if !defined(dnload_realloc)
#define dnload_realloc realloc
#endif
/// \endcond

#endif

#endif
