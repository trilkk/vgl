#ifndef VGL_BUFFER_HPP
#define VGL_BUFFER_HPP

#include "vgl_extern_opengl.hpp"
#include "vgl_packed_data.hpp"

namespace vgl
{

namespace detail
{

/// Current state of vertex array object.
class OpenGlVertexArrayObjectState
{
private:
    /// Currently active VAO.
    GLuint m_vao = 0;

public:
    /// Global state.
    static OpenGlVertexArrayObjectState g_opengl_vertex_array_object_state;

public:
    /// Binds given vertex array object.
    ///
    /// \param op Vertex array object ID.
    void bind(GLuint op)
    {
        if(m_vao != op)
        {
            dnload_glBindVertexArray(op);
            m_vao = op;
        }
    }

#if defined(VGL_USE_LD)
    /// Invalidates given vertex array object.
    ///
    /// \param op Vertex array object ID.
    void invalidate(GLuint op)
    {
        if(m_vao == op)
        {
            m_vao = 0;
        }
    }

    /// Gets the currently bound vertex array object.
    ///
    /// \return Vertex array object handle.
    GLuint getCurrentVao()
    {
        return m_vao;
    }
#endif
};

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
    explicit Buffer()
    {
        dnload_glGenBuffers(1, &m_id);
    }

    /// Destructor.
    ~Buffer()
    {
#if defined(VGL_USE_LD)
        glDeleteBuffers(1, &m_id);
#endif
    }

private:
    /// Update data to GPU.
    ///
    /// \param ptr Pointer to data to update.
    /// \param count Number of bytes to update.
    void update(const void* ptr, unsigned count) const
    {
        detail::OpenGlVertexArrayObjectState::g_opengl_vertex_array_object_state.bind(0);
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
        detail::OpenGlVertexArrayObjectState::g_opengl_vertex_array_object_state.bind(0);
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
    /// There is no need to check for currently bound buffer.
    /// During rendering, only the currently bound Vertex Array Object matters.
    /// During updates, checking the bound buffer makes no sense.
    void bind() const
    {
        dnload_glBindBuffer(BufferType, m_id);
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

#if !defined(VGL_USE_LD)
#include "vgl_buffer.cpp"
#endif

#endif
