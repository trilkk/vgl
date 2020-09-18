#ifndef VGL_MESH_HPP
#define VGL_MESH_HPP

#include "vgl_bounding_box.hpp"
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

    /// Geometry handle, filled when updating the mesh to the GPU.
    optional<GeometryHandle> m_handle;

    /// Index collections.
    vector<IndexBlock> m_blocks;

    /// Bounding box for the mesh.
    BoundingBox m_box;

#if defined(USE_LD)
    /// Name of the mesh, for debugging.
    string m_name;
#endif

public:
    /// Default constructor.
    explicit Mesh() = default;

public:
    /// Accessor.
    ///
    /// \return Bounding box.
    const BoundingBox& getBoundingBox() const
    {
        return m_box;
    }

    /// Draw the mesh.
    ///
    /// \param op Program to draw with.
    void draw(const GlslProgram& op)
    {
#if defined(USE_LD) && defined(DEBUG)
        if(!m_handle)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("cannot draw mesh with invalid geometry handle"));
        }
#endif
        m_handle->draw(op, GL_TRIANGLES, m_data.getIndexCount());
    }

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
    void write(uint16_t op)
    {
        m_data.write(op);
    }

    /// Write vertex data with semantic.
    ///
    /// \param channel Associated channel.
    /// \param data Geometry data.
    void write(GeometryChannel channel, const ivec4& data)
    {
        m_data.write(channel, data);
    }

    /// Write vertex data with semantic.
    ///
    /// \param channel Associated channel.
    /// \param data Geometry data.
    void write(GeometryChannel channel, const vec2& data)
    {
        m_data.write(channel, data);
    }

    /// Write vertex data with semantic.
    ///
    /// \param channel Associated channel.
    /// \param data Geometry data.
    void write(GeometryChannel channel, const vec3& data)
    {
        m_data.write(channel, data);

        // Update bounding box with data if writing to position channel.
        if(channel == GeometryChannel::POSITION)
        {
            m_box.addPoint(data);
        }
    }

    /// Write vertex data with semantic.
    ///
    /// \param channel Associated channel.
    /// \param data Geometry data.
    void write(GeometryChannel channel, const uvec4& data)
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

#if defined(USE_LD)
    /// Accessor.
    ///
    /// \return Name.
    const string& getName() const
    {
        return m_name;
    }
    /// Set the name.
    ///
    /// \param op New name.
    void setName(string_view op)
    {
        m_name = op;
    }
#endif
};

/// Smart pointer type.
using MeshUptr = unique_ptr<Mesh>;

}

#endif
