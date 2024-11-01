#ifndef VGL_LOGICAL_VERTEX_HPP
#define VGL_LOGICAL_VERTEX_HPP

#include "vgl_logical_face.hpp"
#include "vgl_mesh.hpp"
#include "vgl_vector.hpp"

#if defined(USE_LD)
#include "vgl_bitset.hpp"
#endif

namespace vgl
{

/// Logical vertex class.
///
/// Only limited number of faces can attach to a vertex.
class LogicalVertex
{
public:
    /// Reference for bone animation.
    class BoneRef
    {
    public:
        /// Weight data.
        uvec4 m_weights;

        /// Reference data.
        uvec4 m_references;

    public:
        /// Accessor.
        ///
        /// \return Weights.
        constexpr uvec4 getWeights() const noexcept
        {
            return m_weights;
        }

        /// Accessor.
        ///
        /// \return References.
        constexpr uvec4 getReferences() const noexcept
        {
            return m_references;
        }

    public:
        /// Default constructor.
        constexpr explicit BoneRef() noexcept = default;

        /// Constructor.
        ///
        /// \param weights Weights.
        /// \param references References.
        constexpr explicit BoneRef(uvec4 weights, uvec4 references) noexcept :
            m_weights(weights),
            m_references(references)
        {
        }

    public:
        /// Equals operator.
        ///
        /// \param rhs Right-hand-side operand.
        constexpr bool operator==(const BoneRef& rhs) const noexcept
        {
            return (m_weights == rhs.m_weights) &&
                (m_references == rhs.m_references);
        }
        /// Not equals operator.
        ///
        /// \param rhs Right-hand-side operand.
        constexpr bool operator!=(const BoneRef& rhs) const noexcept
        {
            return !(*this == rhs);
        }
    };

private:
    /// Position data.
    vec3 m_position;

    /// Normal data.
    optional<vec3> m_normal;

    /// Texture coordinate data.
    /// Assumed to be from faces.
    optional<vec2> m_texcoord;

    /// Color data.
    optional<uvec4> m_color;

    /// Bone data.
    optional<BoneRef> m_bone_ref;

    /// Face listing.
    vector<LogicalFace*> m_face_references;

public:
    /// Empty constructor.
    constexpr explicit LogicalVertex() noexcept = default;

    /// Constructor.
    ///
    /// \param px X position.
    /// \param py Y position.
    /// \param pz Z position.
    constexpr explicit LogicalVertex(float px, float py, float pz) noexcept :
        m_position(px, py, pz)
    {
    }

    /// Constructor.
    ///
    /// \param pos Position.
    constexpr explicit LogicalVertex(const vec3& pos) noexcept :
        m_position(pos)
    {
    }

    /// Constructor.
    ///
    /// \param pos Position.
    constexpr explicit LogicalVertex(const vec3& pos, const vec2& texcoord) noexcept :
        m_position(pos),
        m_texcoord(texcoord)
    {
    }

    /// Constructor.
    ///
    /// \param pos Position.
    /// \param col Color.
    constexpr explicit LogicalVertex(const vec3& pos, const uvec4& col) noexcept :
        m_position(pos),
        m_color(col)
    {
    }

    /// Constructor.
    ///
    /// \param pos Position.
    /// \param weights Bone weights.
    /// \param references Bone references.
    constexpr explicit LogicalVertex(const vec3& pos, const uvec4& weights, const uvec4& references) noexcept :
        m_position(pos),
        m_bone_ref(BoneRef(weights, references))
    {
    }

    /// Constructor.
    ///
    /// \param pos Position.
    /// \param col Color.
    /// \param weights Bone weights.
    /// \param references Bone references.
    constexpr explicit LogicalVertex(const vec3& pos, const uvec4& col, const uvec4& weights, const uvec4& references) noexcept :
        m_position(pos),
        m_color(col),
        m_bone_ref(BoneRef(weights, references))
    {
    }

    /// Constructor.
    ///
    /// Used for cloning.
    ///
    /// \param pos Position.
    /// \param col Optional color.
    /// \param bref Optional bone reference struct.
    constexpr explicit LogicalVertex(const vec3& pos, const optional<uvec4>& col, const optional<BoneRef>& bref) noexcept :
        m_position(pos),
        m_color(col),
        m_bone_ref(bref)
    {
    }

