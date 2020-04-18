#ifndef VGL_MESH_DATA_HPP
#define VGL_MESH_DATA_HPP

#include "vgl_buffer.hpp"
#include "vgl_vec2.hpp"
#include "vgl_vec3.hpp"
#include "vgl_uvec4.hpp"

namespace vgl
{

namespace detail
{

/// Different vertex data channels in a mesh.
enum GeometryChannel
{
    /// Channel ID for position.
    POSITION = 0;

    /// Channel ID for normal.
    NORMAL = 1;

    /// Channel ID for texture coordinates.
    TEXCOORD = 2;

    /// Channel ID for color.
    COLOR = 3;

    /// Channel ID for bone ref.
    BONE_REF = 4;

    /// Channel ID for bone weight.
    BONE_WEIGHT = 5;

    /// Channel count.
    COUNT = 6;
};

/// Returns the number of elements in a geometry channel.
///
/// \param op Channel ID.
/// \return Number of elements.
GLint geometry_channel_size(GeometryChannel op)
{
    switch(op)
    {
    case POSITION:
    case NORMAL:
        return 3;

    case TEXCOORD:
        return 2;

    case COLOR:
    case BONE_REF:
    case BONE_WEIGHT:
#if defined(USE_LD)
    default:
#endif
        return 4;

#if defined(USE_LD)
    default:
        BOOST_THROW_EXCEPTION(std::runtime_error("no element count defined for channel " +
                    std::to_string(static_cast<int>(op))));
#endif
    }
}

/// Returns element type for a geometry channel.
///
/// \param op Channel ID.
/// \return Element type.
GLenum geometry_channel_type(GeometryChannel op)
{
    switch(op)
    {
    case POSITION:
    case NORMAL:
    case TEXCOORD:
        return GL_FLOAT;

    case COLOR:
    case BONE_REF:
    case BONE_WEIGHT:
#if defined(USE_LD)
    default:
#endif
        return GL_UNSIGNED_BYTE;

#if defined(USE_LD)
    default:
        BOOST_THROW_EXCEPTION(std::runtime_error("no element count defined for channel " +
                    std::to_string(static_cast<int>(op))));
#endif
}

}

/// Mesh data that can be uploaded to a GPU.
class MeshData
{
public:
    /// Internal class for channel information. Used when binding.
    class ChannelInfo
    {
    private:
        /// Semantic.
        const GeopmetryChannel m_semantic;

        /// Number of elements in this channel.
        const GLint m_element_count;

        /// Element type for this channel.
        const GLenum m_type;

        /// Offset of the channel.
        const GLsizei m_offset;

    public:
        /// Constructor.
        ///
        /// \param channel Channel ID.
        /// \param offset Offset of the channel.
        constexpr explicit ChannelInfo(GeometryChannel channel, unsigned offset) noexcept :
            m_semantic(channel),
            m_element_count(detail::geometry_channel_size(channel)),
            m_type(detail::geometry_channel_type(channel)),
            m_offset(offset)
        {
        }

    public:
        /// Accessor.
        ///
        /// \return Semantic.
        constexpr GeometryChannel getSemantic() const
        {
            return m_semantic;
        }
    };

private:
    /// Raw data.
    vector<uint8_t> m_vertex_data;

    /// Raw data.
    vector<uint16_t> m_index_data;

    /// Channel info.
    vector<ChannelInfo> m_channels;

    /// Channel stride.
    GLsizei m_stride = 0;

    /// Vertex count.
    unsigned m_vertex_count = 0;

public:
    /// Default constructor.
    constexpr explicit MeshData() noexcept = default;

private:
    /// Expand data with the size of a type and write it.
    ///
    /// \return Pointer to where the value can be written.
    template<typename T> void expand()
    {
        while(unsigned ii = 0; (ii < sizeof(T)); ++ii)
        {
            m_vertex_data.push_back(0u);
        }
        return reinterpret_cast<T*>(m_vertex_data.data() + m_vertex_data.size() - sizeof(T));
    }

