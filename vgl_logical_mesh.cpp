#include "vgl_logical_mesh.hpp"

namespace vgl
{

#if defined(USE_LD)
unsigned int LogicalMesh::g_vertices_erased = 0;
#endif

#if !defined(VGL_DISABLE_CSG)
namespace
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
#if defined(USE_LD)
    // Rotate direction vector components once if it's perpendicular to the reference vector.
    float dot_result = dot(unit_dir, normalize(ref));
    if(abs(dot_result) >= 0.999f)
    {
        std::cerr << "WARNING: direction vector " << dir << " is perpendicular to reference vector " << ref << std::endl;
        return vec3(unit_dir.z(), unit_dir.x(), unit_dir.y());
    }
#else
    (void)ref;
#endif
    return unit_dir;
}

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
        VGL_ASSERT(to_int16(ret) < to_int16(CsgCommand::COUNT));
        return ret;
    }

    /// Read a signed int.
    ///
    /// \return Signed int.
    constexpr int readInt()
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
    constexpr float readFloat()
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
    constexpr vec2 readVec2()
    {
        float px = readFloat();
        float py = readFloat();
        return vec2(px, py);
    }

    /// Read a vec3 and advance.
    ///
    /// \return vec3 coordinates.
    constexpr vec3 readVec3()
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
    constexpr operator bool() const
    {
        return (*m_ptr != 0);
    }
};

}

void LogicalMesh::csgTrapezoid(const vec3* points, const vec2* sizes, unsigned count, const vec3& param_dir,
        const vec3& param_up, CsgFlags flags)
{
    vec3 unit_fw = normalize(param_dir);
    vec3 unit_up = perpendiculate(param_up, unit_fw);
    vec3 unit_rt = normalize(cross(unit_fw, unit_up));

    unsigned index_base = getLogicalVertexCount();
    bool flat = flags[CSG_FLAG_FLAT];

    for(unsigned ii = 0; (ii < count); ++ii)
    {
        const vec3& pos = points[ii];
        const vec2& sz = sizes[ii];

        vec3 rt = unit_rt * (sz.x() * 0.5f);
        vec3 up = unit_up * (sz.y() * 0.5f);

        addVertex(pos - rt - up);
        addVertex(pos + rt - up);
        addVertex(pos + rt + up);
        addVertex(pos - rt + up);

        // Start laying out faces after the first segment.
        if(ii < (count - 1))
        {
            unsigned curr = index_base + (ii * 4);

            // Lay out sides.
            if(!flags[CSG_FLAG_NO_RIGHT])
            {
                addFace(curr + 1, curr + 5, curr + 6, curr + 2, flat);
            }
            if(!flags[CSG_FLAG_NO_LEFT])
            {
                addFace(curr + 4, curr + 0, curr + 3, curr + 7, flat);
            }
            if(!flags[CSG_FLAG_NO_BOTTOM])
            {
                addFace(curr + 4, curr + 5, curr + 1, curr + 0, flat);
            }
            if(!flags[CSG_FLAG_NO_TOP])
            {
                addFace(curr + 3, curr + 2, curr + 6, curr + 7, flat);
            }
        }
    }

    // Front face is the first.
    if(!flags[CSG_FLAG_NO_FRONT])
    {
        addFace(index_base + 0, index_base + 1, index_base + 2, index_base + 3, flat);
    }
    // Back face is the last.
    if(!flags[CSG_FLAG_NO_BACK])
    {
        unsigned last = index_base + (count * 4);
        addFace(last - 1, last - 2, last - 3, last - 4, flat);
    }
}

