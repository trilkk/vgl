#ifndef VGL_CSG_HPP
#define VGL_CSG_HPP

#include "vgl_logical_mesh.hpp"

namespace vgl
{

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
/// \param p1 Center of front face.
/// \param p2 Center of back face.
/// \param width Width.
/// \param height Height.
/// \param up Up direction.
void csg_box(LogicalMesh& lmesh, const vec3& p1, const vec3& p2, float width, float height,
        CsgFlags flags = CsgFlags(0), const vec3& param_up = vec3(0.0f, 1.0f, 0.0f))
{
    vec3 forward = p2 - p1;
    vec3 unit_up = normalize(param_up);

    // Rotate up vector components once if it's perpendicular to the forward vector.
    if(dot(unit_up, normalize(forward)) >= 0.999f)
    {
        unit_up = vec3(unit_up.z(), unit_up.x(), unit_up.y());
    }

    vec3 up = unit_up * (width * 0.5f);
    vec3 rt = normalize(cross(forward, param_up)) * (height * 0.5f);

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

/// CSG command enumeration.
///
/// In base cgl namespace so it's easier to use.
///
/// Unless specifically mentioned, the geometric values are multiplied by 10.
/// Other multipliers are signified by the multiplier in parenthesis.
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
    /// - Texcoords (2) (x100).
    /// - Corner index.
    /// - Texcoords (2) (x100).
    /// - Corner index.
    /// - Texcoords (2) (x100).
    TRIANGLE_TC,

    /// Quad face.
    /// - Corner indices (4).
    QUAD,

    /// Quad face (with texcoord).
    /// - Corner index.
    /// - Texcoords (2) (x100).
    /// - Corner index.
    /// - Texcoords (2) (x100).
    /// - Corner index.
    /// - Texcoords (2) (x100).
    /// - Corner index.
    /// - Texcoords (2) (x100).
    QUAD_TC,

    /// Box.
    /// - Start position (3).
    /// - End position (3).
    /// - Up vector (3).
    /// - Width.
    /// - Height.
    /// - CSG flags.
    BOX,
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

    /// Read an unsigned int.
    ///
    /// \return Unsigned int.
    unsigned readUnsigned()
    {
#if defined(USE_LD)
        VGL_ASSERT(*m_ptr >= 0);
#endif
        unsigned ret = static_cast<unsigned>(*m_ptr);
        ++m_ptr;
        return ret;
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
    /// Coordinate is implied multiplied by 10.
    ///
    /// \return Float coordinate.
    float readFloat()
    {
        float ret = static_cast<float>(*m_ptr) / 10.0f;
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
        float px = readFloat() / 10.0f;
        float py = readFloat() / 10.0f;
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
                break;
            }

        case CsgCommand::TRIANGLE:
            {
                unsigned c1 = reader.readUnsigned();
                unsigned c2 = reader.readUnsigned();
                unsigned c3 = reader.readUnsigned();
                msh.addFace(c1, c2, c3);
                break;
            }

        case CsgCommand::TRIANGLE_TC:
            {
                unsigned c1 = reader.readUnsigned();
                vec2 tc1 = reader.readVec2();
                unsigned c2 = reader.readUnsigned();
                vec2 tc2 = reader.readVec2();
                unsigned c3 = reader.readUnsigned();
                vec2 tc3 = reader.readVec2();
                msh.addFace(c1, tc1, c2, tc2, c3, tc3);
                break;
            }

        case CsgCommand::QUAD:
            {
                unsigned c1 = reader.readUnsigned();
                unsigned c2 = reader.readUnsigned();
                unsigned c3 = reader.readUnsigned();
                unsigned c4 = reader.readUnsigned();
                msh.addFace(c1, c2, c3, c4);
                break;
            }

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
                vec3 up = reader.readVec3();
                float width = reader.readFloat();
                float height = reader.readFloat();
                CsgFlags flags = reader.readFlags();
                csg_box(msh, p1, p2, width, height, flags, up);
                break;
            }

#if defined(USE_LD)
        default:
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("invalid CSG command: " + std::to_string(static_cast<int>(command))));
            }
#endif
        }
    }
}

}

}

#endif