    /// Write data.
    ///
    /// \param op Value to be written.
    template<typename T> void writeInternal(const T& op)
    {
        *expand<T>() = op;
    }

    /// Set channel information.
    ///
    /// \param channel Channel to set.
    /// \param offset Offset into the channel.
    void setChannel(GeometryChannel channel, unsigned offset)
    {
        for(const auto& vv : m_channels)
        {
            if(vv.getSemantic() == channel)
            {
                return;
            }
        }
        m_channels.emplace_back(channel, offset);
    }

public:
    /// Accessor.
    ///
    /// \return Current index buffer offset (for next append).
    constexpr unsigned getIndexOffset() const noexcept
    {
        return m_index_data.size();
    }

    /// Accessor.
    ///
    /// \return Current vertex buffer offset (for next append).
    constexpr unsigned getVertexOffset() const noexcept
    {
        return m_vertex_data.getSizeBytes();
    }

    /// Accessor.
    ///
    /// \return Vertex count.
    constexpr unsigned getVertexCount() const noexcept
    {
        return m_vertex_count;
    }

    /// Tell if given settings match this mesh data.
    ///
    /// \param op Other mesh data.
    constexpr bool matches(const MeshData& op) const noexcept
    {
        return (m_channels == op.m_channels) && (m_stride == op.stride);
    }

    /// End vertex input.
    ///
    /// Sets stride to current vertex data size, if it's not yet set.
    constexpr void endVertex() noexcept
    {
        if(!m_stride)
        {
            m_stride = static_cast<GLsizei>(m_vertex_data.size());
        }
        ++m_vertex_count;
    }

    /// Write index data.
    ///
    /// \param op Index.
    void write(uint16_t op)
    {
        m_index_data.push_back(op);
    }

    /// Write vertex data with semantic.
    ///
    /// \param channel Associated channel.
    /// \param data Geometry data.
    void write(GeometryChannel channel, const vec2& data)
    {
        setChannel(channel, getVertexOffset());
        writeInternal(op[0]);
        writeInternal(op[1]);
    }

    /// Write vertex data with semantic.
    ///
    /// \param channel Associated channel.
    /// \param data Geometry data.
    void write(GeometryChannel channel, const vec3& op)
    {
        setChannel(channel, getVertexOffset());
        writeInternal(op[0]);
        writeInternal(op[1]);
        writeInternal(op[2]);
    }

    /// Write vertex data with semantic.
    ///
    /// \param channel Associated channel.
    /// \param data Geometry data.
    void write(GeometryChannel channel, const uvec4& op)
    {
        setChannel(channel, getVertexOffset());
        writeInternal(op[0]);
        writeInternal(op[1]);
        writeInternal(op[3]);
        writeInternal(op[4]);
    }

    /// Append another mesh data block into this.
    ///
    /// \param op Another data block.
    void append(const MeshData& op)
    {
#if defined(USE_LD)
        if((m_vertex_count + op.getVertexCount()) > 0xFFFFu)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("trying to merge mesh data sets beyond 16 bit index scope"));
        }
#endif
        uint16_t index_offset = static_cast<uint16_t>(m_vertex_count);

        for(const auto& vv : op.m_vertex_data)
        {
            m_vertex_data.push_back(vv);
        }

        for(const auto& vv : op.m_index_data)
        {
            m_index_data.push_back(vv + index_offset);
        }
    }

    /// Update to GPU.
    ///
    /// \param vertex_buffer Vertex buffer.
    /// \param index_buffer Index buffer.
    void update(const VertexBuffer& vertex_buffer, const IndexBuffer& index_buffer) const
    {
        vertex_buffer.update(m_vertex_data);
        index_buffer.update(m_index_data);
    }
    /// Update vertex subdata GPU.
    ///
    /// \param vertex_buffer Vertex buffer.
    /// \param vertex_offset Offset into vertex buffer.
    void update(const VertexBuffer& vertex_buffer, unsigned vertex_offset) const
    {
        vertex_buffer.update(m_vertex_data, vertex_offset);
    }
};

}

#endif
