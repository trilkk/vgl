#ifndef VGL_CSTDDEF_HPP
#define VGL_CSTDDEF_HPP

#include "vgl_config.hpp"

#include <cstddef>

namespace vgl
{

using std::nullptr_t;

}

#if defined(VGL_USE_LD) && defined(DEBUG)
#define VGL_VOLUNTARY_MEMBER_VALUE(name, value) name = value
#else
#define VGL_VOLUNTARY_MEMBER_VALUE(name, value) name
#endif

#endif
