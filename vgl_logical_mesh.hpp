#ifndef VGL_LOGICAL_MESH_HPP
#define VGL_LOGICAL_MESH_HPP

#include "vgl_logical_vertex.hpp"
#include "vgl_mesh.hpp"
#include "vgl_task_dispatcher.hpp"

namespace vgl
{

/// \cond
class LogicalMesh;
/// \endcond

namespace detail
{

/// \cond
void csg_read_data(LogicalMesh&, const int16_t*);
void csg_read_raw(LogicalMesh&, const int16_t*, const uint8_t*, const uint16_t*, unsigned, unsigned, unsigned, float);
void csg_read_raw(LogicalMesh&, const int16_t*, const uint16_t*, unsigned, unsigned, float);
/// \endcond

}

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
    constexpr explicit LogicalMesh() = default;

    /// Constructor using CSG elements.
    ///
    /// \param op Input data for CSG construction.
    explicit LogicalMesh(const int16_t* op)
    {
        detail::csg_read_data(*this, op);
    }

    /// Constructor using raw data.
    ///
    /// \param vertices Vertex input.
    /// \param bones Bone input.
    /// \param faces Face input.
    /// \param vertices_amount Vertex data element count.
    /// \param bones_amount Bone data element count.
    /// \param faces_amount Face data element count.
    /// \param scale Scale to multiply with.
    explicit LogicalMesh(const int16_t *vertices, const uint8_t *bones, const uint16_t* faces,
            unsigned vertices_amount, unsigned bones_amount, unsigned faces_amount, float scale)
    {
        detail::csg_read_raw(*this, vertices, bones, faces, vertices_amount, bones_amount, faces_amount, scale);
    }
    /// Constructor using raw data.
    ///
    /// No bone data.
    ///
    /// \param vertices Vertex input.
    /// \param faces Face input.
    /// \param vertices_amount Vertex data element count.
    /// \param faces_amount Face data element count.
    /// \param scale Scale to multiply with.
    explicit LogicalMesh(const int16_t *vertices, const uint16_t* faces,
            unsigned vertices_amount, unsigned faces_amount, float scale)
    {
        detail::csg_read_raw(*this, vertices, nullptr, faces, vertices_amount, 0, faces_amount, scale);
    }

private:
    /// Add face (internal).
    ///
    /// \param op Face to add.
    /// \return Index of face added.
    unsigned addFaceInternal(LogicalFace&& op)
    {
        unsigned ret = getLogicalFaceCount();
        m_faces.push_back(move(op));
        return ret;
    }

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
        m_vertices[idx].removeFaceReference(face);
        unsigned ret = cloneVertex(idx);
        m_vertices[ret].addFaceReference(face);
        bool success = face.replaceVertexIndex(idx, ret);
#if defined(USE_LD)
        if(!success)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("replacing a new vertex turned a face degenerate"));
        }
#else
        (void)success;
#endif
        return ret;
    }

    /// Tells whether a vertex at given index is orphaned.
    ///
    /// \param op Index of the vertex.
    /// \return True if orphaned, false if in use.
    bool isOrphanedVertex(unsigned op)
    {
        return (m_vertices[op].getFaceReferences().size() <= 0);
    }

    /// Erase orphaned vertex at given index.
    ///
    /// \param op Vertex index.
    void eraseOrphanedVertex(unsigned op)
    {
#if defined(USE_LD) && defined(DEBUG)
        if(!isOrphanedVertex(op))
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("cannot erase non-orphaned vertex " + std::to_string(op)));
        }
#endif
        if((op + 1) < m_vertices.size())
        {
            m_vertices[op] = move(m_vertices.back());
            unsigned erase_count = replaceVertexIndex(m_vertices.size() - 1, op);
#if defined(USE_LD)
            if(erase_count)
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("erasing orphaned vertex turned a face degenerate"));
            }
#else
            (void)erase_count;
#endif
        }
        m_vertices.pop_back();
    }

    /// Replaces vertex index.
    ///
    /// Erases faces that become degenerate as a consequence.
    ///
    /// \param src Source vertex index.
    /// \param dst Destination vertex index.
    /// \return Number of faces erased by the process.
    unsigned replaceVertexIndex(unsigned src, unsigned dst)
    {
        unsigned ret = 0;

        for(unsigned ii = 0; (ii < m_faces.size());)
        {
            LogicalFace& face = m_faces[ii];

            if(!face.replaceVertexIndex(src, dst))
            {
                if((ii + 1) < m_faces.size())
                {
                    face = move(m_faces.back());
                }
                m_faces.pop_back();
                ++ret;
            }
            else
            {
                ++ii;
            }
        }

#if defined(USE_LD)
        // Clear face references to mark vertex really as orphan.
        LogicalVertex& srcVertex = m_vertices[src];
        srcVertex.clearFaceReferences();
#endif

        return ret;
    }
        
