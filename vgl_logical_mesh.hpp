#ifndef VGL_LOGICAL_MESH_HPP
#define VGL_LOGICAL_MESH_HPP

#include "vgl_logical_vertex.hpp"
#include "vgl_mesh.hpp"
#include "vgl_task_queue.hpp"

namespace vgl
{

/// Logical mesh.
///
/// Not an actual renderable mesh. Must be compiled and then uploaded to GPU.
class LogicalMesh
{
private:
    /// Logical vertex data.
    vector<LogicalVertex> m_vertices;

    /// Logical face data.
    vector<LogicalFace> m_faces;

public:
    /// Constructor.
    ///
    /// \param paint_color Face paint color (default: white).
    explicit constexpr LogicalMesh() = default;

private:
    /// Add vertex (internal).
    ///
    /// \param op Vertex to add.
    /// \return Index of vertex added.
    unsigned addVertexInternal(LogicalVertex&& op)
    {
        unsigned ret = getLogicalVertexCount();
        m_vertices.push_back(move(op));
        return ret;
    }

public:
    /// Add face.
    ///
    /// \param c1 First corner point.
    /// \param c2 Second corner point.
    /// \param c3 Third corner point.
    /// \param col Face color.
    /// \param flat Is the face flat?
    unsigned addFace(unsigned c1, unsigned c2, unsigned c3, const optional<uvec4>& col = nullopt, bool flat = false)
    {
        unsigned ret = getLogicalFaceCount();
        m_faces.emplace_back(c1, c2, c3, col, flat);
        return ret;
    }
    /// Add face.
    ///
    /// \param c1 First corner point.
    /// \param tc1 First texcoord.
    /// \param c2 Second corner point.
    /// \param tc2 Second texcoord.
    /// \param c3 Third corner point.
    /// \param tc3 Third texcoord.
    /// \param col Face color.
    /// \param flat Is the face flat?
    unsigned addFace(unsigned c1, const vec2& tc1, unsigned c2, const vec2& tc2, unsigned c3,
            const vec2& tc3, const optional<uvec4>& col = nullopt, bool flat = false)
    {
        unsigned ret = getLogicalFaceCount();
        m_faces.emplace_back(c1, tc1, c2, tc2, c3, tc3, col, flat);
        return ret;
    }
    /// Add face.
    ///
    /// \param c1 First corner point.
    /// \param c2 Second corner point.
    /// \param c3 Third corner point.
    /// \param c4 Fourth corner point.
    /// \param col Face color.
    /// \param flat Is the face flat?
    unsigned addFace(unsigned c1, unsigned c2, unsigned c3, unsigned c4, const optional<uvec4>& col = nullopt,
            bool flat = false)
    {
        unsigned ret = getLogicalFaceCount();
        m_faces.emplace_back(c1, c2, c3, c4, col, flat);
        return ret;
    }
    /// Add face.
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
    unsigned addFace(unsigned c1, const vec2& tc1, unsigned c2, const vec2& tc2, unsigned c3, const vec2& tc3,
            unsigned c4, const vec2& tc4, const optional<uvec4>& col = nullopt, bool flat = false)
    {
        unsigned ret = getLogicalFaceCount();
        m_faces.emplace_back(c1, tc1, c2, tc2, c3, tc3, c4, tc4, col, flat);
        return ret;
    }

    /// Add vertex.
    ///
    /// \param args Arguments
    /// \return Index of vertex added.
    template<typename...Args> unsigned addVertex(Args&&...args)
    {
        return addVertexInternal(LogicalVertex(args...));
    }

    /// Calculate a normal for a face.
    ///
    /// \param face Face to calculate for.
    constexpr void calculateNormal(LogicalFace& face) const
    {
        const LogicalVertex& v0 = m_vertices[face.getIndex(0)];
        const LogicalVertex& v1 = m_vertices[face.getIndex(1)];
        const LogicalVertex& v2 = m_vertices[face.getIndex(2)];

        vec3 l1 = v1.getPosition() - v0.getPosition();
        vec3 l2 = v2.getPosition() - v0.getPosition();

        face.setNormal(cross(l1, l2));
    }

