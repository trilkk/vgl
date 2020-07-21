#ifndef VGL_BOUNDING_BOX
#define VGL_BOUNDING_BOX

#include "vgl_mat4.hpp"

namespace vgl
{

class BoundingBox
{
private:
    /// Minimum corner.
    /// Also serves as initialization check.
    optional<vec3> m_min;

    /// Maximum corner.
    vec3 m_max;

public:
    /// Empty constructor.
    constexpr BoundingBox() noexcept = default;

    /// Constructor.
    ///
    /// \param op One point within the box.
    constexpr BoundingBox(const vec3& op) :
        m_min(op),
        m_max(op)
    {
    }

    /// Constructor.
    ///
    /// \param p1 First coordinate.
    /// \param p2 Second coordinate.
    constexpr BoundingBox(const vec3& p1, const vec3& p2) :
        m_min(p1),
        m_max(p2)
    {
        addPoint(p2);
    }

public:
    /// Adds a point to the bounding box area.
    ///
    /// \param op Point to add.
    constexpr void addPoint(const vec3& op)
    {
        if(!isInitialized())
        {
            m_min = op;
            m_max = op;
        }

        (*m_min)[0] = min(m_min->x(), op[0]);
        (*m_min)[1] = min(m_min->y(), op[1]);
        (*m_min)[2] = min(m_min->z(), op[2]);
        m_max[0] = max(m_max[0], op[0]);
        m_max[1] = max(m_max[1], op[1]);
        m_max[2] = max(m_max[2], op[2]);
    }

    /// Gets the bounding box center point.
    ///
    /// \return Bounding box center.
    constexpr vec3 getCenter() const
    {
        VGL_ASSERT(isInitialized());
        return (*m_min + m_max) * 0.5f;
    }

    /// Is the bounding box initialized?
    ///
    /// \return True if any data has been inserted, false otherwise.
    constexpr bool isInitialized() const
    {
        return static_cast<bool>(m_min);
    }

    /// Returns a new bounding box, axis-aligned, transformed with given matrix.
    ///
    /// \param trns Transformation matrix.
    constexpr BoundingBox transform(const vgl::mat4& trns) const
    {
        VGL_ASSERT(isInitialized());

        array<vec3, 6> transformed_coords;

        vec3 new_min(trns * (*m_min));
        vec3 new_max(trns * m_max);

        transformed_coords[0] = trns * vec3(m_max.x(), m_min->y(), m_min->z());
        transformed_coords[1] = trns * vec3(m_min->x(), m_max.y(), m_min->z());
        transformed_coords[2] = trns * vec3(m_min->x(), m_min->y(), m_max.z());
        transformed_coords[3] = trns * vec3(m_max.x(), m_max.y(), m_min->z());
        transformed_coords[4] = trns * vec3(m_max.x(), m_min->y(), m_max.z());
        transformed_coords[5] = trns * vec3(m_min->x(), m_max.y(), m_max.z());

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
    /// \param z1 Z value 1.
    /// \param z2 Z value 2.
    /// \return True if collides, false otherwise.
    constexpr bool collidesZ(float z1, float z2)
    {
        if(z1 > z2)
        {
            return collides_range(m_min->z(), m_max.z(), z2, z1);
        }
        return collides_range(m_min->z(), m_max.z(), z1, z2);
    }

protected:
    /// Tells if a range of values collides.
    ///
    /// \param min1 Minimum value 1.
    /// \param max1 Maximum value 1.
    /// \param min2 Minimum value 2.
    /// \param max2 Maximum value 2.
    static constexpr bool collides_range(float min1, float max1, float min2, float max2)
    {
        return !((max1 <= min2) || (max2 <= min1));
    }
};

}

#endif
