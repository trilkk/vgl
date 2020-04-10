#ifndef VGL_LOGICAL_VERTEX_HPP
#define VGL_LOGICAL_VERTEX_HPP

#include "vgl_logical_face.hpp"
#include "vgl_vector.hpp"

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
        constexpr uvec4 getWeights() const
        {
            return m_weights;
        }

        /// Accessor.
        ///
        /// \return References.
        constexpr uvec4 getReferences() const
        {
            return m_references;
        }

    public:
        /// Default constructor.
        explicit BoneRef() = default;

        /// Constructor.
        ///
        /// \param weights Weights.
        /// \param references References.
        constexpr explicit BoneRef(uvec4 weights, uvec4 references) :
            m_weights(weights),
            m_references(references)
        {
        }

    public:
        /// Equals operator.
        ///
        /// \param rhs Right-hand-side operand.
        constexpr bool operator==(const BoneRef& rhs) const
        {
            return (m_weights == rhs.m_weights) &&
                (m_references == rhs.m_references);
        }
        /// Not equals operator.
        ///
        /// \param rhs Right-hand-side operand.
        constexpr bool operator!=(const BoneRef& rhs) const
        {
            return !(*this == rhs);
        }
    };

private:
    /// Position data.
    vec3 m_position;

    /// Normal data.
    optional<vec3> m_normal;

    /// Color data.
    optional<uvec4> m_color;

    /// Bone data.
    optional<BoneRef> m_bone_ref;

    /// Texture coordinate data.
    /// Assumed to be from faces.
    optional<vec2> m_texcoord;

    /// Face listing.
    vector<LogicalFace*> m_face_references;

public:
    /// Empty constructor.
    explicit LogicalVertex() = default;

    /// Constructor.
    ///
    /// \param pos Position.
    constexpr explicit LogicalVertex(const vec3 &pos) :
        m_position(pos)
    {
    }

    /// Constructor.
    ///
    /// \param pos Position.
    /// \param col Color.
    constexpr explicit LogicalVertex(const vec3& pos, const uvec4& col) :
        m_position(pos),
        m_color(col)
    {
    }

    /// Constructor.
    ///
    /// \param pos Position.
    /// \param weights Bone weights.
    /// \param references Bone references.
    constexpr explicit LogicalVertex(const vec3& pos, const uvec4& weights, const uvec4& references) :
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
    constexpr explicit LogicalVertex(const vec3& pos, const uvec4& col, const uvec4& weights, const uvec4& references) :
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
    constexpr explicit LogicalVertex(const vec3& pos, const optional<uvec4>& col, const optional<BoneRef>& bref) :
        m_position(pos),
        m_color(col),
        m_bone_ref(bref)
    {
    }

    /// Move constructor.
    ///
    /// \param op Source vertex.
    constexpr LogicalVertex(LogicalVertex&& op) :
        m_position(move(op.m_position)),
        m_normal(move(op.m_normal)),
        m_color(move(op.m_color)),
        m_bone_ref(move(op.m_bone_ref)),
        m_texcoord(move(op.m_texcoord)),
        m_face_references(move(op.m_face_references))
    {
    }

public:
    /// Add reference to face.
    ///
    /// \param op Face.
    void addFaceReference(LogicalFace *op)
    {
        m_face_references.push_back(op);
    }
    /// Add reference to face wrapper.
    ///
    /// \param op Face.
    void addFaceReference(LogicalFace &op)
    {
        addFaceReference(&op);
    }

    /// Clear face references.
    void clearFaceReferences()
    {
        m_face_references.clear();
    }

    /// Calculate normal averaging from face reference normals.
    void calculateNormal()
    {
        // If already set, this is a part of a flat face and should not average a normal.
        if(m_normal)
        {
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
    constexpr optional<uvec4> getColor() const
    {
        return m_color;
    }
    /// Setter.
    ///
    /// \param op Color.
    constexpr void setColor(const uvec4& op)
    {
        m_color = op;
    }

    /// Accessor.
    ///
    /// \return Reference to the face references.
    const vector<LogicalFace*>& getFaceReferences() const
    {
        return m_face_references;
    }

    /// Accessor.
    ///
    /// \return Normal or nullopt.
    constexpr optional<vec3> getNormal() const
    {
        return m_normal;
    }
    /// Setter.
    ///
    /// \param op Normal.
    constexpr void setNormal(const vec3& op)
    {
        m_normal = op;
    }

    /// Accessor.
    ///
    /// \return Position.
    constexpr vec3 getPosition() const
    {
        return m_position;
    }

    /// Accessor.
    ///
    /// \return References or nullopt.
    constexpr optional<uvec4> getReferences() const
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
    constexpr optional<uvec4> getWeights() const
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
    constexpr optional<vec2> getTexcoord() const
    {
        return m_texcoord;
    }
    /// Setter.
    ///
    /// \param op Texcoords.
    constexpr void setTexcoord(const vec2& op)
    {
        m_texcoord = op;
    }

    /// Check if a vertex matches another.
    ///
    /// Face references are not compared, only data.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return True if match, false if no.
    constexpr bool matches(const LogicalVertex& rhs) const
    {
        return almost_equal(m_position, rhs.m_position) &&
            almost_equal(m_normal, rhs.m_normal) &&
            (m_color == rhs.m_color) &&
            (m_bone_ref == rhs.m_bone_ref) &&
            almost_equal(m_texcoord, rhs.m_texcoord);
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
