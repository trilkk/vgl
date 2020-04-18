#ifndef VGL_GEOMETRY_BUFFER_HPP
#define VGL_GEOMETRY_BUFFER_HPP

#include "vgl_buffer.hpp"
#include "vgl_mesh_data.hpp"

namespace vgl
{

namespace detail
{

/// Geometry handle.
///
/// Contains a reference to geometry buffer and data index within for reuploading data.
class GeometryHandle
{
private:
    /// Geometry buffer used.
    GeometryBuffer& m_geometry_buffer;

    /// Offset into the geometry buffer (bytes, for updating data).
    unsigned m_vertex_offset;

    /// Offset into the index buffer (elements, for drawing).
    unsigned m_index_offset;

public:
    /// Constructor.
    ///
    /// \param buffer Geometry buffer.
    /// \param vertex_offset Vertex buffer offset.
    /// \param index_offset Index buffer offset.
    constexpr explicit GeometryHandle(GeometryBuffer& buffer, unsigned vertex_offset, unsigned index_offset) noexcept :
        m_geometry_buffer(buffer),
        m_vertex_offset(vertex_offset)
        m_index_offset(index_offset)
    {
    }

public:
    /// Accessor.
    ///
    /// \return Geometry buffer.
    constexpr GeometryBuffer& getBuffer() const noexcept
    {
        return m_geometry_buffer;
    }

    /// Accessor.
    ///
    /// \return Offset into buffer.
    constexpr unsigned getVertexOffset() const noexcept
    {
        return m_vertex_offset;
    }

    /// Accessor.
    ///
    /// \return Offset into buffer.
    constexpr unsigned getIndexOffset() const noexcept
    {
        return m_index_offset;
    }

    /// Update mesh data by this geometry buffer handle.
    void update(const MeshData& op) const
    {
        op.update(buffer.getVertexBuffer(), m_vertex_offset);
    }
};

}

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
    explicit GepometryBuffer(const MeshData& op) :
        m_data(op)
    {
        update();
    }

private:
    /// Append mesh data into the mesh data in this, and immediately upload to GPU.
    ///
    /// \param op Mesh data to append.
    GeometryHandle append(const MeshData& op)
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
        return m_data.append(op);
    }
};

}

#endif
