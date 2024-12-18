#ifndef VGL_EXTERN_STDLIB_HPP
#define VGL_EXTERN_STDLIB_HPP

/// \file External include: stdlib

#include "vgl_config.hpp"

#include <cstdlib>

#if defined(USE_LD)

/// \cond
#if !defined(dnload_free)
#define dnload_free free
#endif
#if !defined(dnload_rand)
#define dnload_rand rand
#endif
#if !defined(dnload_realloc)
#define dnload_realloc realloc
#endif
#if !defined(dnload_srand)
#define dnload_srand srand
#endif
/// \endcond

#endif

#endif