void LogicalMesh::csgCone(const vec3& p1, const vec3& p2, const vec3 param_fw, const vec3& param_up, unsigned fidelity,
        float radius1, float radius2, CsgFlags flags)
{
    vec3 unit_fw = normalize(param_fw);
    vec3 unit_up = perpendiculate(param_up, unit_fw);
    vec3 unit_rt = normalize(cross(unit_fw, unit_up));

    const float rad_offset = static_cast<float>(M_PI) * 2.0f / static_cast<float>(fidelity) * 0.5f;
    unsigned index_base = getLogicalVertexCount();
    bool flat = flags[CSG_FLAG_FLAT];

    // Invalid values for cylinders.
    VGL_ASSERT(!flags[CSG_FLAG_NO_BOTTOM]);
    VGL_ASSERT(!flags[CSG_FLAG_NO_TOP]);
    VGL_ASSERT(!flags[CSG_FLAG_NO_LEFT]);
    VGL_ASSERT(!flags[CSG_FLAG_NO_RIGHT]);

    // Bottom and top.
    addVertex(p1);
    addVertex(p2);

    for(unsigned ii = 0; (ii < fidelity); ++ii)
    {
        float rad = static_cast<float>(ii) / static_cast<float>(fidelity) * static_cast<float>(M_PI * 2.0) + rad_offset;
        vec3 dir1 = (cos(rad) * unit_rt + sin(rad) * unit_up) * radius1;
        vec3 dir2 = (cos(rad) * unit_rt + sin(rad) * unit_up) * radius2;
        addVertex(p1 + dir1);
        addVertex(p2 + dir2);

        unsigned c1 = index_base + 2 + (ii * 2);
        unsigned n1 = c1 + 1;
        unsigned c2 = n1 + 1;
        unsigned n2 = c2 + 1;
        if((ii + 1) >= fidelity)
        {
            c2 = index_base + 2;
            n2 = index_base + 3;
        }

        if(!flags[CSG_FLAG_NO_FRONT])
        {
            addFace(index_base + 0, c1, c2, flat);
        }
        if(!flags[CSG_FLAG_NO_BACK])
        {
            addFace(index_base + 1, n2, n1, flat);
        }
        addFace(c1, n1, n2, c2, flat);
    }
}

void LogicalMesh::csgPipe(const vec3* points, unsigned count, unsigned fidelity, float radius,
        CsgFlags flags)
{
    const float rad_offset = static_cast<float>(M_PI) * 2.0f / static_cast<float>(fidelity) * 0.5f;
    unsigned index_base = getLogicalVertexCount();
    bool flat = flags[CSG_FLAG_FLAT];
    vgl::vec3 prev_unit_up;

    // Invalid values for pipes.
    VGL_ASSERT(!flags[CSG_FLAG_NO_BOTTOM]);
    VGL_ASSERT(!flags[CSG_FLAG_NO_TOP]);
    VGL_ASSERT(!flags[CSG_FLAG_NO_LEFT]);
    VGL_ASSERT(!flags[CSG_FLAG_NO_RIGHT]);

    // Beginning and end.
    addVertex(points[0]);
    addVertex(points[count - 1]);

    for(unsigned ii = 1; (ii < (count - 1)); ++ii)
    {
        const vec3& p1 = points[ii - 1];
        const vec3& p2 = points[ii];
        const vec3& p3 = points[ii + 1];
        vec3 diff1 = normalize(p2 - p1);
        vec3 diff2 = normalize(p3 - p2);
        vec3 unit_up = normalize(cross(diff1, diff2));

        // Prevent absurd turns in the pipe.
        if((ii > 1) && (dot(prev_unit_up, unit_up) < 0.0f))
        {
            unit_up = -unit_up;
        }
        prev_unit_up = unit_up;

        vec3 unit_rt1 = normalize(cross(diff1, unit_up));
        vec3 unit_rt2 = normalize(cross(diff2, unit_up));
        vec3 up = unit_up * radius;

        // If at the beginning, make the starting vertices.
        if(ii == 1)
        {
            vec3 rt = unit_rt1 * radius;

            for(unsigned jj = 0; (jj < fidelity); ++jj)
            {
                float rad = static_cast<float>(jj) / static_cast<float>(fidelity) * static_cast<float>(M_PI * 2.0) + rad_offset;
                vec3 dir = cos(rad) * rt + sin(rad) * up;
                addVertex(p1 + dir);

                unsigned c1 = index_base + 2 + jj;
                unsigned c2 = c1 + 1;
                if((jj + 1) >= fidelity)
                {
                    c2 = index_base + 2;
                }

                if(!flags[CSG_FLAG_NO_FRONT])
                {
                    addFace(index_base + 0, c1, c2, flat);
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
                float rad = static_cast<float>(jj) / static_cast<float>(fidelity) * static_cast<float>(M_PI * 2.0) + rad_offset;
                vec3 dir = cos(rad) * rt + sin(rad) * up;
                addVertex(p2 + dir);

                unsigned n1 = index_base + 2 + (ii * fidelity) + jj;
                unsigned e1 = n1 - fidelity;
                unsigned n2 = n1 + 1;
                unsigned e2 = e1 + 1;
                if((jj + 1) >= fidelity)
                {
                    n2 = index_base + 2 + (ii * fidelity);
                    e2 = n2 - fidelity;
                }

                addFace(e1, n1, n2, e2, flat);
            }
        }

        // If at the end, make the ending vertices and connect to previous segment.
        if(ii == (count - 2))
        {
            vec3 rt = unit_rt2 * radius;

            for(unsigned jj = 0; (jj < fidelity); ++jj)
            {
                float rad = static_cast<float>(jj) / static_cast<float>(fidelity) * static_cast<float>(M_PI * 2.0) + rad_offset;
                vec3 dir = cos(rad) * rt + sin(rad) * up;
                addVertex(p3 + dir);

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
                    addFace(index_base + 1, n2, n1, flat);
                }
                addFace(e1, n1, n2, e2, flat);
            }
        }
    }
}

