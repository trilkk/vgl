#ifndef VGL_BOUNDING_BOX
#define VGL_BOUNDING_BOX

#include "vgl_mat4.hpp"

namespace vgl
{

class BoundingBox
{
private:
    /// Minimum corner.
    vec3 m_min;

    /// Maximum corner.
    vec3 m_max;

    /// Center point.
    /// Also serves as initialization check.
    optional<vec3> m_center;

public:
    /// Empty constructor.
    constexpr explicit BoundingBox() noexcept = default;

    /// Constructor.
    ///
    /// \param op One point within the box.
    constexpr explicit BoundingBox(const vec3& op) noexcept :
        m_min(op),
        m_max(op),
        m_center(op)
    {
    }

    /// Constructor.
    ///
    /// \param p1 Minimum coordinate.
    /// \param p2 Maximum coordinate.
    constexpr explicit BoundingBox(const vec3& p1, const vec3& p2) noexcept :
        m_min(p1),
        m_max(p2),
        m_center((p1 + p2) * 0.5f)
    {
        VGL_ASSERT(m_min.x() <= m_max.x());
        VGL_ASSERT(m_min.y() <= m_max.y());
        VGL_ASSERT(m_min.z() <= m_max.z());
    }

public:
    /// Accessor.
    ///
    /// \return Minimum point.
    constexpr const vec3& getMin() const noexcept
    {
        return m_min;
    }

    /// Accessor.
    ///
    /// \return Maximum point.
    constexpr const vec3& getMax() const noexcept
    {
        return m_max;
    }

    /// Accessor.
    ///
    /// \return Bounding box center.
    constexpr const vec3& getCenter() const noexcept
    {
        VGL_ASSERT(isInitialized());
        return *m_center;
    }

    /// Adds a point to the bounding box area.
    ///
    /// \param op Point to add.
    constexpr void addPoint(const vec3& op) noexcept
    {
        if(!isInitialized())
        {
            m_min = op;
            m_max = op;
        }
        else
        {
            m_min[0] = min(m_min.x(), op.x());
            m_min[1] = min(m_min.y(), op.y());
            m_min[2] = min(m_min.z(), op.z());
            m_max[0] = max(m_max.x(), op.x());
            m_max[1] = max(m_max.y(), op.y());
            m_max[2] = max(m_max.z(), op.z());
        }

        // Update center point every time.
        // This is unoptimal, but does not matter.
        m_center = (m_min + m_max) * 0.5f;
    }

    /// Is the bounding box initialized?
    ///
    /// \return True if any data has been inserted, false otherwise.
    constexpr bool isInitialized() const noexcept
    {
        return static_cast<bool>(m_center);
    }

    /// Returns a new bounding box, axis-aligned, transformed with given matrix.
    ///
    /// \param trns Transformation matrix.
    constexpr BoundingBox transform(const vgl::mat4& trns) const noexcept
    {
        VGL_ASSERT(isInitialized());

        vec3 new_min = trns * m_min;
        vec3 new_max = new_min;

        array<vec3, 7> transformed_coords;
        transformed_coords[0] = trns * vec3(m_max.x(), m_min.y(), m_min.z());
        transformed_coords[1] = trns * vec3(m_min.x(), m_max.y(), m_min.z());
        transformed_coords[2] = trns * vec3(m_min.x(), m_min.y(), m_max.z());
        transformed_coords[3] = trns * vec3(m_max.x(), m_max.y(), m_min.z());
        transformed_coords[4] = trns * vec3(m_max.x(), m_min.y(), m_max.z());
        transformed_coords[5] = trns * vec3(m_min.x(), m_max.y(), m_max.z());
        transformed_coords[6] = trns * m_max;

        for(const vec3& vv : transformed_coords)
        {

            new_min[0] = min(new_min.x(), vv.x());
            new_min[1] = min(new_min.y(), vv.y());
            new_min[2] = min(new_min.z(), vv.z());
            new_max[0] = max(new_max.x(), vv.x());
            new_max[1] = max(new_max.y(), vv.y());
            new_max[2] = max(new_max.z(), vv.z());
        }

        return BoundingBox(new_min, new_max);
    }

    /// Tells if the bounding box collides with a Z range.
    ///
    /// Input values need not be in order.
    ///
    /// \param zmin Smaller Z value.
    /// \param zmax Larger Z value.
    /// \return True if collides, false otherwise.
    constexpr bool collidesZ(float zmin, float zmax) const noexcept
    {
        VGL_ASSERT(zmin <= zmax);
        return collides_range(m_min.z(), m_max.z(), zmin, zmax);
    }

protected:
    /// Tells if a range of values collides.
    ///
    /// \param min1 Minimum value 1.
    /// \param max1 Maximum value 1.
    /// \param min2 Minimum value 2.
    /// \param max2 Maximum value 2.
    static constexpr bool collides_range(float min1, float max1, float min2, float max2) noexcept
    {
        return !((max1 <= min2) || (max2 <= min1));
    }

public:
#if defined(USE_LD)
    /// Stream output operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const BoundingBox& rhs)
    {
        return lhs << "BoundingBox(" << rhs.getMin() << ", " << rhs.getMax() << ")";
    }
#endif
};

}

#endif
