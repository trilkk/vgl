#ifndef VGL_LOGICAL_FACE_HPP
#define VGL_LOGICAL_FACE_HPP

#include "vgl_array.hpp"
#include "vgl_mesh.hpp"

namespace vgl
{

/// Logical face class.
///
/// Up to quads supported.
class LogicalFace
{
private:
    /// Corner count.
    unsigned m_num_corners;

    /// Corner points.
    array<unsigned, 4> m_indices;

    /// Corner texcoords.
    optional<array<vec2, 4>> m_texcoords;

    /// Face color.
    ///
    /// Overrides vertex color for referenced vertices if set.
    optional<uvec4> m_color;

    /// Face normal.
    vec3 m_normal;

    /// Is the face flat (as opposed to gouraud).
    bool m_flat = false;

    /// Block within the mesh.
    unsigned m_block = 0;

private:
    /// Deleted copy constructor.
    LogicalFace(const LogicalFace&) = delete;
    /// Deleted assignment.
    LogicalFace& operator=(const LogicalFace&) = delete;

public:
    /// Constructor.
    ///
    /// \param c1 First corner point.
    /// \param c2 Second corner point.
    /// \param c3 Third corner point.
    /// \param flat Is the face flat?
    explicit constexpr LogicalFace(unsigned c1, unsigned c2, unsigned c3, bool flat = false) noexcept :
        m_num_corners(3),
        m_indices{c1, c2, c3, 0u},
        m_flat(flat)
    {
    }

    /// Constructor.
    ///
    /// \param c1 First corner point.
    /// \param c2 Second corner point.
    /// \param c3 Third corner point.
    /// \param col Face color.
    /// \param flat Is the face flat?
    explicit constexpr LogicalFace(unsigned c1, unsigned c2, unsigned c3, const uvec4& col,
            bool flat = false) noexcept :
        m_num_corners(3),
        m_indices{c1, c2, c3, 0u},
        m_color(col),
        m_flat(flat)
    {
    }

    /// Constructor.
    ///
    /// \param c1 First corner point.
    /// \param tc1 First texcoord.
    /// \param c2 Second corner point.
    /// \param tc2 Second texcoord.
    /// \param c3 Third corner point.
    /// \param tc3 Third texcoord.
    /// \param flat Is the face flat?
    explicit constexpr LogicalFace(unsigned c1, const vec2& tc1, unsigned c2, const vec2& tc2, unsigned c3,
            const vec2& tc3, bool flat = false) noexcept :
        m_num_corners(3),
        m_indices{c1, c2, c3, 0u},
        m_texcoords(array<vec2, 4>{tc1, tc2, tc3, vec2(0.0f, 0.0f)}),
        m_flat(flat)
    {
    }

    /// Constructor.
    ///
    /// \param c1 First corner point.
    /// \param tc1 First texcoord.
    /// \param c2 Second corner point.
    /// \param tc2 Second texcoord.
    /// \param c3 Third corner point.
    /// \param tc3 Third texcoord.
    /// \param col Face color.
    /// \param flat Is the face flat?
    explicit constexpr LogicalFace(unsigned c1, const vec2& tc1, unsigned c2, const vec2& tc2, unsigned c3,
            const vec2& tc3, const uvec4& col, bool flat = false) noexcept :
        m_num_corners(3),
        m_indices{c1, c2, c3, 0u},
        m_texcoords(array<vec2, 4>{tc1, tc2, tc3, vec2(0.0f, 0.0f)}),
        m_color(col),
        m_flat(flat)
    {
    }

    /// Constructor.
    ///
    /// \param c1 First corner point.
    /// \param c2 Second corner point.
    /// \param c3 Third corner point.
    /// \param c4 Fourth corner point.
    /// \param flat Is the face flat?
    explicit constexpr LogicalFace(unsigned c1, unsigned c2, unsigned c3, unsigned c4, bool flat = false) noexcept :
        m_num_corners(4),
        m_indices{c1, c2, c3, c4},
        m_flat(flat)
    {
    }

    /// Constructor.
    ///
    /// \param c1 First corner point.
    /// \param c2 Second corner point.
    /// \param c3 Third corner point.
    /// \param c4 Fourth corner point.
    /// \param col Face color.
    /// \param flat Is the face flat?
    explicit constexpr LogicalFace(unsigned c1, unsigned c2, unsigned c3, unsigned c4, const uvec4& col,
            bool flat = false) noexcept :
        m_num_corners(4),
        m_indices{c1, c2, c3, c4},
        m_color(col),
        m_flat(flat)
    {
    }

