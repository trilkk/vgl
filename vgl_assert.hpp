#ifndef VGL_ASSERT_HPP
#define VGL_ASSERT_HPP

#include <boost/assert.hpp>

#if defined(USE_LD) && defined(DEBUG)
#define VGL_ASSERT(expr) BOOST_ASSERT(expr)
#else
#define VGL_ASSERT(expr) ((void)0)
#endif

#endif
