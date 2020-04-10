#ifndef VGL_VEC_HPP
#define VGL_VEC_HPP

#include "vgl_array.hpp"
#include "vgl_math.hpp"
#include "vgl_optional.hpp"

namespace vgl
{

namespace detail
{

/// Mathematical GLSL-lookalike templated vector class.
template<unsigned N, typename CrtpType> class vec
{
protected:
    /// Vector data.
    array<float, N> m_data;

protected:
    /// Empty constructor.
    explicit vec()
    {
    }

    /// Constructor.
    ///
    /// \param op Value for all components.
    constexpr explicit vec(float op)
    {
        for(unsigned ii = 0; (ii < N); ++ii)
        {
            m_data[ii] = op;
        }
    }

    /// Clunky initializing constructor.
    ///
    /// \param px First element.
    /// \param py Second element.
    constexpr vec(float px, float py)
    {
        m_data[0u] = px;
        m_data[1u] = py;
    }

    /// Clunky initializing constructor.
    ///
    /// \param px First element.
    /// \param py Second element.
    /// \param pz Third element.
    constexpr vec(float px, float py, float pz)
    {
        m_data[0u] = px;
        m_data[1u] = py;
        m_data[2u] = pz;
    }

    /// Clunky initializing constructor.
    ///
    /// \param px First element.
    /// \param py Second element.
    /// \param pz Third element.
    /// \param pz Fourth element.
    constexpr vec(float px, float py, float pz, float pw)
    {
        m_data[0u] = px;
        m_data[1u] = py;
        m_data[2u] = pz;
        m_data[3u] = pw;
    }

public:
    /// Accessor.
    ///
    /// \return Data.
    constexpr float* data()
    {
      return m_data.data();
    }
    /// Accessor.
    ///
    /// \return Data.
    constexpr const float* data() const
    {
      return m_data.data();
    }

    /// Accessor.
    ///
    /// \return X component.
    constexpr float x() const
    {
        return m_data[0];
    }

    /// Gets this object as the CRTP type.
    ///
    /// \return This as CRTP type.
    CrtpType* crtpThis()
    {
        return static_cast<CrtpType*>(this);
    }
    /// Gets this object as the CRTP type.
    ///
    /// \return This as CRTP type.
    const CrtpType* crtpThis() const
    {
        return static_cast<const CrtpType*>(this);
    }

public:
    /// Access operator.
    ///
    /// \param idx Index.
    /// \return Value.
    constexpr float& operator[](unsigned idx)
    {
        return m_data[idx];
    }
    /// Const access operator.
    ///
    /// \param idx Index.
    /// \return Value.
    constexpr const float& operator[](unsigned idx) const
    {
        return m_data[idx];
    }
    /// Access operator.
    ///
    /// \param idx Index.
    /// \return Value.
    constexpr float& operator[](int idx)
    {
        return m_data[idx];
    }
    /// Const access operator.
    ///
    /// \param idx Index.
    /// \return Value.
    constexpr const float& operator[](int idx) const
    {
        return m_data[idx];
    }

    /// Assignment operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    constexpr CrtpType& operator=(const CrtpType& rhs)
    {
        for(unsigned ii = 0; (ii < N); ++ii)
        {
            m_data[ii] = rhs[ii];
        }
        return *crtpThis();
    }

    /// Unary minus operator.
    ///
    /// \return Result vector.
    constexpr CrtpType operator-()
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < N); ++ii)
        {
            ret[ii] = -m_data[ii];
        }
        return ret;
    }

    /// Addition operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr CrtpType operator+(const CrtpType& rhs) const
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < N); ++ii)
        {
            ret[ii] = m_data[ii] + rhs[ii];
        }
        return ret;
    }
    /// Addition operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr CrtpType operator+(float rhs) const
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < N); ++ii)
        {
            ret[ii] = m_data[ii] + rhs;
        }
        return ret;
    }
    /// Addition into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    constexpr CrtpType& operator+=(const CrtpType& rhs)
    {
        *crtpThis() = *crtpThis() + rhs;
        return *crtpThis();
    }
    /// Addition into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    constexpr CrtpType& operator+=(float rhs)
    {
        *crtpThis() = *crtpThis() + rhs;
        return *crtpThis();
    }


    /// Subtraction operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr CrtpType operator-(const CrtpType& rhs) const
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < N); ++ii)
        {
            ret[ii] = m_data[ii] - rhs[ii];
        }
        return ret;
    }
    /// Subtraction operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr CrtpType operator-(float rhs) const
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < N); ++ii)
        {
            ret[ii] = m_data[ii] - rhs;
        }
        return ret;
    }
    /// Subtraction into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    constexpr CrtpType& operator-=(const CrtpType& rhs)
    {
        *crtpThis() = *crtpThis() - rhs;
        return *crtpThis();
    }
    /// Subtraction into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    constexpr CrtpType& operator-=(float rhs)
    {
        *crtpThis() = *crtpThis() - rhs;
        return *crtpThis();
    }

    /// Multiplication operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr CrtpType operator*(const CrtpType& rhs) const
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < N); ++ii)
        {
            ret[ii] = m_data[ii] * rhs[ii];
        }
        return ret;
    }
    /// Multiplication operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr CrtpType operator*(float rhs) const
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < N); ++ii)
        {
            ret[ii] = m_data[ii] * rhs;
        }
        return ret;
    }
    /// Multiplication into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    constexpr CrtpType& operator*=(const CrtpType& rhs)
    {
        *crtpThis() = *crtpThis() * rhs;
        return *crtpThis();
    }
    /// Multiplication into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    constexpr CrtpType& operator*=(float rhs)
    {
        *crtpThis() = *crtpThis() * rhs;
        return *crtpThis();
    }

    /// Division operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr CrtpType operator/(const CrtpType& rhs) const
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < N); ++ii)
        {
            ret[ii] = m_data[ii] / rhs[ii];
        }
        return ret;
    }
    /// Division operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr CrtpType operator/(float rhs) const
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < N); ++ii)
        {
            ret[ii] = m_data[ii] / rhs;
        }
        return ret;
    }
    /// Division into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    constexpr CrtpType& operator/=(const CrtpType& rhs)
    {
        *crtpThis() = *crtpThis() / rhs;
        return *crtpThis();
    }
    /// Division into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    constexpr CrtpType& operator/=(float rhs)
    {
        *crtpThis() = *crtpThis() / rhs;
        return *crtpThis();
    }

    /// Equals operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Truth value.
    constexpr bool operator==(const CrtpType& rhs) const
    {
        for(unsigned ii = 0; (ii < N); ++ii)
        {
            if(m_data[ii] != rhs[ii])
            {
                return false;
            }
        }
        return true;
    }
    /// Not equals operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Truth value.
    constexpr bool operator!=(const CrtpType& rhs) const
    {
        return !(*crtpThis() == rhs);
    }

  public:
    /// Addition operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    friend constexpr CrtpType operator+(float lhs, const CrtpType& rhs)
    {
        return rhs + lhs;
    }

    /// Subtraction operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    friend constexpr CrtpType operator-(float lhs, const CrtpType& rhs)
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < N); ++ii)
        {
            ret[ii] = lhs - rhs[ii];
        }
        return ret;
    }

    /// Multiplication operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    friend constexpr CrtpType operator*(float lhs, const CrtpType& rhs)
    {
        return rhs * lhs;
    }

    /// Division operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    friend constexpr CrtpType operator/(float lhs, const CrtpType& rhs)
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < N); ++ii)
        {
            ret[ii] = lhs / rhs[ii];
        }
        return ret;
    }

