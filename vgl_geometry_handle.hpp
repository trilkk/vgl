#ifndef VGL_GEOMETRY_HANDLE_HPP
#define VGL_GEOMETRY_HANDLE_HPP

namespace vgl
{

/// \cond
class GeometryBuffer;
/// \endcond

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
        m_vertex_offset(vertex_offset),
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
};

}

#endif