    /// Constructor.
    ///
    /// \param c1 First corner point.
    /// \param tc1 First texcoord.
    /// \param c2 Second corner point.
    /// \param tc2 Second texcoord.
    /// \param c3 Third corner point.
    /// \param tc3 Third texcoord.
    /// \param c4 Fourth corner point
    /// \param tc4 Fourth corner point
    /// \param flat Is the face flat?
    explicit constexpr LogicalFace(unsigned c1, const vec2& tc1, unsigned c2, const vec2& tc2, unsigned c3,
            const vec2& tc3, unsigned c4, const vec2& tc4, bool flat = false) noexcept :
        m_num_corners(4),
        m_indices{c1, c2, c3, c4},
        m_texcoords(array<vec2, 4>{tc1, tc2, tc3, tc4}),
        m_flat(flat)
    {
    }

    /// Constructor.
    ///
    /// \param c1 First corner point.
    /// \param tc1 First texcoord.
    /// \param c2 Second corner point.
    /// \param tc2 Second texcoord.
    /// \param c3 Third corner point.
    /// \param tc3 Third texcoord.
    /// \param c4 Fourth corner point
    /// \param tc4 Fourth corner point
    /// \param col Face color.
    /// \param flat Is the face flat?
    explicit constexpr LogicalFace(unsigned c1, const vec2& tc1, unsigned c2, const vec2& tc2, unsigned c3,
            const vec2& tc3, unsigned c4, const vec2& tc4, const uvec4& col, bool flat = false) noexcept :
        m_num_corners(4),
        m_indices{c1, c2, c3, c4},
        m_texcoords(array<vec2, 4>{tc1, tc2, tc3, tc4}),
        m_color(col),
        m_flat(flat)
    {
    }

    /// Move constructor.
    ///
    /// \param op Source face.
    constexpr LogicalFace(LogicalFace&& op) :
        m_num_corners(op.m_num_corners),
        m_indices(op.m_indices),
        m_texcoords(op.m_texcoords),
        m_color(op.m_color),
        m_normal(op.m_normal),
        m_flat(op.m_flat),
        m_block(op.m_block)
    {
    }

private:
      /// Perform access check.
      ///
      /// Throws an error if the given index is too far into the optional value is not initialized.
      /// \param idx Index to check.
      constexpr void accessCheck(unsigned idx) const
      {
#if defined(USE_LD) && defined(DEBUG)
          if(idx >= m_num_corners)
          {
              BOOST_THROW_EXCEPTION(std::runtime_error("accessing index " + std::to_string(idx) + " of a " +
                          std::to_string(m_num_corners) + "-corner face"));
          }
#else
          (void)idx;
#endif
      }

      /// Remove a corner.
      ///
      /// \param idx Index to remove.
      constexpr void removeCorner(unsigned idx)
      {
#if defined(USE_LD) && defined(DEBUG)
          if(m_num_corners <= 3)
          {
              BOOST_THROW_EXCEPTION(std::runtime_error("cannot degrade triangle by removing a corner"));
          }
#endif
          --m_num_corners;
          for(unsigned ii = idx; (ii < m_num_corners); ++ii)
          {
              m_indices[ii] = m_indices[ii + 1];
          }
      }

public:
    /// Accessor.
    ///
    /// \return Face color.
    constexpr optional<uvec4> getColor() const noexcept
    {
        return m_color;
    }

    /// Accessor.
    ///
    /// \param idx Corner index.
    /// \return Vertex index at corner.
    constexpr unsigned getIndex(unsigned idx) const
    {
        accessCheck(idx);
        return m_indices[idx];
    }

    /// Accessor.
    ///
    /// \return Normal vector.
    constexpr vec3 getNormal() const noexcept
    {
        return m_normal;
    }
    /// Setter.
    ///
    /// \param op Normal vector.
    constexpr void setNormal(const vec3& op) noexcept
    {
        m_normal = op;
    }

    /// Accessor.
    ///
    /// \return Corner count.
    constexpr unsigned getNumCorners() const noexcept
    {
        return m_num_corners;
    }

