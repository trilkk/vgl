#ifndef VGL_MESH_DATA_HPP
#define VGL_MESH_DATA_HPP

#include "vgl_bitset.hpp"
#include "vgl_buffer.hpp"
#include "vgl_geometry_handle.hpp"
#include "vgl_glsl_program.hpp"
#include "vgl_state.hpp"
#include "vgl_vec2.hpp"
#include "vgl_vec3.hpp"
#include "vgl_uvec4.hpp"

namespace vgl
{

/// \cond
class MeshData;
/// \endcond

namespace detail
{

/// \cond
void geometry_handle_update_mesh_data(const GeometryHandle&, const MeshData&);
/// \endcond

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
        const GeometryChannel m_semantic;

        /// Number of elements in this channel.
        const GLint m_element_count;

        /// Element type for this channel.
        const GLenum m_type;

        /// Offset of the channel.
        const unsigned m_offset;

    public:
        /// Constructor.
        ///
        /// \param channel Channel ID.
        /// \param offset Offset of the channel.
        constexpr explicit ChannelInfo(GeometryChannel channel, unsigned offset) noexcept :
            m_semantic(channel),
            m_element_count(detail::geometry_channel_element_count(channel)),
            m_type(detail::geometry_channel_element_type(channel)),
            m_offset(offset)
        {
        }

    public:
        /// Accessor.
        ///
        /// \return Semantic.
        constexpr GeometryChannel getSemantic() const noexcept
        {
            return m_semantic;
        }

        /// Bind for drawing.
        ///
        /// \param prog Program to bind with.
        /// \param stride Stride for binding.
        int bind(const GlslProgram& prog, GLsizei stride) const
        {
            int ret = prog.getAttribLocation(m_semantic);
            if(ret >= 0)
            {
                unsigned idx = static_cast<unsigned>(ret);            
                attrib_array_enable(idx);
                dnload_glVertexAttribPointer(idx, m_element_count, m_type, GL_FALSE, stride,
                        reinterpret_cast<void*>(m_offset));
            }
            return ret;
        }

    public:
        /// Equals operator.
        ///
        /// \param op Other channel info.
        constexpr bool operator==(const ChannelInfo& op) const noexcept
        {
            return (m_semantic == op.m_semantic) &&
                (m_element_count == op.m_element_count) &&
                (m_type == op.m_type) &&
                (m_offset == op.m_offset);
        }

        /// Not equals operator.
        ///
        /// \param op Other channel info.
        constexpr bool operator!=(const ChannelInfo& op) const noexcept
        {
            return !(*this == op);
        }

#if defined(USE_LD)
        /// Stream output operator.
        ///
        /// \param lhs Left-hand-side operand.
        /// \param rhs Right-hand-side operand.
        /// \return Output stream.
        friend std::ostream& operator<<(std::ostream& lhs, const ChannelInfo& rhs)
        {
            return lhs << to_string(rhs.m_semantic) << ": " << rhs.m_element_count << ", " << rhs.m_type << ", " <<
                rhs.m_offset;
        }
#endif
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

    /// Copy constructor.
    ///
    /// \param op Source mesh data.
    MeshData(const MeshData& op) :
        m_stride(op.m_stride),
        m_vertex_count(op.m_vertex_count)
    {
        for(const auto& vv : op.m_vertex_data)
        {
            m_vertex_data.push_back(vv);
        }

        for(const auto& vv : op.m_index_data)
        {
            m_index_data.push_back(vv);
        }

        for(const auto& vv : op.m_channels)
        {
            m_channels.push_back(vv);
        }
    }

private:
    /// Expand data with the size of a type.
    ///
    /// \return Pointer to where the value can be written.
    template<typename T> T* expand()
    {
        for(unsigned ii = 0; (ii < sizeof(T)); ++ii)
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
    /// \return Size of index buffer.
    constexpr unsigned getIndexCount() const noexcept
    {
        return m_index_data.size();
    }

    /// Accessor.
    ///
    /// \return Offset at the end of index buffer.
    constexpr unsigned getIndexOffset() const noexcept
    {
        return m_index_data.getSizeBytes();
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
        return (m_channels == op.m_channels) && (m_stride == op.m_stride);
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
        writeInternal(data[0]);
        writeInternal(data[1]);
    }

    /// Write vertex data with semantic.
    ///
    /// \param channel Associated channel.
    /// \param data Geometry data.
    void write(GeometryChannel channel, const vec3& data)
    {
        setChannel(channel, getVertexOffset());
        writeInternal(data[0]);
        writeInternal(data[1]);
        writeInternal(data[2]);
    }

    /// Write vertex data with semantic.
    ///
    /// \param channel Associated channel.
    /// \param data Geometry data.
    void write(GeometryChannel channel, const uvec4& data)
    {
        setChannel(channel, getVertexOffset());
        writeInternal(data[0]);
        writeInternal(data[1]);
        writeInternal(data[2]);
        writeInternal(data[3]);
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
            m_index_data.push_back(static_cast<uint16_t>(vv + index_offset));
        }
    }

    /// Bind the attributes in this mesh data.
    ///
    /// \param op Program to bind with.
    void bindAttributes(const GlslProgram& op) const
    {
#if defined(USE_LD) && defined(DEBUG)
        bitset<detail::MAX_ATTRIB_ARRAYS> bound_attributes;
#else
        unsigned disable_attribs = 0;
#endif

        for(const auto& vv : m_channels)
        {
            int idx = vv.bind(op, m_stride);
            if(idx >= 0)
            {
#if defined(USE_LD) && defined(DEBUG)
                bound_attributes[idx] = true;
#else
                disable_attribs = max(disable_attribs, static_cast<unsigned>(idx) + 1);
#endif
            }
        }

#if defined(USE_LD) && defined(DEBUG)
        unsigned disable_attribs = 0;
        optional<unsigned> disabled_location;
        for(unsigned ii = 0; (ii < detail::MAX_ATTRIB_ARRAYS); ++ii)
        {
            if(bound_attributes[ii])
            {
                if(disabled_location)
                {
                    BOOST_THROW_EXCEPTION(std::runtime_error("attribute binding gap at " +
                                std::to_string(*disabled_location)));
                }
                disable_attribs = ii + 1;
            }
            else
            {
                disabled_location = ii;
            }
        }
#endif
        disable_excess_attrib_arrays(disable_attribs);
    }

    /// Update to GPU.
    ///
    /// \param vertex_buffer Vertex buffer.
    /// \param index_buffer Index buffer.
    void update(const VertexBuffer& vertex_buffer, const IndexBuffer& index_buffer) const
    {
        vertex_buffer.update(m_vertex_data);
        index_buffer.update(m_index_data);
#if defined(USE_LD)
        increment_data_size_vertex(m_vertex_data.getSizeBytes());
        increment_data_size_index(m_index_data.getSizeBytes());
#endif
    }
    /// Update vertex subdata GPU.
    ///
    /// \param vertex_buffer Vertex buffer.
    /// \param vertex_offset Offset into vertex buffer.
    void update(const VertexBuffer& vertex_buffer, unsigned vertex_offset) const
    {
        vertex_buffer.update(m_vertex_data, vertex_offset);
    }

    /// Update to GPU.
    ///
    /// \param op Handle describing update location.
    void update(const GeometryHandle& op) const
    {
        detail::geometry_handle_update_mesh_data(op, *this);
    }
};

}

#endif
