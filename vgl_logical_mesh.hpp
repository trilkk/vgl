#ifndef VGL_LOGICAL_MESH_HPP
#define VGL_LOGICAL_MESH_HPP

#include "vgl_logical_vertex.hpp"
#include "vgl_mesh.hpp"
#include "vgl_task_dispatcher.hpp"

namespace vgl
{

#if !defined(VGL_DISABLE_CSG)
/// CSG bit.
static const unsigned CSG_FLAG_NO_LEFT = 0;
/// CSG flag.
static const unsigned CSG_NO_LEFT = 1 << CSG_FLAG_NO_LEFT;

/// CSG bit.
static const unsigned CSG_FLAG_NO_RIGHT = 1;
/// CSG flag.
static const unsigned CSG_NO_RIGHT = 1 << CSG_FLAG_NO_RIGHT;

/// CSG bit.
static const unsigned CSG_FLAG_NO_BOTTOM = 2;
/// CSG flag.
static const unsigned CSG_NO_BOTTOM = 1 << CSG_FLAG_NO_BOTTOM;

/// CSG bit.
static const unsigned CSG_FLAG_NO_TOP = 3;
/// CSG flag.
static const unsigned CSG_NO_TOP = 1 << CSG_FLAG_NO_TOP;

/// CSG bit.
static const unsigned CSG_FLAG_NO_BACK = 4;
/// CSG flag.
static const unsigned CSG_NO_BACK = 1 << CSG_FLAG_NO_BACK;

/// CSG bit.
static const unsigned CSG_FLAG_NO_FRONT = 5;
/// CSG flag.
static const unsigned CSG_NO_FRONT = 1 << CSG_FLAG_NO_FRONT;

/// CSG bit: flat shading (as opposed to smooth.
static const unsigned CSG_FLAG_FLAT = 6;
/// CSG flag.
static const unsigned CSG_FLAT = 1 << CSG_FLAG_FLAT;

/// CSG flag count.
static const unsigned CSG_FLAG_COUNT = 7;

/// CSG flag set type.
using CsgFlags = bitset<CSG_FLAG_COUNT>;

/// CSG command enumeration.
///
/// In base cgl namespace so it's easier to use.
///
/// Unless specifically mentioned, the coordinate values are multiplied by 100.
/// Other multipliers are signified by the multiplier in parenthesis.
///
/// "Up vector (1/4)" signifies either:
/// One value, 1, 2 or 3, or the same values negative - unit vector on given axis, negative for negative direction.
/// Or 0, followed by direction vector verbatim.
enum class CsgCommand : int16_t
{
    /// Stop reading.
    NONE = 0,

    /// Vertex (position only).
    /// - Position (3).
    VERTEX,

    /// Triangle face.
    /// - Corner indices (3).
    TRIANGLE,

    /// Triangle face (with texcoord).
    /// - Corner index.
    /// - Texcoords (2).
    /// - Corner index.
    /// - Texcoords (2).
    /// - Corner index.
    /// - Texcoords (2).
    TRIANGLE_TC,

    /// Quad face.
    /// - Corner indices (4).
    QUAD,

    /// Quad face (with texcoord).
    /// - Corner index.
    /// - Texcoords (2).
    /// - Corner index.
    /// - Texcoords (2).
    /// - Corner index.
    /// - Texcoords (2).
    /// - Corner index.
    /// - Texcoords (2).
    QUAD_TC,

    /// Box.
    /// - Start position (3).
    /// - End position (3).
    /// - Up vector (1/4).
    /// - Width.
    /// - Height.
    /// - CSG flags.
    BOX,

    /// Trapezoid (chain).
    /// - Number of points.
    /// - Points / sizes (3 + 2 each).
    /// - Forward vector (1/4).
    /// - Up vector (1/4).
    /// - CSG flags.
    TRAPEZOID,

    /// Cone.
    /// - Start position (3).
    /// - End position (3).
    /// - Forward vector (1/4).
    /// - Up vector (1/4).
    /// - Fidelity.
    /// - Radius start.
    /// - Radius end.
    /// - CSG flags.
    CONE,

    /// Cylinder.
    /// - Start position (3).
    /// - End position (3).
    /// - Up vector (1/4).
    /// - Fidelity.
    /// - Radius.
    /// - CSG flags.
    CYLINDER,

    /// Pipe.
    /// - Number of points.
    /// - Points (3 each).
    /// - Fidelity.
    /// - Radius.
    /// - CSG flags.
    PIPE,