    /// Accessor.
    ///
    /// \param idx Corner index.
    /// \return Texcoord at corner or nullopt.
    constexpr optional<vec2> getTexcoord(unsigned idx) const
    {
        accessCheck(idx);
        if(m_texcoords)
        {
            return (*m_texcoords)[idx];
        }
        return nullopt;
    }
    /// Setter.
    ///
    /// \param op Texture coordinate to set on all vertices.
    constexpr void setTexcoord(const vec2& op) noexcept
    {
        m_texcoords = array<vec2, 4>{op, op, op, op};
    }

    /// Get texcoord for given vertex index.
    ///
    /// If given vertex index is not found, an exception will be thrown.
    ///
    /// \param idx Vertex index.
    /// \return Texcoord for given vertex or nullopt.
    constexpr optional<vec2> getTexcoordForVertex(unsigned idx) noexcept
    {
        if(!m_texcoords)
        {
            return nullopt;
        }

        for(unsigned ii = 0; (ii < m_num_corners); ++ii)
        {
            if(m_indices[ii] == idx)
            {
                return (*m_texcoords)[ii];
            }
        }

        return nullopt;
    }

    /// Tell if the face is flat.
    ///
    /// \return True if flat, false if gouraud.
    constexpr bool isFlat() const noexcept
    {
        return m_flat;
    }

    /// Tell if the face is a quad.
    ///
    /// \return True if quad, false if triangle.
    constexpr bool isQuad() const noexcept
    {
        return (m_num_corners >= 4);
    }

    /// Replace vertex index.
    ///
    /// \param dst Destination index.
    /// \param src Source index.
    /// \return True if the face is still valid after the replace, false if not.
    constexpr bool replaceVertexIndex(unsigned dst, unsigned src)
    {
        for(unsigned ii = 0; (ii < m_num_corners); ++ii)
        {
            if(m_indices[ii] == src)
            {
                m_indices[ii] = dst;
            }
        }   

        return verify();
    }

    /// Try to repair any inconsistencies in the face.
    ///
    /// \return True if the face is appropriate.
    constexpr bool verify()
    {
        // Remove adjacent identical indices.
        for(unsigned ii = 0; (ii < m_num_corners);)
        {
            unsigned jj = ii + 1;
            if(jj >= m_num_corners)
            {
                jj = 0;
            }

            if(m_indices[ii] == m_indices[jj])
            {
                removeCorner(jj);
            }
            else
            {
                ++ii;
            }
        }

#if defined(USE_LD)
        if(isQuad() && ((m_indices[0] == m_indices[2]) || (m_indices[1] == m_indices[3])))
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("degenerate trapezoid quad"));
        }
#endif

        // Check for degeneracy.
        if(3 > m_num_corners)
        {
            return false;
        }
        return true;
    }

    /// Write face data to mesh.
    ///
    /// \param op Target mesh.
    void write(Mesh& op) const
    {
#if defined(USE_LD)
        if((m_num_corners != 3) && (m_num_corners != 4))
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("don't know how to write face with " +
                        std::to_string(m_num_corners) + " corners"));
        }
#endif
        op.write(static_cast<uint16_t>(m_indices[0]));
        op.write(static_cast<uint16_t>(m_indices[1]));
        op.write(static_cast<uint16_t>(m_indices[2]));

        // Write another triangle if this is a quad.
        if(isQuad())
        {
            op.write(static_cast<uint16_t>(m_indices[2]));
            op.write(static_cast<uint16_t>(m_indices[3]));
            op.write(static_cast<uint16_t>(m_indices[0]));
        }
    }

public:
    /// Move operator.
    ///
    /// \param op Source face.
    /// \return This object.
    constexpr LogicalFace& operator=(LogicalFace&& op) noexcept
    {
        m_num_corners = op.m_num_corners;
        m_indices = op.m_indices;
        m_texcoords = op.m_texcoords;
        m_color = op.m_color;
        m_normal = op.m_normal;
        m_flat = op.m_flat;
        m_block = op.m_block;
        return *this;
    }



public:
#if defined(USE_LD)
    /// Stream output operator.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const LogicalFace& rhs)
    {
        lhs << "[ " << rhs.getIndex(0) << " ; " << rhs.getIndex(1) << " ; " << rhs.getIndex(2);

        if(rhs.isQuad())
        {
            lhs << " ; " << rhs.getIndex(3);
        }

        return lhs << " ] | " << rhs.getNormal();
    }
#endif
};

}

#endif
