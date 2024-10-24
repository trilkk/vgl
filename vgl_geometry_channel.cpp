#include "vgl_geometry_channel.hpp"

namespace vgl
{

#if defined(USE_LD)

/// Get human-readable string corresponding to a channel
///
/// \param op Channel ID.
/// \return String representation for channel.
string to_string(GeometryChannel op)
{
    switch(op)
    {
    case POSITION:
        return string("POSITION");

    case NORMAL:
        return string("NORMAL");

    case TEXCOORD:
        return string("TEXCOORD");

    case COLOR:
        return string("COLOR");

    case BONE_WEIGHT:
        return string("BONE_WEIGHT");

    case BONE_REF:
        return string("BONE_REF");

    default:
        break;
    }

    VGL_THROW_RUNTIME_ERROR("no string representation defined for channel " + to_string(static_cast<int>(op)));
}

#endif

}

