#include "vgl_geometry_channel.hpp"

namespace vgl
{

#if defined(USE_LD)

/// Get human-readable string corresponding to a channel
///
/// \param op Channel ID.
/// \return String representation for channel.
std::string to_string(GeometryChannel op)
{
    switch(op)
    {
    case POSITION:
        return std::string("POSITION");

    case NORMAL:
        return std::string("NORMAL");

    case TEXCOORD:
        return std::string("TEXCOORD");

    case COLOR:
        return std::string("COLOR");

    case BONE_WEIGHT:
        return std::string("BONE_WEIGHT");

    case BONE_REF:
        return std::string("BONE_REF");

    default:
        break;
    }

    BOOST_THROW_EXCEPTION(std::runtime_error("no string representation defined for channel " +
                std::to_string(static_cast<int>(op))));
}

#endif

}

