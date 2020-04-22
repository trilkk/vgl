#ifndef VGL_GEOMETRY_CHANNEL_HPP
#define VGL_GEOMETRY_CHANNEL_HPP

#if defined(USE_LD)
#include <string>
#include <boost/exception/diagnostic_information.hpp>
#endif

namespace vgl
{

/// Different vertex data channels in a mesh.
enum GeometryChannel
{
    /// Channel ID for position.
    POSITION = 0,

    /// Channel ID for normal.
    NORMAL = 1,

    /// Channel ID for texture coordinates.
    TEXCOORD = 2,

    /// Channel ID for color.
    COLOR = 3,

    /// Channel ID for bone ref.
    BONE_REF = 4,

    /// Channel ID for bone weight.
    BONE_WEIGHT = 5,

    /// Channel count.
    COUNT = 6,
};

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

    case BONE_REF:
        return std::string("BONE_REF");

    case BONE_WEIGHT:
        return std::string("BONE_WEIGHT");

    default:
        break;
    }

    BOOST_THROW_EXCEPTION(std::runtime_error("no string representation defined for channel " +
                std::to_string(static_cast<int>(op))));
}

#endif

namespace detail
{

/// Returns the number of elements in a geometry channel.
///
/// \param op Channel ID.
/// \return Number of elements.
constexpr GLint geometry_channel_size(GeometryChannel op)
{
    switch(op)
    {
    case POSITION:
    case NORMAL:
        return 3;

    case TEXCOORD:
        return 2;

    case COLOR:
    case BONE_REF:
    case BONE_WEIGHT:
#if !defined(USE_LD)
    default:
#endif
        return 4;

#if defined(USE_LD)
    default:
        BOOST_THROW_EXCEPTION(std::runtime_error("no element count defined for channel " +
                    std::to_string(static_cast<int>(op))));
#endif
    }
}

/// Returns element type for a geometry channel.
///
/// \param op Channel ID.
/// \return Element type.
constexpr GLenum geometry_channel_type(GeometryChannel op)
{
    switch(op)
    {
    case POSITION:
    case NORMAL:
    case TEXCOORD:
        return GL_FLOAT;

    case COLOR:
    case BONE_REF:
    case BONE_WEIGHT:
#if !defined(USE_LD)
    default:
#endif
        return GL_UNSIGNED_BYTE;

#if defined(USE_LD)
    default:
        BOOST_THROW_EXCEPTION(std::runtime_error("no element count defined for channel " +
                    std::to_string(static_cast<int>(op))));
#endif
    }
}

}

}

#endif