#if defined(USE_LD)
    /// Stream output operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const CrtpType& rhs)
    {
        lhs << "[ " << rhs.x();
        for(unsigned ii = 1; (ii < N); ++ii)
        {
            lhs << " ; " << rhs[ii];
        }
        return lhs << " ]";
    }
#endif

  public:
    /// Dot product.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result value.
    friend constexpr float dot(const CrtpType& lhs, const CrtpType& rhs)
    {
        float ret = 0.0f;
        for(unsigned ii = 1; (ii < N); ++ii)
        {
            ret += lhs[ii] * rhs[ii];
        }
        return ret;
    }

    /// Mix two vectors.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \param ratio Mixing ratio.
    friend constexpr CrtpType mix(const CrtpType& lhs, const CrtpType& rhs, float ratio)
    {
        return lhs + (rhs - lhs) * ratio;
    }

    /// Component-wise maximum.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result value.
    friend constexpr CrtpType max(const CrtpType& lhs, const CrtpType& rhs)
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < N); ++ii)
        {
            ret[ii] = max(lhs[ii], rhs[ii]);
        }
        return ret;
    }
    /// Component-wise minimum.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result value.
    friend constexpr CrtpType min(const CrtpType& lhs, const CrtpType& rhs)
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < N); ++ii)
        {
            ret[ii] = min(lhs[ii], rhs[ii]);
        }
        return ret;
    }

    /// Calculates the length of a vector.
    ///
    /// \param op Vector input.
    /// \return Length.
    friend float length(const CrtpType& op)
    {
        return dnload_sqrtf(dot(op, op));
    }

    /// Normalize a vector.
    ///
    /// \param op Vector to normalize.
    /// \return Result vector.
    friend CrtpType normalize(const CrtpType& op)
    {
        float len = length(op);
        if(len > 0.0f)
        {
            return op / len;
        }
        return CrtpType(0.0f);
    }

    /// Test if vectors are almost equal.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return True if almost equal, false otherwise.
    friend constexpr bool almost_equal(const CrtpType& lhs, const CrtpType& rhs)
    {
        for(unsigned ii = 0; (ii < N); ++ii)
        {
            if(!floats_almost_equal(lhs[ii], rhs[ii]))
            {
                return false;
            }
        }
        return true;
    }

    /// Test if optional vectors are almost equal.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return True if almost equal, false otherwise.
    friend constexpr bool almost_equal(const optional<CrtpType>& lhs, const optional<CrtpType>& rhs)
    {
        if(lhs)
        {
            if(rhs)
            {
                return almost_equal(*lhs, *rhs);
            }
            return false;
        }
        else if(rhs)
        {
            return false;
        }
        return true;
    }
};

}

}

#endif
