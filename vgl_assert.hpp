#ifndef VGL_ASSERT_HPP
#define VGL_ASSERT_HPP

#include "vgl_config.hpp"

#if defined(VGL_USE_LD) && defined(DEBUG)
#include <boost/assert.hpp>
/// Assertion macro (enabled).
#define VGL_ASSERT(expr) BOOST_ASSERT(expr)
#else
/// Assertion macro (disabled).
#define VGL_ASSERT(expr) ((void)0)
#endif

#endif