    /// Number of commands.
    COUNT,
};

/// conversion operator.
constexpr int16_t to_int16(CsgCommand op)
{
    return static_cast<int16_t>(op);
}
#endif

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

private:
#if defined(USE_LD)
    /// Number of vertices erased during logical mesh generation.
    static unsigned int g_vertices_erased;
#endif

public:
    /// Constructor.
    ///
    /// \param paint_color Face paint color (default: white).
    constexpr explicit LogicalMesh() = default;

#if !defined(VGL_DISABLE_CSG)
    /// Constructor using CSG elements.
    ///
    /// \param op Input data for CSG construction.
    explicit LogicalMesh(const int16_t* op)
    {
        csgReadData(op);
    }

    /// Constructor using raw data.
    ///
    /// \param vertices Vertex input.
    /// \param bones Bone input.
    /// \param faces Face input.
    /// \param vertices_amount Vertex data element count.
    /// \param faces_amount Face data element count.
    /// \param scale Scale to multiply with.
    explicit LogicalMesh(const int16_t *vertices, const uint8_t *bones, const uint16_t* faces,
            unsigned vertices_amount, unsigned faces_amount, float scale)
    {
        csgReadRaw(vertices, bones, faces, vertices_amount, faces_amount, scale);
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
        csgReadRaw(vertices, nullptr, faces, vertices_amount, faces_amount, scale);
    }
#endif

    /// Move constructor.
    ///
    /// \param other Source object.
    constexpr LogicalMesh(LogicalMesh&& other) noexcept :
        m_vertices(move(other.m_vertices)),
        m_faces(move(other.m_faces))
    {
    }

