#ifndef VGL_GEOMETRY_BUFFER_HPP
#define VGL_GEOMETRY_BUFFER_HPP

#include "vgl_buffer.hpp"
#include "vgl_mesh_data.hpp"
#include "vgl_unique_ptr.hpp"

namespace vgl
{

/// Geometry buffer.
///
/// Collection attribute data.
class GeometryBuffer
{
private:
    /// Internal mesh data (for uploading).
    MeshData m_data;

    /// Vertex buffer.
    VertexBuffer m_vertex_buffer;

    /// Index buffer.
    IndexBuffer m_index_buffer;

public:
    /// Default constructor.
    explicit GeometryBuffer() = default;

    /// Constructor from existing mesh data.
    ///
    /// Implicitly uploads to GPU.
    ///
    /// \param op Mesh data to construct from.
    explicit GeometryBuffer(const MeshData& op) :
        m_data(op)
    {
        update();
    }

private:
    /// Append mesh data into the mesh data in this, and immediately upload to GPU.
    ///
    /// \param op Mesh data to append.
    GeometryHandle appendInternal(const MeshData& op)
    {
        GeometryHandle ret(*this, m_data.getVertexOffset(), m_data.getIndexOffset());
        m_data.append(op);
        update();
        return ret;
    }

    /// Update to GPU.
    void update()
    {
        m_data.update(m_vertex_buffer, m_index_buffer);
    }

public:
    /// Accessor.
    ///
    /// \return Reference to internal vertex buffer.
    constexpr VertexBuffer& getVertexBuffer()
    {
        return m_vertex_buffer;
    }

    /// Try to append given mesh data into this buffer.
    ///
    /// \param op Mesh data to attempt.
    /// \return Geometry handle on success or nullopt.
    optional<GeometryHandle> append(const MeshData& op)
    {
        if(!m_data.matches(op))
        {
            return nullopt;
        }
        if((m_data.getVertexCount() + op.getVertexCount()) > 0xFFFFu)
        {
            return nullopt;
        }
        return appendInternal(op);
    }
};

/// Geometry buffer unique pointer type.
using GeometryBufferUptr = unique_ptr<GeometryBuffer>;

namespace detail
{

/// Update mesh data into GPU as described by geometry handle.
///
/// \param handle Handle into GPU data.
/// \param mesh_data Mesh data being updated.
void geometry_handle_update_mesh_data(const GeometryHandle& handle, const MeshData& mesh_data)
{
    mesh_data.update(handle.getBuffer().getVertexBuffer(), handle.getVertexOffset());
}

}

}

#endif
