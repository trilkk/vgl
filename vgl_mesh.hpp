#ifndef VGL_MESH_HPP
#define VGL_MESH_HPP

#include "vgl_geometry_buffer.hpp"
#include "vgl_index_block.hpp"
#include "vgl_unique_ptr.hpp"

namespace vgl
{

/// Renderable mesh.
class Mesh
{
private:
    /// Geomery for the mesh.
    const GeometryBuffer m_vertex_buffer;

    /// Index collections.
    vector<IndexBlock> m_blocks;

public:
    /// Default constructor.
    explicit Mesh() = default;
};

/// Smart pointer type.
using MeshUptr = unique_ptr<Mesh>;

}

#endif
