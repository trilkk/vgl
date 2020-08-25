#ifndef VGL_ARMATURE_HPP
#define VGL_ARMATURE_HPP

#include "vgl_bone.hpp"

namespace vgl
{

/// Armature.
class Armature
{
private:
    /// Child bones.
    vector<Bone> m_bones;

public:
    /// Empty constructor.
    explicit Armature() noexcept = default;

    /// Constructor into bone data.
    ///
    /// \param bdata Bone data.
    /// \param bones_amount Element count of bone data.
    /// \param hdata Hierarchy data.
    /// \param hierarchy_amount Element count of hierarchy data.
    /// \param scale Scale to multiply with.
    explicit Armature(const int16_t *bdata, const uint8_t *hdata, unsigned bones_amount, unsigned hierarchy_amount,
            float scale)
    {
        readRaw(bdata, hdata, bones_amount, hierarchy_amount, scale);
    }

private:
    /// Initialize armature data from data blobs.
    ///
    /// \param bdata Bone data.
    /// \param bones_amount Element count of bone data.
    /// \param hdata Hierarchy data.
    /// \param hierarchy_amount Element count of hierarchy data.
    /// \param scale Scale to multiply with.
    void readRaw(const int16_t *bdata, const uint8_t *hdata, unsigned bones_amount, unsigned hierarchy_amount, float scale)
    {
#if defined(USE_LD)
        if(!m_bones.empty())
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("trying to init non-empty armature from data"));
        }
#endif
        for(unsigned ii = 0, idx = 0; (ii < bones_amount); ii += 3, ++idx)
        {
            vec3 pos(static_cast<float>(bdata[ii + 0]),
                    static_cast<float>(bdata[ii + 1]),
                    static_cast<float>(bdata[ii + 2]));

            addBone(idx, pos * scale);
        }

        {
            const uint8_t *iter = hdata;

            for(Bone &vv : m_bones)
            {
                for(uint8_t child_count = *iter++; (0 < child_count); --child_count)
                {
                    Bone& child = getBone(*iter++);

                    vv.addChild(&child);
                }
            }

#if defined(USE_LD)
            if(hdata + hierarchy_amount != iter)
            {
                std::ostringstream sstr;
                sstr << "reference data inconsistency: " << static_cast<unsigned>(iter - hdata) << " vs. " <<
                    hierarchy_amount;
                BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
            }
#else
            (void)hierarchy_amount;
#endif
        }
    }

public:
    /// Add a new bone.
    ///
    /// \param idx Index.
    /// \param pos Position.
    void addBone(unsigned idx, const vec3 &pos)
    {
        m_bones.emplace_back(idx, pos);
    }

    /// Accessor.
    ///
    /// \param idx Bone index.
    /// \return Bone reference.
    Bone& getBone(unsigned idx)
    {
        return m_bones[idx];
    }
    /// Const accessor
    ///
    /// \param idx Bone index.
    /// \return Bone reference.
    const Bone& getBone(unsigned idx) const
    {
        return m_bones[idx];
    }

    /// Accessor.
    ///
    /// \return Bone count.
    unsigned getBoneCount() const noexcept
    {
        return m_bones.size();
    }

    /// Hierarchically transform all bones.
    ///
    /// \param matrices Matrix data.
    void hierarchicalTransform(mat3 *matrices) const
    {
        for(auto& vv : m_bones)
        {
            if(vv.getParent())
            {
                continue;
            }

            vv.recursiveTransform(matrices);
        }
    }

public:
    /// Creates a new armature.
    ///
    /// \param bdata Bone data.
    /// \param bones_amount Element count of bone data.
    /// \param hdata Hierarchy data.
    /// \param hierarchy_amount Element count of hierarchy data.
    /// \param scale Scale to multiply with.
    static unique_ptr<Armature> create(const int16_t *bdata, const uint8_t *hdata, unsigned bones_amount,
            unsigned hierarchy_amount, float scale)
    {
        return unique_ptr<Armature>(new Armature(bdata, hdata, bones_amount, hierarchy_amount, scale));
    }

public:
#if defined(USE_LD)
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream &lhs, const Armature& rhs)
    {
        return lhs << "Armature: " << rhs.m_bones.size() << " bones";
    }
#endif
};

/// Armature unique pointer type.
using ArmatureUptr = unique_ptr<Armature>;

}

#endif
