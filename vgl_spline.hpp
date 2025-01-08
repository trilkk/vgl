#ifndef VGL_SPLINE_HPP
#define VGL_SPLINE_HPP

#include "vgl_spline_point.hpp"
#include "vgl_vector.hpp"

namespace vgl
{

/// Bezier spline interpolator.
///
/// The class is simply named spline since it could theoretically be used for other 
class Spline
{
private:
    /// Points within the spline.
    vector<SplinePoint> m_points;

    /// Are the control points dirty for the spline?
    bool m_dirty = true;

private:
    /// Spline interpolation (bezier).
    ///
    /// \param idx Base index.
    /// \param interp Interpolation value.
    constexpr vec3 splineInterpolateBezier(int idx, float interp) const
    {
        const SplinePoint& curr = getPointClamped(idx);
        const SplinePoint& next = getPointClamped(idx + 1);
        const vec3& aa = curr.getPoint();
        const vec3& bb = curr.getNext();
        const vec3& cc = next.getPrev();
        const vec3& dd = next.getPoint();
        vec3 ee = mix(aa, bb, interp);
        vec3 ff = mix(bb, cc, interp);
        vec3 gg = mix(cc, dd, interp);
        vec3 hh =  mix(ee, ff, interp);
        vec3 ii =  mix(ff, gg, interp);

        return mix(hh, ii, interp);
    }

    /// Return point position at given point.
    ///
    /// Returns first/last points if outside spline.
    ///
    /// \param idx Index to access.
    /// \return Point at index.
    constexpr const SplinePoint& getPointClamped(int idx) const
    {
        if(static_cast<int>(m_points.size()) <= idx)
        {
            return m_points.back();
        }
        if(0 > idx)
        {
            return m_points.front();
        }
        return m_points[idx];
    }

    /// Precalculate points for spline.
    ///
    /// Only does anything if the spline is dirty.
    constexpr void precalculate()
    {
        if(!m_dirty)
        {
            return;
        }

        for(int ii = 0, ee = static_cast<int>(m_points.size()); (ii < ee); ++ii)
        {
            SplinePoint &vv = m_points[ii];
            const vec3& prev = getPointClamped(ii - 1).getPoint();
            const vec3& next = getPointClamped(ii + 1).getPoint();
            const vec3& curr = vv.getPoint();

            vv.setPrev(normalize(prev - next) * dnload_sqrtf(length(prev - curr)) + curr);
            vv.setNext(normalize(next - prev) * dnload_sqrtf(length(next - curr)) + curr);
        }

        m_dirty = false;
    }

public:
    /// Add spline point.
    ///
    /// \param pos Position.
    /// \param stamp Timestamp.
    void addPoint(const vec3 &pos, float stamp)
    {
#if defined(VGL_USE_LD)
        if(0.0f > stamp)
        {
            VGL_THROW_RUNTIME_ERROR("tried to add spline point with invalid timestamp: " + to_string(stamp));
        }
#endif
        m_points.emplace_back(pos, stamp);
        m_dirty = true;
    }
    /// Add spline point wrapper
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param pz Z coordinate.
    /// \param stamp Timestamp.
    void addPoint(int16_t px, int16_t py, int16_t pz, int16_t stamp)
    {
        addPoint(vec3(static_cast<float>(px), static_cast<float>(py), static_cast<float>(pz)),
                static_cast<float>(stamp));
    }

    /// Read data.
    ///
    /// \param data Data iteration pointer.
    /// \return Data pointer after read.
    const int16_t* readData(const int16_t *data)
    {
        const int16_t *iter = data;
        for(;;)
        {
            const int16_t *next = iter + 4;
            if(is_segment_end(iter))
            {
                return next;
            }
            addPoint(iter[0], iter[1], iter[2], iter[3]);
            iter = next;
        }
    }

    /// Get position at given time.
    ///
    /// \param stamp Timestamp to get position for.
    constexpr vec3 resolvePosition(float stamp)
    {
#if defined(VGL_USE_LD)
        if(m_points.empty())
        {
            VGL_THROW_RUNTIME_ERROR("incorrect spline data");
        }
#endif
        precalculate();

        float current_time = 0.0f;
        for(unsigned ii = 0; (m_points.size() > ii); ++ii)
        {
            const SplinePoint &vv = m_points[ii];
            float current_segment = vv.getTimestamp();

            if((current_time + current_segment) > stamp)
            {
                float interp = (stamp - current_time) / current_segment;
                return splineInterpolateBezier(ii, interp);
            }
            current_time += current_segment;
        }

        return m_points.back().getPoint();
    }
    /// Resolve position wrapper.
    ///
    /// \param stamp Timestamp to get position for.
    constexpr vec3 resolvePosition(int stamp)
    {
        return resolvePosition(static_cast<float>(stamp));
    }

public:
    /// Tell if this is an end of a spline segment data blob.
    ///
    /// \param data Input data.
    /// \return True if yes, false if no.
    constexpr static bool is_segment_end(const int16_t *data)
    {
        return ((0 == data[0]) &&
                (0 == data[1]) &&
                (0 == data[2]) &&
                (0 == data[3]));
    }
};

}

#endif
