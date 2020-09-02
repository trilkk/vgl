#ifndef VGL_CSG_HPP
#define VGL_CSG_HPP

#include "vgl_logical_mesh.hpp"

namespace vgl
{

namespace detail
{

/// Normalizes a direction vector and ensures it's not perpendicular to another vector.
///
/// Direction vector does not need to be an unit vector, it is normalized.
///
/// \param dir Direction vector.
/// \param ref Reference vector.
vec3 perpendiculate(const vec3& dir, const vec3& ref)
{
    vec3 unit_dir = normalize(dir);

    // Rotate direction vector components once if it's perpendicular to the reference vector.
    float dot_result = dot(unit_dir, normalize(ref));
    if(abs(dot_result) >= 0.999f)
    {
        return vec3(unit_dir.z(), unit_dir.x(), unit_dir.y());
    }
    return unit_dir;
}

}

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

using CsgFlags = bitset<CSG_FLAG_COUNT>;

/// Create a box shape.
///
/// \param lmesh Target logical mesh.
/// \param p1 Center of front face.
/// \param p2 Center of back face.
/// \param width Width.
/// \param height Height.
/// \param flags CSG flags.
/// \param up Up direction.
void csg_box(LogicalMesh& lmesh, const vec3& p1, const vec3& p2, float width, float height,
        CsgFlags flags = CsgFlags(0), const vec3& param_up = vec3(0.0f, 1.0f, 0.0f))
{
    vec3 forward = p2 - p1;
    vec3 unit_up = perpendiculate(param_up, forward);
    vec3 rt = normalize(cross(forward, unit_up)) * (width * 0.5f);
    vec3 up = normalize(cross(rt, forward)) * (height * 0.5f);

    unsigned index_base = lmesh.getLogicalVertexCount();
    bool flat = flags[CSG_FLAG_FLAT];

    lmesh.addVertex(p1 - rt - up);
    lmesh.addVertex(p1 + rt - up);
    lmesh.addVertex(p1 + rt + up);
    lmesh.addVertex(p1 - rt + up);
    lmesh.addVertex(p2 - rt - up);
    lmesh.addVertex(p2 + rt - up);
    lmesh.addVertex(p2 + rt + up);
    lmesh.addVertex(p2 - rt + up);

    if(!flags[CSG_FLAG_NO_FRONT])
    {
        lmesh.addFace(index_base + 0, index_base + 1, index_base + 2, index_base + 3, flat);
    }
    if(!flags[CSG_FLAG_NO_BACK])
    {
        lmesh.addFace(index_base + 5, index_base + 4, index_base + 7, index_base + 6, flat);
    }
    if(!flags[CSG_FLAG_NO_RIGHT])
    {
        lmesh.addFace(index_base + 1, index_base + 5, index_base + 6, index_base + 2, flat);
    }
    if(!flags[CSG_FLAG_NO_LEFT])
    {
        lmesh.addFace(index_base + 4, index_base + 0, index_base + 3, index_base + 7, flat);
    }
    if(!flags[CSG_FLAG_NO_BOTTOM])
    {
        lmesh.addFace(index_base + 4, index_base + 5, index_base + 1, index_base + 0, flat);
    }
    if(!flags[CSG_FLAG_NO_TOP])
    {
        lmesh.addFace(index_base + 3, index_base + 2, index_base + 6, index_base + 7, flat);
    }
}

/// Create a cylinder shape.
///
/// \param lmesh Target logical mesh.
/// \param p1 Starting point.
/// \param p2 End point.
/// \param fidelity Fidelity of the cylinder, should be at least 3.
/// \param radius Radius of the cylinder.
/// \param flags CSG flags.
/// \param up Up direction.
void csg_cylinder(LogicalMesh& lmesh, const vec3& p1, const vec3& p2, unsigned fidelity, float radius,
        CsgFlags flags = CsgFlags(0), const vec3& param_up = vec3(0.0f, 1.0f, 0.0f))
{
    vec3 forward = p2 - p1;
    vec3 unit_up = perpendiculate(param_up, forward);
    vec3 rt = normalize(cross(forward, unit_up)) * radius;
    vec3 up = normalize(cross(rt, forward)) * radius;

    unsigned index_base = lmesh.getLogicalVertexCount();
    bool flat = flags[CSG_FLAG_FLAT];

    // Bottom and top.
    lmesh.addVertex(p1);
    lmesh.addVertex(p2);

    for(unsigned ii = 0; (ii < fidelity); ++ii)
    {
        float rad = static_cast<float>(ii) / static_cast<float>(fidelity) * static_cast<float>(M_PI * 2.0);
        vec3 dir = cos(rad) * rt + sin(rad) * up;
        lmesh.addVertex(p1 + dir);
        lmesh.addVertex(p2 + dir);

        unsigned c1 = index_base + 2 + (ii * 2);
        unsigned n1 = c1 + 1;
        unsigned c2 = n1 + 1;
        unsigned n2 = c2 + 1;
        if((ii + 1) >= fidelity)
        {
            c2 = index_base + 2;
            n2 = index_base + 3;
        }

        if(!flags[CSG_FLAG_NO_BOTTOM])
        {
            lmesh.addFace(index_base + 0, c1, c2, flat);
        }
        if(!flags[CSG_FLAG_NO_TOP])
        {
            lmesh.addFace(index_base + 1, n2, n1, flat);
        }
        lmesh.addFace(c1, n1, n2, c2, flat);
    }
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
void csg_pipe(LogicalMesh& lmesh, const vec3* points, unsigned count, unsigned fidelity, float radius,
        CsgFlags flags = CsgFlags(0))
{
    unsigned index_base = lmesh.getLogicalVertexCount();
    bool flat = flags[CSG_FLAG_FLAT];

    // Beginning and end.
    lmesh.addVertex(points[0]);
    lmesh.addVertex(points[count - 1]);

    for(unsigned ii = 1; (ii < (count - 1)); ++ii)
    {
        const vec3& p1 = points[ii - 1];
        const vec3& p2 = points[ii];
        const vec3& p3 = points[ii + 1];
        vec3 diff1 = normalize(p2 - p1);
        vec3 diff2 = normalize(p3 - p2);
        vec3 unit_up = normalize(cross(diff1, diff2));
        vec3 unit_rt1 = normalize(cross(diff1, unit_up));
        vec3 unit_rt2 = normalize(cross(diff2, unit_up));
        vec3 up = unit_up * radius;

        // If at the beginning, make the starting vertices.
        if(ii == 1)
        {
            vec3 rt = unit_rt1 * radius;

            for(unsigned jj = 0; (jj < fidelity); ++jj)
            {
                float rad = static_cast<float>(jj) / static_cast<float>(fidelity) * static_cast<float>(M_PI * 2.0);
                vec3 dir = cos(rad) * rt + sin(rad) * up;
                lmesh.addVertex(p1 + dir);

                unsigned c1 = index_base + 2 + jj;
                unsigned c2 = c1 + 1;
                if((jj + 1) >= fidelity)
                {
                    c2 = index_base + 2;
                }

                if(!flags[CSG_FLAG_NO_FRONT])
                {
                    lmesh.addFace(index_base + 0, c1, c2, flat);
                }
            }
        }

        // Make mid-vertices and connect earlier segment.
        {
            // radmul is 1 for straight pipe and sqrt(2) for straight angle.
            float radmul = sqrt(1.0f - (dot(diff1, diff2) - 1.0f));
            vec3 rt = normalize(unit_rt1 + unit_rt2) * radius * radmul;

            for(unsigned jj = 0; (jj < fidelity); ++jj)
            {
                float rad = static_cast<float>(jj) / static_cast<float>(fidelity) * static_cast<float>(M_PI * 2.0);
                vec3 dir = cos(rad) * rt + sin(rad) * up;
                lmesh.addVertex(p2 + dir);

                unsigned n1 = index_base + 2 + (ii * fidelity) + jj;
                unsigned e1 = n1 - fidelity;
                unsigned n2 = n1 + 1;
                unsigned e2 = e1 + 1;
                if((jj + 1) >= fidelity)
                {
                    n2 = index_base + 2 + (ii * fidelity);
                    e2 = n2 - fidelity;
                }

                lmesh.addFace(e1, n1, n2, e2, flat);
            }
        }

        // If at the end, make the ending vertices and connect to previous segment.
        if(ii == (count - 2))
        {
            vec3 rt = unit_rt2 * radius;

            for(unsigned jj = 0; (jj < fidelity); ++jj)
            {
                float rad = static_cast<float>(jj) / static_cast<float>(fidelity) * static_cast<float>(M_PI * 2.0);
                vec3 dir = cos(rad) * rt + sin(rad) * up;
                lmesh.addVertex(p3 + dir);

                unsigned n1 = index_base + 2 + ((count - 1) * fidelity) + jj;
                unsigned e1 = n1 - fidelity;
                unsigned n2 = n1 + 1;
                unsigned e2 = e1 + 1;
                if((jj + 1) >= fidelity)
                {
                    n2 = index_base + 2 + ((count - 1) * fidelity);
                    e2 = n2 - fidelity;
                }

                if(!flags[CSG_FLAG_NO_BACK])
                {
                    lmesh.addFace(index_base + 1, n2, n1, flat);
                }
                lmesh.addFace(e1, n1, n2, e2, flat);
            }
        }
    }
}

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
enum class CsgCommand
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
};

/// conversion operator.
constexpr int16_t to_int16(CsgCommand op)
{
    return static_cast<int16_t>(op);
}

namespace detail
{

/// CSG data reader class.
class CsgReader
{

private:
    /// Increasing data pointer.
    const int16_t* m_ptr;

public:
    /// Constructor.
    ///
    /// \param op Data pointer.
    constexpr CsgReader(const int16_t* op) noexcept :
        m_ptr(op)
    {
    }

public:
    /// Read a CSG command and advance.
    ///
    /// \return Command.
    CsgCommand readCommand()
    {
        CsgCommand ret = static_cast<CsgCommand>(*m_ptr);
        ++m_ptr;
        return ret;
    }

