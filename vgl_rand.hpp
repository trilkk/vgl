#ifndef VGL_RAND_HPP
#define VGL_RAND_HPP

#include "vgl_extern_stdlib.hpp"

namespace vgl
{

#if !defined(VGL_DISABLE_RAND)

/// \brief Random float value.
///
/// \param op Given maximum value.
/// \return Random value between 0.0f and given value.
float frand(float op = 1.0f);

/// \brief Random float value.
///
/// \param lo Minimum value.
/// \param hi Maximum value.
/// \return Random value between low and high values.
float frand(float lo, float hi);

#endif

}

#if !defined(USE_LD)
#include "vgl_rand.cpp"
#endif

#endif
