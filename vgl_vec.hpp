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
template<unsigned N, typename ReflectionType> class vec
{
public:
    /// Publicly visible data size.
    static const unsigned data_size = N;

    /// Publicly visible CRTP type.
    using CrtpType = ReflectionType;

protected:
    /// Vector data.
    array<float, data_size> m_data;

protected:
    /// Empty constructor.
    constexpr explicit vec() noexcept
    {
    }

    /// Constructor.
    ///
    /// \param op Value for all components.
    constexpr explicit vec(float op) noexcept
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
    constexpr vec(float px, float py) noexcept
    {
        m_data[0u] = px;
        m_data[1u] = py;
    }

    /// Clunky initializing constructor.
    ///
    /// \param px First element.
    /// \param py Second element.
    /// \param pz Third element.
    constexpr vec(float px, float py, float pz) noexcept
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
    constexpr vec(float px, float py, float pz, float pw) noexcept
    {
        m_data[0u] = px;
        m_data[1u] = py;
        m_data[2u] = pz;
        m_data[3u] = pw;
    }

protected:
    /// Gets this object as the CRTP type.
    ///
    /// \return This as CRTP type.
    CrtpType* crtpThis() noexcept
    {
        return static_cast<CrtpType*>(this);
    }
    /// Gets this object as the CRTP type.
    ///
    /// \return This as CRTP type.
    const CrtpType* crtpThis() const noexcept
    {
        return static_cast<const CrtpType*>(this);
    }

public:
    /// Accessor.
    ///
    /// \return Data.
    constexpr float* data() noexcept
    {
      return m_data.data();
    }
    /// Accessor.
    ///
    /// \return Data.
    constexpr const float* data() const noexcept
    {
      return m_data.data();
    }

    /// Accessor.
    ///
    /// \return X component.
    constexpr float x() const noexcept
    {
        return m_data[0u];
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
    constexpr CrtpType& operator=(const CrtpType& rhs) noexcept
    {
        for(unsigned ii = 0; (ii < data_size); ++ii)
        {
            (*this)[ii] = rhs[ii];
        }
        return *crtpThis();
    }

    /// Unary minus operator.
    ///
    /// \return Result vector.
    constexpr CrtpType operator-() const noexcept
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < data_size); ++ii)
        {
            ret[ii] = -(*this)[ii];
        }
        return ret;
    }

    /// Addition into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    constexpr CrtpType& operator+=(const CrtpType& rhs) noexcept
    {
        return *crtpThis() = (*crtpThis()) + rhs;
    }
    /// Addition into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    constexpr CrtpType& operator+=(float rhs) noexcept
    {
        return *crtpThis() = (*crtpThis()) + rhs;
    }

    /// Subtraction into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    constexpr CrtpType& operator-=(const CrtpType& rhs) noexcept
    {
        return *crtpThis() = (*crtpThis()) - rhs;
    }
    /// Subtraction into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    constexpr CrtpType& operator-=(float rhs) noexcept
    {
        return *crtpThis() = (*crtpThis()) - rhs;
    }

    /// Multiplication into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    constexpr CrtpType& operator*=(const CrtpType& rhs) noexcept
    {
        return *crtpThis() = (*crtpThis()) * rhs;
    }
    /// Multiplication into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    constexpr CrtpType& operator*=(float rhs) noexcept
    {
        return *crtpThis() = (*crtpThis()) * rhs;
    }

    /// Division into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    constexpr CrtpType& operator/=(const CrtpType& rhs) noexcept
    {
        return *crtpThis() = (*crtpThis()) / rhs;
    }
    /// Division into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    constexpr CrtpType& operator/=(float rhs) noexcept
    {
        return *crtpThis() = (*crtpThis()) / rhs;
    }

public:
    /// Addition operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr friend CrtpType operator+(const CrtpType& lhs, const CrtpType& rhs) noexcept
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < data_size); ++ii)
        {
            ret[ii] = lhs[ii] + rhs[ii];
        }
        return ret;
    }
    /// Addition operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr friend CrtpType operator+(const CrtpType& lhs, float rhs) noexcept
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < data_size); ++ii)
        {
            ret[ii] = lhs[ii] + rhs;
        }
        return ret;
    }
    /// Addition operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr friend CrtpType operator+(float lhs, const CrtpType& rhs) noexcept
    {
        return rhs + lhs;
    }

    /// Subtraction operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr friend CrtpType operator-(const CrtpType& lhs, const CrtpType& rhs) noexcept
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < data_size); ++ii)
        {
            ret[ii] = lhs[ii] - rhs[ii];
        }
        return ret;
    }
    /// Subtraction operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr friend CrtpType operator-(const CrtpType& lhs, float rhs) noexcept
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < data_size); ++ii)
        {
            ret[ii] = lhs[ii] - rhs;
        }
        return ret;
    }
    /// Subtraction operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr friend CrtpType operator-(float lhs, const CrtpType& rhs) noexcept
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < data_size); ++ii)
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
    constexpr friend CrtpType operator*(const CrtpType& lhs, const CrtpType& rhs) noexcept
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < data_size); ++ii)
        {
            ret[ii] = lhs[ii] * rhs[ii];
        }
        return ret;
    }
    /// Multiplication operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr friend CrtpType operator*(const CrtpType& lhs, float rhs) noexcept
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < data_size); ++ii)
        {
            ret[ii] = lhs[ii] * rhs;
        }
        return ret;
    }
    /// Multiplication operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr friend CrtpType operator*(float lhs, const CrtpType& rhs) noexcept
    {
        return rhs * lhs;
    }

    /// Division operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr friend CrtpType operator/(const CrtpType& lhs, const CrtpType& rhs) noexcept
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < data_size); ++ii)
        {
            ret[ii] = lhs[ii] / rhs[ii];
        }
        return ret;
    }
    /// Division operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr friend CrtpType operator/(const CrtpType& lhs, float rhs) noexcept
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < data_size); ++ii)
        {
            ret[ii] = lhs[ii] / rhs;
        }
        return ret;
    }
    /// Division operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr friend CrtpType operator/(float lhs, const CrtpType& rhs) noexcept
    {
        CrtpType ret;
        for(unsigned ii = 0; (ii < data_size); ++ii)
        {
            ret[ii] = lhs / rhs[ii];
        }
        return ret;
    }

    /// Equals operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Truth value.
    constexpr friend bool operator==(const CrtpType& lhs, const CrtpType& rhs) noexcept
    {
        for(unsigned ii = 0; (ii < data_size); ++ii)
        {
            if(lhs[ii] != rhs[ii])
            {
                return false;
            }
        }
        return true;
    }
    /// Not equals operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Truth value.
    constexpr friend bool operator!=(const CrtpType& lhs, const CrtpType& rhs) noexcept
    {
        return !(lhs == rhs);
    }

#if defined(VGL_USE_LD)
    /// Stream output operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const CrtpType& rhs)
    {
        lhs << "[ " << rhs.x();
        for(unsigned ii = 1; (ii < data_size); ++ii)
        {
            lhs << " ; " << rhs[ii];
        }
        return lhs << " ]";
    }
#endif
};

}

}

#endif