    /// Read a signed int.
    ///
    /// \return Signed int.
    int readInt()
    {
        int ret = static_cast<int>(*m_ptr);
        ++m_ptr;
        return ret;
    }

    /// Read an unsigned int.
    ///
    /// \return Unsigned int.
    unsigned readUnsigned()
    {
        int ret = readInt();
        VGL_ASSERT(ret >= 0);
        return static_cast<unsigned>(ret);
    }

    /// Read CSG flags.
    ///
    /// \return CSG flags.
    CsgFlags readFlags()
    {
        return CsgFlags(readUnsigned());
    }

    /// Read single floating point coordinate.
    ///
    /// Coordinate is implied multiplied by 100.
    ///
    /// \return Float coordinate.
    float readFloat()
    {
        float ret = static_cast<float>(*m_ptr) * 0.01f;
        ++m_ptr;
        return ret;
    }

    /// Read a vec2 and advance.
    ///
    /// 2-component vector is considered to be a texture coordinate and multiplies by 100 instead.
    ///
    /// \return vec2 texture coordinates.
    vec2 readVec2()
    {
        float px = readFloat();
        float py = readFloat();
        return vec2(px, py);
    }

    /// Read a vec3 and advance.
    ///
    /// \return vec3 coordinates.
    vec3 readVec3()
    {
        float px = readFloat();
        float py = readFloat();
        float pz = readFloat();
        return vec3(px, py, pz);
    }

