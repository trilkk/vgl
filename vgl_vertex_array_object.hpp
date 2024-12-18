#ifndef VGL_VERTEX_ARRAY_OBJECT
#define VGL_VERTEX_ARRAY_OBJECT

#include "vgl_buffer.hpp"

namespace vgl
{

/// \cond
class GlslProgram;
class MeshData;
class VertexArrayObject;
/// \endcond

namespace detail
{

/// \cond
void setup_vertex_array_object(
        const VertexArrayObject& vertex_array_object,
        const GlslProgram& program,
        const VertexBuffer& vertex_buffer,
        const IndexBuffer& index_buffer,
        const MeshData& data);
/// \endcond

}

/// Abstraction for vertex array object.
class VertexArrayObject
{
private:
    /// Vertex array object id.
    GLuint m_id = 0;

private:
    /// Deleted copy constructor.
    VertexArrayObject(const VertexArrayObject&) = delete;
    /// Deleted assignment.
    VertexArrayObject& operator=(const VertexArrayObject&) = delete;

public:
    /// Default constructor.
    ///
    /// \param program Program associated with the vertex array object.
    /// \param vertex_buffer Vertex buffer to use.
    /// \param index_buffer Index buffer to use.
    /// \param data Data to use for format.
    explicit VertexArrayObject(
            const GlslProgram& program,
            const VertexBuffer& vertex_buffer,
            const IndexBuffer& index_buffer,
            const MeshData& data)
    {
        dnload_glGenVertexArrays(1, &m_id);
        detail::setup_vertex_array_object(*this, program, vertex_buffer, index_buffer, data);
    }

    /// Destructor.
    ~VertexArrayObject()
    {
#if defined(USE_LD)
        if(m_id)
        {
            detail::OpenGlVertexArrayObjectState::g_opengl_vertex_array_object_state.invalidate(m_id);
            glDeleteVertexArrays(1, &m_id);
        }
#endif
    }

    /// Move constructor.
    ///
    /// \param other Source object.
    VertexArrayObject(VertexArrayObject&& other) :
        m_id(other.m_id)
    {
#if defined(USE_LD)
        other.m_id = 0;
#endif
    }

public:
    /// Accessor.
    ///
    /// \return Identifier.
    GLuint getId() const
    {
        return m_id;
    }

    /// Binds the vertex array object for use.
    void bind() const
    {
        detail::OpenGlVertexArrayObjectState::g_opengl_vertex_array_object_state.bind(m_id);
    }

public:
    /// Move operator.
    ///
    /// \param other Source object.
    /// \return This object.
    VertexArrayObject& operator=(VertexArrayObject&& other)
    {
        m_id = other.m_id;
#if defined(USE_LD)
        other.m_id = 0;
#endif
        return *this;
    }
};

}

#endif
