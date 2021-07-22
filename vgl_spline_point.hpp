#ifndef VGL_SPLINE_POINT_HPP
#define VGL_SPLINE_POINT_HPP

#include "vgl_vec3.hpp"

namespace vgl
{

/// Spline point.
class SplinePoint
{
private:
    /// Spline point.
    vec3 m_point;

    /// Previous control point.
    vec3 m_prev;

    /// Next control point.
    vec3 m_next;

    /// Timestamp as difference to previous point.
    float m_timestamp;

public:
    /// Constructor.
    ///
    /// \param pos Position.
    /// \param stamp Timestamp.
    explicit SplinePoint(const vec3 &pos, float stamp) :
        m_point(pos),
        m_timestamp(stamp)
    {
    }

public:
    /// Accessor.
    ///
    /// \return Next control point.
    constexpr vec3 getNext() const
    {
        return m_next;
    }
    /// Setter.
    ///
    /// \param op New next control point.
    constexpr void setNext(const vec3 &op)
    {
        m_next = op;
    }

    /// Accessor.
    ///
    /// \return Actual point.
    constexpr vec3 getPoint() const
    {
        return m_point;
    }

    /// Accessor.
    ///
    /// \return Previous control point.
    constexpr vec3 getPrev() const
    {
        return m_prev;
    }
    /// Setter.
    ///
    /// \param op New previous control point.
    constexpr void setPrev(const vec3 &op)
    {
        m_prev = op;
    }

    /// Accessor.
    ///
    /// \return Timestamp.
    constexpr float getTimestamp() const
    {
        return m_timestamp;
    }
};

}

#endif