void LogicalMesh::csgReadData(const int16_t* data)
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
                addVertex(pos);
            }
            break;

        case CsgCommand::TRIANGLE:
            {
                unsigned c1 = reader.readUnsigned();
                unsigned c2 = reader.readUnsigned();
                unsigned c3 = reader.readUnsigned();
                addFace(c1, c2, c3);
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
                addFace(c1, tc1, c2, tc2, c3, tc3);
            }
            break;

        case CsgCommand::QUAD:
            {
                unsigned c1 = reader.readUnsigned();
                unsigned c2 = reader.readUnsigned();
                unsigned c3 = reader.readUnsigned();
                unsigned c4 = reader.readUnsigned();
                addFace(c1, c2, c3, c4);
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
                addFace(c1, tc1, c2, tc2, c3, tc3, c4, tc4);
            }
            break;

        case CsgCommand::BOX:
            {
                vec3 p1 = reader.readVec3();
                vec3 p2 = reader.readVec3();
                vec3 up = reader.readDirVec();
                float width = reader.readFloat();
                float height = reader.readFloat();
                CsgFlags flags = reader.readFlags();
                csgBox(p1, p2, up, width, height, flags);
            }
            break;

        case CsgCommand::TRAPEZOID:
            {
                unsigned count = reader.readUnsigned();
                vector<vec3> points;
                vector<vec2> sizes;
                for(unsigned ii = 0; (ii < count); ++ii)
                {
                    points.push_back(reader.readVec3());
                    sizes.push_back(reader.readVec2());
                }
                vec3 dir = reader.readDirVec();
                vec3 up = reader.readDirVec();
                CsgFlags flags = reader.readFlags();
                csgTrapezoid(points.data(), sizes.data(), count, dir, up, flags);
            }
            break;

        case CsgCommand::CONE:
            {
                vec3 p1 = reader.readVec3();
                vec3 p2 = reader.readVec3();
                vec3 fw = reader.readDirVec();
                vec3 up = reader.readDirVec();
                unsigned fidelity = reader.readUnsigned();
                float radius1 = reader.readFloat();
                float radius2 = reader.readFloat();
                CsgFlags flags = reader.readFlags();
                csgCone(p1, p2, fw, up, fidelity, radius1, radius2, flags);
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
                csgCylinder(p1, p2, up, fidelity, radius, flags);
            }
            break;

        case CsgCommand::PIPE:
            {
                unsigned count = reader.readUnsigned();
                vector<vec3> points;
                for(unsigned ii = 0; (ii < count); ++ii)
                {
                    points.push_back(reader.readVec3());
                }
                unsigned fidelity = reader.readUnsigned();
                float radius = reader.readFloat();
                CsgFlags flags = reader.readFlags();
                csgPipe(points.data(), count, fidelity, radius, flags);
            }
            break;

#if defined(USE_LD)
        default:
            VGL_THROW_RUNTIME_ERROR("invalid CSG command: " + to_string(static_cast<int>(command)));
#endif
        }
    }
}

void LogicalMesh::csgReadRaw(const int16_t *vertices, const uint8_t *bones, const uint16_t* faces,
        unsigned vertices_amount, unsigned faces_amount, float scale)
{
    // Store logical vertex count before adding raw data.
    unsigned index_base = getLogicalVertexCount();

    {
        const int16_t *viter = vertices;
        const uint8_t *biter = bones;

        for(unsigned ii = 0; (ii < vertices_amount); ++ii)
        {
            vec3 ver(static_cast<float>(viter[0]) * scale,
                    static_cast<float>(viter[1]) * scale,
                    static_cast<float>(viter[2]) * scale);

            if(bones)
            {
                uvec4 wt(biter[0],
                        biter[1],
                        biter[2],
                        0);
                uvec4 rf(biter[3],
                        biter[4],
                        biter[5],
                        0);

                addVertex(ver, wt, rf);
            }
            else
            {
                addVertex(ver);
            }

            viter += 3;
            biter += 6;
        }
    }

    {
        const uint16_t *fiter = faces;

        for(unsigned ii = 0; (ii < faces_amount); ++ii)
        {
            addFace(fiter[0] + index_base,
                    fiter[1] + index_base,
                    fiter[2] + index_base);

            fiter += 3;
        }
    }
}
#endif

}