    /// Read a direction vector and advance.
    ///
    /// \return vec3 coordinates.
    vec3 readDirVec()
    {
        int dir = readInt();
        switch(dir)
        {
        case -1:
            return vec3(-1.0f, 0.0f, 0.0f);

        case 1:
            return vec3(1.0f, 0.0f, 0.0f);

        case -2:
            return vec3(0.0f, -1.0f, 0.0f);

        case 2:
            return vec3(0.0f, 1.0f, 0.0f);

        case -3:
            return vec3(0.0f, 0.0f, -1.0f);

        case 3:
            return vec3(0.0f, 0.0f, 1.0f);

        default:
            VGL_ASSERT(dir == 0);
            break;
        }

        // Read verbatim.
        return readVec3();
    }

public:
    /// Indicates whether the reader has reached the end of stream.
    ///
    /// \return True if still ok, false if at end of stream.
    operator bool() const
    {
        return (*m_ptr != 0);
    }
};

/// Read packed data.
///
/// \param msh Mesh to store forms to.
/// \param data Data to read.
void csg_read_data(LogicalMesh& msh, const int16_t* data)
{
    CsgReader reader(data);

    // Iterate over data.
    // Zero as command halts.
    while(reader)
    {
        CsgCommand command = reader.readCommand();
        switch(command)
        {
        case CsgCommand::VERTEX:
#if !defined(USE_LD)
        default:
#endif
            {
                vec3 pos = reader.readVec3();
                msh.addVertex(pos);
            }
            break;

        case CsgCommand::TRIANGLE:
            {
                unsigned c1 = reader.readUnsigned();
                unsigned c2 = reader.readUnsigned();
                unsigned c3 = reader.readUnsigned();
                msh.addFace(c1, c2, c3);
            }
            break;

        case CsgCommand::TRIANGLE_TC:
            {
                unsigned c1 = reader.readUnsigned();
                vec2 tc1 = reader.readVec2();
                unsigned c2 = reader.readUnsigned();
                vec2 tc2 = reader.readVec2();
                unsigned c3 = reader.readUnsigned();
                vec2 tc3 = reader.readVec2();
                msh.addFace(c1, tc1, c2, tc2, c3, tc3);
            }
            break;

        case CsgCommand::QUAD:
            {
                unsigned c1 = reader.readUnsigned();
                unsigned c2 = reader.readUnsigned();
                unsigned c3 = reader.readUnsigned();
                unsigned c4 = reader.readUnsigned();
                msh.addFace(c1, c2, c3, c4);
            }
            break;

        case CsgCommand::QUAD_TC:
            {
                unsigned c1 = reader.readUnsigned();
                vec2 tc1 = reader.readVec2();
                unsigned c2 = reader.readUnsigned();
                vec2 tc2 = reader.readVec2();
                unsigned c3 = reader.readUnsigned();
                vec2 tc3 = reader.readVec2();
                unsigned c4 = reader.readUnsigned();
                vec2 tc4 = reader.readVec2();
                msh.addFace(c1, tc1, c2, tc2, c3, tc3, c4, tc4);
                break;
            }

        case CsgCommand::BOX:
            {
                vec3 p1 = reader.readVec3();
                vec3 p2 = reader.readVec3();
                vec3 up = reader.readDirVec();
                float width = reader.readFloat();
                float height = reader.readFloat();
                CsgFlags flags = reader.readFlags();
                csg_box(msh, p1, p2, width, height, flags, up);
            }
            break;

        case CsgCommand::CYLINDER:
            {
                vec3 p1 = reader.readVec3();
                vec3 p2 = reader.readVec3();
                vec3 up = reader.readDirVec();
                unsigned fidelity = reader.readUnsigned();
                float radius = reader.readFloat();
                CsgFlags flags = reader.readFlags();
                csg_cylinder(msh, p1, p2, fidelity, radius, flags, up);
            }
            break;

        case CsgCommand::PIPE:
            {
                vector<vec3> points;
                for(unsigned ii = 0, ee = reader.readUnsigned(); (ii < ee); ++ii)
                {
                    points.push_back(reader.readVec3());
                }
                unsigned fidelity = reader.readUnsigned();
                float radius = reader.readFloat();
                CsgFlags flags = reader.readFlags();
                csg_pipe(msh, points.data(), points.size(), fidelity, radius, flags);
            }
            break;

#if defined(USE_LD)
        default:
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("invalid CSG command: " + std::to_string(static_cast<int>(command))));
            }
#endif
        }
    }
}