    /// Clones a vertex.
    ///
    /// \param idx Index to clone with.
    /// \return New vertex index.
    unsigned cloneVertex(unsigned idx)
    {
        unsigned ret = m_vertices.size();
        m_vertices.push_back(m_vertices[idx].clone());
        return ret;
    }
    /// Clones a vertex by index for a face.
    ///
    /// \param face Face to clone for.
    /// \param idx Index of vertex to clone.
    /// \return New index.
    unsigned cloneVertexForFace(LogicalFace& face, unsigned idx)
    {
        unsigned ret = cloneVertex(idx);
        bool success = face.replaceVertexIndex(ret, idx);
#if defined(USE_LD)
        if(!success)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("replacing a new vertex turned face degenerate"));
        }
#else
        (void)success;
#endif
        return ret;
    }

    /// Compiles the logical mesh into a mesh.
    ///
    /// \return Mesh.
    MeshUptr compile()
    {
        // First, calculate normals for all faces and duplicate vertices as needed.
        for(auto& face : m_faces)
        {
            for(unsigned ii = 0; (ii < face.getNumCorners()); ++ii)
            {
                unsigned vidx = face.getIndex(ii);
                LogicalVertex* vertex = &(m_vertices[vidx]);

                // Calculate normal and create a new vertex if face flatness decision requires it.
                calculateNormal(face);
                {
                    optional<vec3> vertex_nor = vertex->getNormal();
                    optional<vec3> face_nor = face.isFlat() ? optional<vec3>(face.getNormal()) : nullopt;
                    if(!almost_equal(vertex_nor, face_nor))
                    {
                        vidx = cloneVertexForFace(face, vidx);
                        vertex = &(m_vertices[vidx]);

                        // Apply texcoord since it was not cloned.
                        optional<vec2> face_tc = face.getTexcoordForVertex(vidx);
                        if(face_tc)
                        {
                            vertex->setTexcoord(*face_tc);
                        }
                        // Apply normal if face was flat.
                        if(face_nor)
                        {
                            vertex->setNormal(*face_nor);
                        }
                    }
                }

                // Face now decidedly references this vertex.
                vertex->addFaceReference(&face);
            }
        }

        // Calculate normals for all vertices that were not parts of a flat face.
        for(auto& vertex : m_vertices)
        {
            vertex.calculateNormal();
        }

        // TODO: edge calculation goes here if it's implemented

        // Duplicate vertices for situations where further attribute data does not match.
        for(auto& face : m_faces)
        {
            for(unsigned ii = 0; (ii < face.getNumCorners()); ++ii)
            {
                unsigned vidx = face.getIndex(ii);
                LogicalVertex* vertex = &(m_vertices[vidx]);

                // Apply color from face or create a new vertex.
                {
                    optional<uvec4> vertex_col = vertex->getColor();
                    optional<uvec4> face_col = face.getColor();
                    if(static_cast<bool>(vertex_col) != static_cast<bool>(face_col))
                    {
                        if(!vertex_col)
                        {
                            vertex->setColor(*face_col);
                        }
                    }
                    else if(vertex_col != face_col)
                    {
                        vidx = cloneVertexForFace(face, vidx);
                        vertex = &(m_vertices[vidx]);
                        vertex->setColor(*face_col);
                    }
                }

                // Apply texcoord from face or create a new vertex.
                {
                    optional<vec2> vertex_tc = vertex->getTexcoord();
                    optional<vec2> face_tc = face.getTexcoordForVertex(vidx);
                    if(static_cast<bool>(vertex_tc) != static_cast<bool>(face_tc))
                    {
                        if(!vertex_tc)
                        {
                            vertex->setTexcoord(*face_tc);
                        }
                    }
                    else if(vertex_tc != face_tc)
                    {
                        vidx = cloneVertexForFace(face, vidx);
                        vertex = &(m_vertices[vidx]);
                        vertex->setTexcoord(*face_tc);
                    }
                }
            }
        }

        // All vertex data is set.
        // Loop through vertices and remove identical ones.
        for(unsigned ii = 0; (ii < m_vertices.size()); ++ii)
        {
            LogicalVertex& vertex = m_vertices[ii];

            for(unsigned jj = ii + 1; (jj < m_vertices.size()); ++jj)
            {
                if(vertex.matches(m_vertices[jj]))
                {
                    for(unsigned kk = 0; (kk < m_faces.size()); ++kk)
                    {
                        LogicalFace& face = m_faces[jj];
                        if(!face.replaceVertexIndex(ii, jj))
                        {
                            if((m_faces.size() - 1) > kk)
                            {
                                face = m_faces.back();
                            }
                            m_faces.pop_back();
                        }
                    }
                }
            }
        }

        // TODO: create the actual mesh from this data
        return nullptr;
    }

    /// Accessor.
    ///
    /// \param idx Index.
    /// \return Face at index.
    LogicalFace& getLogicalFace(unsigned idx)
    {
        return m_faces[idx];
    }
    /// Const accessor.
    ///
    /// \param idx Index.
    /// \return Face at index.
    const LogicalFace& getLogicalFace(unsigned idx) const
    {
        return m_faces[idx];
    }

    /// Accessor.
    ///
    /// \return Face count.
    unsigned getLogicalFaceCount() const
    {
        return static_cast<unsigned>(m_faces.size());
    }

    /// Get last added logical face.
    ///
    /// \return Previously added logical face.
    LogicalFace& getLogicalFaceLast()
    {
        return m_faces.back();
    }

    /// Accessor.
    ///
    /// \param idx Index.
    /// \return Vertex (not logical) at index.
    LogicalVertex& getLogicalVertex(unsigned idx)
    {
        return m_vertices[idx];
    }
    /// Const accessor.
    ///
    /// \param idx Index.
    /// \return Vertex (not logical) at index.
    const LogicalVertex& getLogicalVertex(unsigned idx) const
    {
        return m_vertices[idx];
    }

    /// Accessor.
    ///
    /// \return Logical vertex count.
    unsigned getLogicalVertexCount() const
    {
        return static_cast<unsigned>(m_vertices.size());
    }

public:
#if defined(USE_LD)
    /// Stream output operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const LogicalMesh& rhs)
    {
        return lhs << "LogicalMesh(" << &rhs << ')';
    }
#endif
};

}

#endif
