#ifndef VGL_GEOMETRY_HANDLE_HPP
#define VGL_GEOMETRY_HANDLE_HPP

#include "vgl_extern_opengl.hpp"

namespace vgl
{

/// \cond
class GeometryBuffer;
class GlslProgram;
/// \endcond

namespace detail
{

/// \cond
void geometry_buffer_bind(GeometryBuffer& geometry_buffer, const GlslProgram& prog);
/// \endcond

}

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

    /// Offset into the index buffer (bytes, for drawing).
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

    /// Draw from this handle.
    ///
    /// \param prog Program to draw with.
    /// \param mode Mode to draw with.
    /// \param count Number of elements to draw.
    void draw(const GlslProgram& prog, GLenum mode, unsigned count) const
    {
        detail::geometry_buffer_bind(m_geometry_buffer, prog);
        dnload_glDrawElements(mode, static_cast<GLsizei>(count), GL_UNSIGNED_SHORT,
                reinterpret_cast<void*>(m_index_offset));
    }

public:
#if defined(USE_LD)
    /// Stream output operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const GeometryHandle& rhs)
    {
        return lhs << "Handle(" << &rhs.m_geometry_buffer << " @ " << rhs.m_vertex_offset << " ; " <<
            rhs.m_index_offset << ")";
    }
#endif
};

}

#endif