public:
    /// Add face.
    ///
    /// \param args Arguments
    /// \return Index of vertex added.
    template<typename...Args> unsigned addFace(Args&&...args)
    {
        return addFaceInternal(LogicalFace(args...));
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

    /// Compiles the logical mesh into a mesh.
    ///
    /// \param removeIdentical Flag determining whether to perform the identical vertex erase pass (default: true).
    /// \return Mesh.
    MeshUptr compile(bool removeIdentical = true)
    {
#if defined(USE_LD)
        // Clear all face references to destroy state.
        for(auto& vv : m_vertices)
        {
            vv.clearFaceReferences();
        }
#endif

        // First, calculate normals for all faces and duplicate vertices as needed.
        for(auto& face : m_faces)
        {
            for(unsigned ii = 0; (ii < face.getNumCorners()); ++ii)
            {
                unsigned vidx = face.getIndex(ii);
                LogicalVertex* vertex = &(m_vertices[vidx]);

                // Face references this vertex.
                vertex->addFaceReference(face);

                // Calculate normal and create a new vertex if face flatness decision requires it.
                calculateNormal(face);
                {
                    optional<vec3> vertex_nor = vertex->getNormal();
                    optional<vec3> face_nor = face.isFlat() ? optional<vec3>(face.getNormal()) : nullopt;
                    if(static_cast<bool>(vertex_nor) != static_cast<bool>(face_nor))
                    {
                        // Apply normal if face was flat.
                        if(!vertex_nor)
                        {
                            vertex->setNormal(*face_nor);
                        }
                    }
                    else if(!almost_equal(vertex_nor, face_nor))
                    {
                        vidx = cloneVertexForFace(face, vidx);
                        vertex = &(m_vertices[vidx]);
                        vertex->setNormal(*face_nor);
                    }
                }
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
                    else if(!almost_equal(vertex_tc, face_tc))
                    {
                        vidx = cloneVertexForFace(face, vidx);
                        vertex = &(m_vertices[vidx]);
                        vertex->setTexcoord(*face_tc);
                    }
                }
            }
        }

        // Loop through vertices, and remove vertices that are not used by any face.
        for(unsigned ii = 0; (ii < m_vertices.size());)
        {
            if(isOrphanedVertex(ii))
            {
                eraseOrphanedVertex(ii);
            }
            else
            {
                ++ii;
            }
        }
        
        // Loop through vertices and remove identical ones.
        if(removeIdentical)
        {
            for(unsigned ii = 0; (ii < m_vertices.size()); ++ii)
            {
                LogicalVertex& vertex = m_vertices[ii];

                for(unsigned jj = ii + 1; (jj < m_vertices.size());)
                {
                    if(vertex.matches(m_vertices[jj]))
                    {
                        replaceVertexIndex(jj, ii);
                        eraseOrphanedVertex(jj);
                    }
                    else
                    {
                        ++jj;
                    }
                }
            }
        }

        Fence ret = task_wait_main(taskfunc_create_mesh, this);
        return MeshUptr(static_cast<Mesh*>(ret.getReturnValue()));
    }

    /// Create a mesh from the data in this logical mesh.
    ///
    /// \return Pointer to new mesh.
    Mesh* createMesh()
    {        
        Mesh* ret = new Mesh();

        /// Write vertex data.
        {
#if defined(USE_LD)
            bitset<GeometryChannel::COUNT> channels;
#endif

            for(auto& vertex : m_vertices)
            {
#if defined(USE_LD)
                bitset<GeometryChannel::COUNT> written =
#endif
                    vertex.write(*ret);
#if defined(USE_LD)
                if(channels && (channels != written))
                {
                    BOOST_THROW_EXCEPTION(std::runtime_error("channel mismatch between vertices"));
                }
#endif
            }
        }
        
        for(auto& face : m_faces)
        {
            face.write(*ret);
        }

        // Update to GPU before returning.
        ret->update();
        return ret;
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

private:
    /// Export a mesh from this logical mesh.
    ///
    /// \param op Pointer to logical mesh.
    static void* taskfunc_create_mesh(void* op)
    {
        return static_cast<LogicalMesh*>(op)->createMesh();
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