    /// Move constructor.
    ///
    /// \param op Source object.
    constexpr LogicalVertex(LogicalVertex&& op) noexcept:
        m_position(op.m_position),
        m_normal(op.m_normal),
        m_texcoord(op.m_texcoord),
        m_color(op.m_color),
        m_bone_ref(op.m_bone_ref),
        m_face_references(move(op.m_face_references))
    {
    }

public:
    /// Add reference to face.
    ///
    /// \param op Face.
    void addFaceReference(LogicalFace *op)
    {
#if defined(USE_LD) && defined(DEBUG)
        for(const auto& vv : m_face_references)
        {
            if(vv == op)
            {
                VGL_THROW_RUNTIME_ERROR("trying to add duplicate face references");
            }
        }
#endif
        m_face_references.push_back(op);
    }
    /// Add reference to face wrapper.
    ///
    /// \param op Face.
    void addFaceReference(LogicalFace &op)
    {
        addFaceReference(&op);
    }
    /// Remove a face reference.
    ///
    /// \param op face.
    void removeFaceReference(LogicalFace* op)
    {
#if defined(USE_LD) && defined(DEBUG)
        bool face_found = false;
#endif
        for(unsigned ii = 0; (ii < m_face_references.size());)
        {
            if(m_face_references[ii] == op)
            {
#if defined(USE_LD) && defined(DEBUG)
                if(face_found)
                {
                    VGL_THROW_RUNTIME_ERROR("face reference found multiple times");
                }
#endif
                if(ii < (m_face_references.size() - 1))
                {
                    m_face_references[ii] = m_face_references.back();
                }
                m_face_references.pop_back();
#if defined(USE_LD) && defined(DEBUG)
                continue;
#else
                return;
#endif
            }
            ++ii;
        }
    }
    /// Remove a face reference wrapper.
    ///
    /// \param op face.
    void removeFaceReference(LogicalFace& op)
    {
        removeFaceReference(&op);
    }

    /// Append all face references from another vertex into this vertex.
    ///
    /// \param op Another vertex.
    void appendFaceReferences(const LogicalVertex& op)
    {
        for(auto& vv : op.m_face_references)
        {
            addFaceReference(vv);
        }
    }

    /// Clear face references.
    void clearFaceReferences()
    {
        m_face_references.clear();
    }

    /// Calculate normal averaging from face reference normals.
    VGL_MATH_CONSTEXPR void calculateNormal()
    {
        // If already set, this is a part of a flat face and should not average a normal.
        if(m_normal)
        {
            m_normal = normalize(*m_normal);
            return;
        }

        vec3 normal(0.0f, 0.0f, 0.0f);

        for(const auto& vv : m_face_references)
        {
            normal += vv->getNormal();
        }
        m_normal = normalize(normal);
    }

    /// Clone a vertex into a new vertex.
    ///
    /// Normal and texture coordinate data will not be cloned.
    /// Face references will not be cloned. All other data will be.
    ///
    /// \return Clone of the vertex.
    LogicalVertex clone() const
    {
        return LogicalVertex(m_position, m_color, m_bone_ref);
    }

    /// Accessor.
    ///
    /// \return Color.
    constexpr optional<uvec4> getColor() const noexcept
    {
        return m_color;
    }
    /// Setter.
    ///
    /// \param op Color.
    constexpr void setColor(const uvec4& op) noexcept
    {
        m_color = op;
    }

    /// Accessor.
    ///
    /// \return Reference to the face references.
    const vector<LogicalFace*>& getFaceReferences() const noexcept
    {
        return m_face_references;
    }

    /// Accessor.
    ///
    /// \return Normal or nullopt.
    constexpr optional<vec3> getNormal() const noexcept
    {
        return m_normal;
    }
    /// Setter.
    ///
    /// \param op Normal.
    constexpr void setNormal(const vec3& op) noexcept
    {
        m_normal = op;
    }