/// Add raw model data.
///
/// Bone input is arranged as 3 weights and 3 references.
/// The 3 weights should add up to 255.
///
/// \param vertices Vertex input.
/// \param bones Bone input.
/// \param faces Face input.
/// \param vertices_amount Vertex data element count.
/// \param bones_amount Bone data element count.
/// \param faces_amount Face data element count.
/// \param scale Scale to multiply with.
void csg_read_raw(LogicalMesh& msh, const int16_t *vertices, const uint8_t *bones, const uint16_t* faces,
        unsigned vertices_amount, unsigned bones_amount, unsigned faces_amount, float scale)
    {
#if defined(USE_LD)
        if(bones && ((vertices_amount * 2) != bones_amount))
        {
            std::ostringstream sstr;
            sstr << "vertex amount (" << vertices_amount << ") and bones amount (" << bones_amount <<
                ") do not match";
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#else
        (void)bones_amount;
#endif
        // Store logical vertex count before adding raw data.
        unsigned index_base = msh.getLogicalVertexCount();

        for(unsigned ii = 0, jj = 0; (ii < vertices_amount); ii += 3, jj += 6)
        {
            vec3 ver(static_cast<float>(vertices[ii + 0]) * scale,
                    static_cast<float>(vertices[ii + 1]) * scale,
                    static_cast<float>(vertices[ii + 2]) * scale);

            if(bones)
            {
                uvec4 wt(bones[jj + 0],
                        bones[jj + 1],
                        bones[jj + 2],
                        0);
                uvec4 rf(bones[jj + 3],
                        bones[jj + 4],
                        bones[jj + 5],
                        0);

                msh.addVertex(ver, wt, rf);
            }
            else
            {
                msh.addVertex(ver);
            }
        }

        for(unsigned ii = 0; ii < faces_amount; ii += 3)
        {
            msh.addFace(faces[ii + 0] + index_base,
                    faces[ii + 1] + index_base,
                    faces[ii + 2] + index_base);
        }
    }
    /// Add raw model data.
    ///
    /// Bone data is not added.
    ///
    /// \param vertices Vertex input.
    /// \param faces Face input.
    /// \param vertices_amount Vertex data element count.
    /// \param faces_amount Face data element count.
    /// \param scale Scale to multiply with.
    void csg_read_raw(LogicalMesh& msh, const int16_t *vertices, const uint16_t* faces,
            unsigned vertices_amount, unsigned faces_amount, float scale)
    {
        csg_read_raw(msh, vertices, nullptr, faces, vertices_amount, 0, faces_amount, scale);
    }
}

}

#endif
