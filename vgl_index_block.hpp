#ifndef VERBATIM_INDEX_BLOCK
#define VERBATIM_INDEX_BLOCK

namespace vgl
{

/// One block of indices.
///
/// Defines one rendering sequence of primitives within one geometry buffer.
class IndexBlock
{
private:
    /// Primitive type.
    GLenum m_type = GL_TRIANGLES;

    /// Index count.
    unsigned m_count;

    /// First index offset.
    void* m_offset = nullptr;

  public:
    /// Constructor.
    ///
    /// Triangles will be rendered.
    /// Offset assumbed to be zero.
    ///
    /// \param count Number of elements to render.
    IndexBlock(unsigned count) :
        m_count(count)
    {
    }

    /// Constructor.
    ///
    /// \param type Type of elements to render.
    /// \param count Number of elements to render.
    IndexBlock(GLenum type, unsigned count) :
        m_type(type),
        m_count(count)
    {
    }

    /// Constructor.
    ///
    /// \param type Type of elements to render.
    /// \param count Number of elements to render.
    /// \param offset Offset into the buffer.
    IndexBlock(GLenum type, unsigned count, unsigned offset) :
        m_type(type),
        m_count(count),
        m_offset(static_cast<uint16_t*>(nullptr) + offset)
    {
    }

  public:
    /// Draw indexed geometry.
    void drawGeometry() const
    {
        dnload_glDrawElements(m_type, m_count, GL_UNSIGNED_SHORT, m_offset);
    }
};

}

#endif