    /// Accessor.
    ///
    /// \return Position.
    constexpr vec3 getPosition() const noexcept
    {
        return m_position;
    }

    /// Accessor.
    ///
    /// \return References or nullopt.
    constexpr optional<uvec4> getReferences() const noexcept
    {
        if(m_bone_ref)
        {
            return m_bone_ref->getReferences();
        }
        return nullopt;
    }
    /// Accessor.
    ///
    /// \return Weights or nullopt.
    constexpr optional<uvec4> getWeights() const noexcept
    {
        if(m_bone_ref)
        {
            return m_bone_ref->getWeights();
        }
        return nullopt;
    }

    /// Accessor.
    ///
    /// \return Texcoords or nullopt.
    constexpr optional<vec2> getTexcoord() const noexcept
    {
        return m_texcoord;
    }
    /// Setter.
    ///
    /// \param op Texcoords.
    constexpr void setTexcoord(const vec2& op) noexcept
    {
        m_texcoord = op;
    }

    /// Check if a vertex matches another.
    ///
    /// Face references are not compared, only data.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return True if match, false if no.
    constexpr bool matches(const LogicalVertex& rhs) const noexcept
    {
        return almost_equal(m_position, rhs.m_position) &&
            almost_equal(m_normal, rhs.m_normal) &&
            almost_equal(m_texcoord, rhs.m_texcoord) &&
            (m_color == rhs.m_color) &&
            (m_bone_ref == rhs.m_bone_ref);
    }

    /// Write this vertex into a mesh.
    ///
    /// \param op Mesh to write to.
#if defined(USE_LD)
    bitset<static_cast<unsigned>(GeometryChannel::COUNT)>
#else
        void
#endif
        write(Mesh& op) const
    {
#if defined(USE_LD)
        bitset<GeometryChannel::COUNT> ret(GeometryChannel::POSITION);
#endif
        op.write(GeometryChannel::POSITION, m_position);

        if(m_normal)
        {
#if defined(USE_LD)
            ret.set(GeometryChannel::NORMAL);
#endif
            op.write(GeometryChannel::NORMAL, ivec3(*m_normal));
        }

        if(m_texcoord)
        {
#if defined(USE_LD)
            ret.set(GeometryChannel::TEXCOORD);
#endif
            op.write(GeometryChannel::TEXCOORD, *m_texcoord);
        }

        if(m_color)
        {
#if defined(USE_LD)
            ret.set(GeometryChannel::COLOR);
#endif
            op.write(GeometryChannel::COLOR, *m_color);
        }

        if(m_bone_ref)
        {
#if defined(USE_LD)
            ret.set(GeometryChannel::BONE_WEIGHT);
            ret.set(GeometryChannel::BONE_REF);
#endif
            op.write(GeometryChannel::BONE_WEIGHT, m_bone_ref->getWeights());
            op.write(GeometryChannel::BONE_REF, m_bone_ref->getReferences());
        }

        op.endVertex();

#if defined(USE_LD)
        return ret;
#endif
    }

public:
    /// Move operator.
    ///
    /// \param op Source vertex.
    constexpr LogicalVertex& operator=(LogicalVertex&& op) noexcept
    {
        m_position = op.m_position;
        m_normal = op.m_normal;
        m_texcoord = op.m_texcoord;
        m_color = op.m_color;
        m_bone_ref = op.m_bone_ref;
        m_face_references = move(op.m_face_references);
        return *this;
    }

public:
#if defined(USE_LD)
    /// Stream output operator.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const LogicalVertex& rhs)
    {
        lhs << "Vertex(" << rhs.getPosition();
        if(rhs.m_normal)
        {
            lhs << " ; " << *rhs.m_normal;
        }
        if(rhs.m_color)
        {
            lhs << " ; " << *rhs.m_color;
        }
        if(rhs.m_bone_ref)
        {
            lhs << " ; " << (*rhs.m_bone_ref).getWeights() << " ; " << (*rhs.m_bone_ref).getReferences();
        }
        if(rhs.m_texcoord)
        {
            lhs << " ; " << *rhs.m_texcoord;
        }
        return lhs << ")";
    }
#endif
};

}

#endif
