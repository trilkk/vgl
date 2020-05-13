#ifndef VGL_BUFFER_HPP
#define VGL_BUFFER_HPP

#include "vgl_packed_data.hpp"

namespace vgl
{

namespace detail
{

/// Currently bound vertex buffer ID.
GLuint g_current_vertex_buffer = 0;

/// Currently bound index buffer ID.
GLuint g_current_index_buffer = 0;

/// Updates bound buffer ID.
///
/// \param op Buffer ID.
/// \return True if ID was changed.
template<GLenum BufferType> bool update_bound_buffer(GLuint op);

/// \cond
template<> bool update_bound_buffer<GL_ARRAY_BUFFER>(GLuint op)
{
    if(g_current_vertex_buffer == op)
    {
        return false;
    }
    g_current_vertex_buffer = op;
    return true;
}
/// \endcond

/// \cond
template<> bool update_bound_buffer<GL_ELEMENT_ARRAY_BUFFER>(GLuint op)
{
    if(g_current_index_buffer == op)
    {
        return false;
    }
    g_current_index_buffer = op;
    return true;
}
/// \endcond

}

/// Represents a GL array/element buffer
template<GLenum BufferType> class Buffer
{
    static_assert((BufferType == GL_ARRAY_BUFFER) || (BufferType == GL_ELEMENT_ARRAY_BUFFER));

private:
    /// Buffer id.
    GLuint m_id;

private:
    /// Deleted copy constructor.
    Buffer(const Buffer&) = delete;
    /// Deleted assignment.
    Buffer& operator=(const Buffer&) = delete;

public:
    /// Constructor.
    Buffer()
    {
        dnload_glGenBuffers(1, &m_id);
    }

    /// Destructor.
    ~Buffer()
    {
        dnload_glDeleteBuffers(1, &m_id);
    }

private:
    /// Update data to GPU.
    ///
    /// \param ptr Pointer to data to update.
    /// \param count Number of bytes to update.
    void update(const void* ptr, unsigned count) const
    {
        bind();
        dnload_glBufferData(BufferType, count, ptr, GL_STATIC_DRAW);
    }

    /// Update sub-data to GPU.
    ///
    /// \param ptr Pointer to data to update.
    /// \param count Number of bytes to update.
    /// \param offset Offset into the buffer.
    void update(const void* ptr, unsigned count, unsigned offset) const
    {
        bind();
        dnload_glBufferSubData(BufferType, offset, count, ptr);
    }

public:
    /// Accessor.
    ///
    /// \return Identifier.
    GLuint getId() const
    {
        return m_id;
    }

    /// Bind this buffer.
    ///
    /// \return True if the buffer was bound, false if no action was necessary.
    bool bind() const
    {
        if(detail::update_bound_buffer<BufferType>(m_id))
        {
            dnload_glBindBuffer(BufferType, m_id);
            return true;
        }
        return false;
    }

    /// Update data to GPU.
    ///
    /// \param op Data to update.
    void update(const PackedData& op) const
    {
        update(op.data(), op.size());
    }
    /// Update data to GPU.
    ///
    /// \param data Data to update.
    /// \param offset Offset to update into.
    void update(const PackedData& data, unsigned offset) const
    {
        update(data.data(), data.size(), offset);
    }

    /// Update data to GPU.
    ///
    /// \param op Data to update.
    void update(const vector<uint16_t>& op) const
    {
        update(op.data(), op.getSizeBytes());
    }
    /// Update data to GPU.
    ///
    /// \param data Data to update.
    /// \param offset Offset to update into.
    void update(const vector<uint16_t>& data, unsigned offset) const
    {
        update(data.data(), data.getSizeBytes(), offset);
    }
};

/// Specialization of Buffer.
using VertexBuffer = Buffer<GL_ARRAY_BUFFER>;

/// Specialization of Buffer.
using IndexBuffer = Buffer<GL_ELEMENT_ARRAY_BUFFER>;

}

#endif
