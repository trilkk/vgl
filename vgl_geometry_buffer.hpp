#ifndef VGL_GEOMETRY_BUFFER_HPP
#define VGL_GEOMETRY_BUFFER_HPP

#include "vgl_mesh_data.hpp"
#include "vgl_unique_ptr.hpp"
#include "vgl_vertex_array_object.hpp"

namespace vgl
{

/// Geometry buffer.
///
/// Collection attribute data.
class GeometryBuffer
{
private:
    /// Program to vertex array object mapping.
    class VaoMapping
    {
    private:
        /// Associated program.
        const GlslProgram* m_program = nullptr;

        /// Associated vertex array object.
        VertexArrayObject m_vertex_array_object;

    public:
        /// Constructor.
        ///
        /// \param program Program to associate with.
        /// \param vertex_buffer Vertex buffer to use.
        /// \param index_buffer Index buffer to use.
        /// \param data Data to use for format.
        explicit VaoMapping(
                const GlslProgram& program,
                const VertexBuffer& vertex_buffer,
                const IndexBuffer& index_buffer,
                const MeshData& data) :
            m_program(&program),
            m_vertex_array_object(program, vertex_buffer, index_buffer, data)
        {
        }

        /// Move constructor.
        ///
        /// \param other Source object.
        VaoMapping(VaoMapping&& other) :
            m_program(other.m_program),
            m_vertex_array_object(move(other.m_vertex_array_object))
        {
        }

    public:
        /// Bind the contained vertex array object into use.
        ///
        /// \return Vertex array object.
        void bind() const
        {
            m_vertex_array_object.bind();
        }

        /// Match check.
        ///
        /// \param op Program to check against.
        /// \return Program.
        bool matches(const GlslProgram& op) const
        {
            VGL_ASSERT(m_program);
            return (m_program == &op);
        }

    public:
        /// Move operator.
        ///
        /// \param other Source object.
        /// \return This object.
        VaoMapping& operator=(VaoMapping&& other)
        {
            m_program = other.m_program;
            m_vertex_array_object = move(other.m_vertex_array_object);
            return *this;
        }
    };

private:
    /// Internal mesh data (for uploading).
    MeshData m_data;

    /// Vertex buffer.
    VertexBuffer m_vertex_buffer;

    /// Index buffer.
    IndexBuffer m_index_buffer;

    /// Vertex array objects.
    vector<VaoMapping> m_vao_mapping;

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
#if defined(VGL_USE_LD)
        detail::increment_buffer_data_sizes(op);
#endif
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
#if defined(VGL_USE_LD)
        detail::increment_buffer_data_sizes(op);
#endif
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

    /// Bind this geometry buffer for drawing.
    ///
    /// \param op Program to bind with.
    void bind(const GlslProgram& op)
    {
        for(const auto& vv : m_vao_mapping)
        {
            if(vv.matches(op))
            {
                vv.bind();
                return;
            }
        }

        // Creating new VAO leaves it bound.
        m_vao_mapping.emplace_back(op, m_vertex_buffer, m_index_buffer, m_data);
    }

#if defined(VGL_USE_LD)
public:
    /// Stream output operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const GeometryBuffer& rhs)
    {
        return lhs << "GeometryBuffer(" << rhs.m_vertex_buffer.getId() << ", " << rhs.m_index_buffer.getId() << ")";
    }
#endif
};

/// Geometry buffer unique pointer type.
using GeometryBufferUptr = unique_ptr<GeometryBuffer>;

namespace detail
{

/// Bind a geometry buffer for rendering.
///
/// \param geometry_buffer Geometry buffer to bind.
/// \param prog Program to bind with.
inline void geometry_buffer_bind(GeometryBuffer& geometry_buffer, const GlslProgram& prog)
{
    geometry_buffer.bind(prog);
}

/// Update mesh data into GPU as described by geometry handle.
///
/// \param handle Handle into GPU data.
/// \param mesh_data Mesh data being updated.
inline void geometry_handle_update_mesh_data(const GeometryHandle& handle, const MeshData& mesh_data)
{
    mesh_data.update(handle.getBuffer().getVertexBuffer(), handle.getVertexOffset());
}

/// Updates mesh data.
///
/// \param vertex_array_object Vertex array object to setup.
/// \param program Program associated with the vertex array object.
/// \param vertex_buffer Vertex buffer to use.
/// \param index_buffer Index buffer to use.
/// \param data Data to use for format.
inline void setup_vertex_array_object(
        const VertexArrayObject& vertex_array_object,
        const GlslProgram& program,
        const VertexBuffer& vertex_buffer,
        const IndexBuffer& index_buffer,
        const MeshData& data)
{
    vertex_array_object.bind();
    vertex_buffer.bind();
    index_buffer.bind();
    data.bindAttributes(program);
}

}

}

#endif
