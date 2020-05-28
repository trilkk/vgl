#ifndef VGL_CSG_HPP
#define VGL_CSG_HPP

#include "vgl_logical_mesh.hpp"

namespace vgl
{

/// CSG bit.
static const unsigned CSG_FLAG_NO_LEFT = 0;
/// CSG flag.
static const unsigned CSG_NO_LEFT = 1 << CSG_FLAG_NO_LEFT;

/// CSG bit.
static const unsigned CSG_FLAG_NO_RIGHT = 1;
/// CSG flag.
static const unsigned CSG_NO_RIGHT = 1 << CSG_FLAG_NO_RIGHT;

/// CSG bit.
static const unsigned CSG_FLAG_NO_BOTTOM = 2;
/// CSG flag.
static const unsigned CSG_NO_BOTTOM = 1 << CSG_FLAG_NO_BOTTOM;

/// CSG bit.
static const unsigned CSG_FLAG_NO_TOP = 3;
/// CSG flag.
static const unsigned CSG_NO_TOP = 1 << CSG_FLAG_NO_TOP;

/// CSG bit.
static const unsigned CSG_FLAG_NO_BACK = 4;
/// CSG flag.
static const unsigned CSG_NO_BACK = 1 << CSG_FLAG_NO_BACK;

/// CSG bit.
static const unsigned CSG_FLAG_NO_FRONT = 5;
/// CSG flag.
static const unsigned CSG_NO_FRONT = 1 << CSG_FLAG_NO_FRONT;

/// CSG bit: flat shading (as opposed to smooth.
static const unsigned CSG_FLAG_FLAT = 6;
/// CSG flag.
static const unsigned CSG_FLAT = 1 << CSG_FLAG_FLAT;

/// CSG flag count.
static const unsigned CSG_FLAG_COUNT = 7;

using CsgFlags = bitset<CSG_FLAG_COUNT>;

/// Create a box shape.
///
/// \param p1 Center of front face.
/// \param p2 Center of back face.
/// \param width Width.
/// \param height Height.
/// \param up Up direction.
void csg_box(LogicalMesh& lmesh, const vec3& p1, const vec3& p2, float width, float height,
        CsgFlags flags = CsgFlags(0), const vec3& param_up = vec3(0.0f, 1.0f, 0.0f))
{
    vec3 forward = p2 - p1;
    vec3 unit_up = normalize(param_up);

    // Rotate up vector components once if it's perpendicular to the forward vector.
    if(dot(unit_up, normalize(forward)) >= 0.999f)
    {
        unit_up = vec3(unit_up.z(), unit_up.x(), unit_up.y());
    }

    vec3 up = unit_up * (width * 0.5f);
    vec3 rt = normalize(cross(forward, param_up)) * (height * 0.5f);

    unsigned index_base = lmesh.getLogicalVertexCount();
    bool flat = flags[CSG_FLAG_FLAT];

    lmesh.addVertex(p1 - rt - up);
    lmesh.addVertex(p1 + rt - up);
    lmesh.addVertex(p1 + rt + up);
    lmesh.addVertex(p1 - rt + up);
    lmesh.addVertex(p2 - rt - up);
    lmesh.addVertex(p2 + rt - up);
    lmesh.addVertex(p2 + rt + up);
    lmesh.addVertex(p2 - rt + up);

    if(!flags[CSG_FLAG_NO_FRONT])
    {
        lmesh.addFace(index_base + 0, index_base + 1, index_base + 2, index_base + 3, flat);
    }
    if(!flags[CSG_FLAG_NO_BACK])
    {
        lmesh.addFace(index_base + 5, index_base + 4, index_base + 7, index_base + 6, flat);
    }
    if(!flags[CSG_FLAG_NO_RIGHT])
    {
        lmesh.addFace(index_base + 1, index_base + 5, index_base + 6, index_base + 2, flat);
    }
    if(!flags[CSG_FLAG_NO_LEFT])
    {
        lmesh.addFace(index_base + 4, index_base + 0, index_base + 3, index_base + 7, flat);
    }
    if(!flags[CSG_FLAG_NO_BOTTOM])
    {
        lmesh.addFace(index_base + 4, index_base + 5, index_base + 1, index_base + 0, flat);
    }
    if(!flags[CSG_FLAG_NO_TOP])
    {
        lmesh.addFace(index_base + 3, index_base + 2, index_base + 6, index_base + 7, flat);
    }
}

}

#endif