    /// Deleted copy constructor.
    LogicalMesh(const LogicalMesh&) = delete;
    /// Deleted copy operator.
    LogicalMesh& operator=(const LogicalMesh&) = delete;

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
            VGL_THROW_RUNTIME_ERROR("replacing a new vertex turned a face degenerate");
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
#if defined(USE_LD)
        if(!isOrphanedVertex(op))
        {
            VGL_THROW_RUNTIME_ERROR("cannot erase non-orphaned vertex " + to_string(op));
        }
        ++g_vertices_erased;
#endif
        if((op + 1) < m_vertices.size())
        {
            m_vertices[op] = move(m_vertices.back());
            unsigned erase_count = replaceVertexIndex(m_vertices.size() - 1, op);
#if defined(USE_LD)
            if(erase_count)
            {
                VGL_THROW_RUNTIME_ERROR("erasing orphaned vertex turned a face degenerate");
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

    /// Remove identical vertices.
    void removeIdenticalVertices()
    {
        for(unsigned ii = 0; (ii < m_vertices.size()); ++ii)
        {
            LogicalVertex& lhs = m_vertices[ii];

            for(unsigned jj = ii + 1; (jj < m_vertices.size());)
            {
                LogicalVertex& rhs = m_vertices[jj];

                if(lhs.matches(rhs))
                {
                    lhs.appendFaceReferences(rhs);
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

                // Calculate normals for flat faces and apply to face vertices.
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
                    // Clone vertices for flat faces if existing vertex normal does not match.
                    else if(face_nor && !almost_equal(*vertex_nor, *face_nor))
                    {
                        vidx = cloneVertexForFace(face, vidx);
                        vertex = &(m_vertices[vidx]);
                        vertex->setNormal(*face_nor);
                    }
                }
            }
        }

        // Merge vertices that are identical after creating normals for flat faces.
        if(removeIdentical)
        {
            removeIdenticalVertices();
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
            removeIdenticalVertices();
        }

        Fence ret = TaskDispatcher::wait_main(task_create_mesh, this);
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
                    VGL_THROW_RUNTIME_ERROR("channel mismatch between vertices");
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

#if !defined(VGL_DISABLE_CSG)
    /// Create a trapezoid (chain) shape.
    ///
    /// \param lmesh Target logical mesh.
    /// \param points Point array.
    /// \param sizes Size array.
    /// \param count Number of points, should be at least 3.
    /// \param dir Face direction.
    /// \param up Up direction.
    /// \param width1 Width of back face.
    /// \param height1 Height of back face.
    /// \param width2 Width of front face.
    /// \param height2 Height of front face.
    /// \param flags CSG flags.
    void csgTrapezoid(const vec3* points, const vec2* sizes, unsigned count, const vec3& param_dir, const vec3& param_up,
            CsgFlags flags = CsgFlags(0));
    /// Create a box shape.
    ///
    /// Box is a specialization of trapezoid.
    ///
    /// \param lmesh Target logical mesh.
    /// \param p1 Center of front face.
    /// \param p2 Center of back face.
    /// \param param_up Up direction.
    /// \param width Width.
    /// \param height Height.
    /// \param flags CSG flags.
    void csgBox(const vec3& p1, const vec3& p2, const vec3& param_up, float width, float height, CsgFlags flags = CsgFlags(0))
    {
        // Data arrays for trapezoid.
        vec3 points[2] = { p1, p2 };
        vec2 bsize(width, height);
        vec2 sizes[2] = { bsize, bsize };
        // Ensure up is actually perpendicular to fw.
        // Do not normalize to prevent degradation of precision.
        vec3 fw = p2 - p1;
        vec3 rt = cross(fw, param_up);
        vec3 up = cross(rt, fw);
        // Pass to trapezoid.
        csgTrapezoid(points, sizes, 2, fw, up, flags);
    }

    /// Create a cone shape.
    ///
    /// \param lmesh Target logical mesh.
    /// \param p1 Starting point.
    /// \param p2 End point.
    /// \param param_fw Forward direction.
    /// \param param_up Up direction.
    /// \param fidelity Fidelity of the cylinder, should be at least 3.
    /// \param radius1 Radius of cone front.
    /// \param radius2 Radius of cone back.
    /// \param flags CSG flags.
    void csgCone(const vec3& p1, const vec3& p2, const vec3 param_fw, const vec3& param_up, unsigned fidelity, float radius1,
            float radius2, CsgFlags flags = CsgFlags(0));
    /// Create a cylinder shape.
    ///
    /// Cylinder is an specialization of cone.
    ///
    /// \param lmesh Target logical mesh.
    /// \param p1 Starting point.
    /// \param p2 End point.
    /// \param param_up Up direction.
    /// \param fidelity Fidelity of the cylinder, should be at least 3.
    /// \param radius Radius of the cylinder.
    /// \param flags CSG flags.
    void csgCylinder(const vec3& p1, const vec3& p2, const vec3& param_up, unsigned fidelity, float radius,
            CsgFlags flags = CsgFlags(0))
    {
        // Pass to cone.
        vec3 fw = p2 - p1;
        csgCone(p1, p2, fw, param_up, fidelity, radius, radius, flags);
    }

    /// Create a pipe shape.
    ///
    /// Very sharp angles and spirals generate degenerate geometry.
    ///
    /// \param lmesh Target logical mesh.
    /// \param points Point array.
    /// \param count Number of points, should be at least 3.
    /// \param fidelity Fidelity of the cylinder, should be at least 3.
    /// \param radius Radius of the cylinder.
    /// \param flags CSG flags.
    void csgPipe(const vec3* points, unsigned count, unsigned fidelity, float radius, CsgFlags flags = CsgFlags(0));

    /// Read packed data.
    ///
    /// \param msh Mesh to store forms to.
    /// \param data Data to read.
    void csgReadData(const int16_t* data);

    /// Add raw model data.
    ///
    /// Bone input is arranged as 3 weights and 3 references.
    /// The 3 weights should add up to 255.
    ///
    /// \param vertices Vertex input.
    /// \param bones Bone weight and reference input.
    /// \param faces Face input.
    /// \param vertices_amount Vertex data element count.
    /// \param faces_amount Face data element count.
    /// \param scale Scale to multiply with.
    void csgReadRaw(const int16_t *vertices, const uint8_t *bones, const uint16_t* faces, unsigned vertices_amount,
            unsigned faces_amount, float scale);
    /// Add raw model data.
    ///
    /// Bone data is not added.
    ///
    /// \param vertices Vertex input.
    /// \param faces Face input.
    /// \param vertices_amount Vertex data element count.
    /// \param faces_amount Face data element count.
    /// \param scale Scale to multiply with.
    void csgReadRaw(const int16_t *vertices, const uint16_t* faces, unsigned vertices_amount, unsigned faces_amount,
            float scale)
    {
        csgReadRaw(vertices, nullptr, faces, vertices_amount, faces_amount, scale);
    }
#endif

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
    static void* task_create_mesh(void* op)
    {
        return static_cast<LogicalMesh*>(op)->createMesh();
    }

public:
#if defined(USE_LD)
    /// Move operator.
    ///
    /// \param other Source object.
    /// \return This object.
    LogicalMesh& operator=(LogicalMesh&& other)
    {
        m_vertices = move(other.m_vertices);
        m_faces = move(other.m_faces);
        return *this;
    }
#endif

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

#if !defined(USE_LD)
#include "vgl_logical_mesh.cpp"
#endif

#endif
