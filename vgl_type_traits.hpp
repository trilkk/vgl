#ifndef VGL_TYPE_TRAITS_HPP
#define VGL_TYPE_TRAITS_HPP

#include <type_traits>

#if __cplusplus > 201703L && !defined(__clang__)
/// is_constant_evaluated() is available.
#define VGL_IS_CONSTANT_EVALUATED
#endif

namespace vgl
{

using std::is_trivially_constructible;
using std::is_trivially_destructible;

#if defined(VGL_IS_CONSTANT_EVALUATED)
using std::is_constant_evaluated;
#endif

}

#endif
