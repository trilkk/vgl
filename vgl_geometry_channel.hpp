#ifndef VGL_GEOMETRY_CHANNEL_HPP
#define VGL_GEOMETRY_CHANNEL_HPP

#include "vgl_extern_opengl.hpp"

#if defined(VGL_USE_LD)
#include "vgl_throw_exception.hpp"
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

    /// Channel ID for bone weight.
    BONE_WEIGHT = 4,

    /// Channel ID for bone ref.
    BONE_REF = 5,

    /// Channel count.
    COUNT = 6,
};

#if defined(VGL_USE_LD)

/// Get human-readable string corresponding to a channel
///
/// \param op Channel ID.
/// \return String representation for channel.
string to_string(GeometryChannel op);

#endif

namespace detail
{

/// Returns the number of elements in a geometry channel.
///
/// \param op Channel ID.
/// \return Number of elements.
constexpr GLint geometry_channel_element_count(GeometryChannel op)
{
    switch(op)
    {
    case POSITION:
    case NORMAL:
        return 3;

    case TEXCOORD:
        return 2;

    case COLOR:
    case BONE_WEIGHT:
    case BONE_REF:
#if !defined(VGL_USE_LD)
    default:
#endif
        return 4;

#if defined(VGL_USE_LD)
    default:
        VGL_THROW_RUNTIME_ERROR("no element count defined for channel " + to_string(static_cast<int>(op)));
#endif
    }
}

/// Returns element type for a geometry channel.
///
/// \param op Channel ID.
/// \return Element type.
constexpr GLenum geometry_channel_element_type(GeometryChannel op)
{
    switch(op)
    {
    case POSITION:
#if !defined(VGL_ENABLE_VERTEX_NORMAL_PACKING)
    case NORMAL:
#endif
    case TEXCOORD:
        return GL_FLOAT;

#if defined(VGL_ENABLE_VERTEX_NORMAL_PACKING)
    case NORMAL:
        return GL_SHORT;
#endif

    case COLOR:
    case BONE_WEIGHT:
    case BONE_REF:
#if !defined(VGL_USE_LD)
    default:
#endif
        return GL_UNSIGNED_BYTE;

#if defined(VGL_USE_LD)
    default:
        VGL_THROW_RUNTIME_ERROR("no element type defined for channel " + to_string(static_cast<int>(op)));
#endif
    }
}

/// Returns element normalized status for a geometry channel.
///
/// \param op Channel ID.
/// \return Element normalized status.
constexpr GLboolean geometry_channel_element_normalized(GeometryChannel op)
{
    switch(op)
    {
    case POSITION:
#if !defined(VGL_ENABLE_VERTEX_NORMAL_PACKING)
    case NORMAL:
#endif
    case TEXCOORD:
    case BONE_REF:
        return GL_FALSE;

#if defined(VGL_ENABLE_VERTEX_NORMAL_PACKING)
    case NORMAL:
#endif
    case COLOR:
    case BONE_WEIGHT:
#if !defined(VGL_USE_LD)
    default:
#endif
        return GL_TRUE;

#if defined(VGL_USE_LD)
    default:
        VGL_THROW_RUNTIME_ERROR("no element normalized status defined for channel " +
                to_string(static_cast<int>(op)));
#endif
    }
}

}

}

#endif
