#include "vgl_rand.hpp"

namespace vgl
{

#if !defined(VGL_DISABLE_RAND)

float frand(float op)
{
    return static_cast<float>(dnload_rand() & 0xFFFF) * ((1.0f / 65535.0f) * op);
}

float frand(float lo, float hi)
{
    return frand(hi - lo) + lo;
}

#endif

}

