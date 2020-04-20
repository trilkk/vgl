#ifndef VGL_MESH_HPP
#define VGL_MESH_HPP

#include "vgl_geometry_buffer.hpp"
#include "vgl_index_block.hpp"
#include "vgl_unique_ptr.hpp"

namespace vgl
{

namespace detail
{

/// Geometry buffers used for uploading meshes to the GPU.
vector<GeometryBufferUptr> g_geometry_buffers;

}

/// Renderable mesh.
class Mesh
{
private:
    /// Internal mesh data.
    MeshData m_data;

    /// Geomery buffer that was deployed to the GPU.
    const GeometryBuffer* m_geometry_buffer;

    /// Geometry handle, filled when updating the mesh to the GPU.
    optional<GeometryHandle> m_handle;

    /// Index collections.
    vector<IndexBlock> m_blocks;

public:
    /// Default constructor.
    explicit Mesh() = default;

public:
    /// End vertex input.
    ///
    /// Called after last vertes element has been written.
    constexpr void endVertex()
    {
        m_data.endVertex();
    }

    /// Write index data.
    ///
    /// \param op Index.
    void write(int16_t op)
    {
        m_data.write(op);
    }

    /// Write vertex data with semantic.
    ///
    /// \param channel Associated channel.
    /// \param data Geometry data.
    void write(detail::GeometryChannel channel, const vec2& data)
    {
        m_data.write(channel, data);
    }

    /// Write vertex data with semantic.
    ///
    /// \param channel Associated channel.
    /// \param data Geometry data.
    void write(detail::GeometryChannel channel, const vec3& data)
    {
        m_data.write(channel, data);
    }

    /// Write vertex data with semantic.
    ///
    /// \param channel Associated channel.
    /// \param data Geometry data.
    void write(detail::GeometryChannel channel, const uvec4& data)
    {
        m_data.write(channel, data);
    }

    /// Update to the GPU.
    void update()
    {
        // If handle already set, update existing data.
        if(m_handle)
        {
            m_data.update(*m_handle);
        }

        for(auto& vv : detail::g_geometry_buffers)
        {
            optional<GeometryHandle> handle = vv->append(m_data);
            if(handle)
            {
                m_handle = handle;
                return;
            }
        }

        // No matches, must create a new geometry buffer.
        detail::g_geometry_buffers.emplace_back(new GeometryBuffer(m_data));
        m_handle = GeometryHandle(*(detail::g_geometry_buffers.back()), 0, 0);
    }
};

/// Smart pointer type.
using MeshUptr = unique_ptr<Mesh>;

}

#endif
