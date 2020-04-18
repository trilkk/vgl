#ifndef VGL_BUFFER_HPP
#define VGL_BUFFER_HPP

#include "vgl_vector.hpp"

namespace vgl
{

namespace detail
{

/// Currently bound vertex buffer ID.
detail::g_current_vertex_buffer = 0;

/// Currently bound index buffer ID.
detail::g_current_index_buffer = 0;

/// Updates bound buffer ID.
///
/// \param op Buffer ID.
/// \return True if ID was changed.
template<GLenum BufferType> update_bound_buffer(GLuint op);

/// \cond
template<> update_bound_buffer<GL_ARRAY_BUFFER>(GLuint op)
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
template<> update_bound_buffer<GL_ELEMENT_ARRAY_BUFFER>(GLuint op)
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

protected:
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

public:
    /// Accessor.
    ///
    /// \return Identifier.
    GLuint getId() const
    {
        return m_id;
    }

    /// Bind this buffer.
    void bind() const
    {
        if(detail::update_bound_buffer<BufferType>(m_id))
        {
            dnload_glBindBuffer(BufferType, m_id);
        }
    }

    /// Update data to GPU.
    ///
    /// \param op Data to update.
    template<typename T> void update(const vector<T>& op)
    {
        bind();
        dnload_glBufferData(BufferType, op.getSizeBytes(), op.data(), GL_STATIC_DRAW);

    }

    /// Update data to GPU.
    ///
    /// \param data Data to update.
    /// \param offset Offset to update into.
    template<typename T> void update(const vector<T>& data, unsigned offset)
    {
        bind();
        dnload_glBufferSubData(BufferType, offset, op.getSizeBytes(), op.data());

    }
};

}

#endif
