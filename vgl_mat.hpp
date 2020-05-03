#ifndef VGL_MAT_HPP
#define VGL_MAT_HPP

#include "vgl_vec.hpp"

namespace vgl
{

namespace detail
{

/// Mathematical GLSL-lookalike templated matrix class.
template<unsigned A, typename CrtpType> class mat
{
public:
    /// Number of elements (not axis).
    static const unsigned N = A * A;

private:
    /// Matrix data.
    array<float, N> m_data;

protected:
    /// Empty constructor.
    constexpr explicit mat()
    {
    }

    /// Constructor.
    ///
    /// \param op Value for all components.
    constexpr explicit mat(float op)
    {
        for(unsigned ii = 0; (ii < N); ++ii)
        {
            m_data[ii] = op;
        }
    }

    /// Clunky initializing constructor.
    ///
    /// \param op1 First value.
    /// \param op2 Second value.
    /// \param op3 Third value.
    /// \param op4 Fourth value.
    constexpr explicit mat4(float op1, float op2, float op3, float op4)
    {
        m_data[0u] = op1;
        m_data[1u] = op2;
        m_data[2u] = op3;
        m_data[3u] = op4;
    }

    /// Clunky initializing constructor.
    ///
    /// \param op1 First value.
    /// \param op2 Second value.
    /// \param op3 Third value.
    /// \param op4 Fourth value.
    /// \param op5 Fifth value.
    /// \param op6 Sixth value.
    /// \param op7 Seventh value.
    /// \param op8 Eighth value.
    /// \param op9 Ninth value.
    constexpr explicit mat4(float op1, float op2, float op3, float op4, float op5, float op6, float op7, float op8,
            float op9)
    {
        m_data[0u] = op1;
        m_data[1u] = op2;
        m_data[2u] = op3;
        m_data[3u] = op4;
        m_data[4u] = op5;
        m_data[5u] = op6;
        m_data[6u] = op7;
        m_data[7u] = op8;
        m_data[8u] = op9;
    }

    /// Clunky initializing constructor.
    ///
    /// \param op1 First value.
    /// \param op2 Second value.
    /// \param op3 Third value.
    /// \param op4 Fourth value.
    /// \param op5 Fifth value.
    /// \param op6 Sixth value.
    /// \param op7 Seventh value.
    /// \param op8 Eighth value.
    /// \param op9 Ninth value.
    /// \param op10 Tenth value.
    /// \param op11 Eleventh value.
    /// \param op12 Twelwth value.
    /// \param op13 Thirteenth value.
    /// \param op14 Foiurteenth value.
    /// \param op15 Fifteenth value.
    /// \param op16 Sixteenth value.
    constexpr explicit mat4(float op1, float op2, float op3, float op4, float op5, float op6, float op7, float op8,
            float op9, float op10, float op11, float op12, float op13, float op14, float op15, float op16)
    {
        m_data[0u] = op1;
        m_data[1u] = op2;
        m_data[2u] = op3;
        m_data[3u] = op4;
        m_data[4u] = op5;
        m_data[5u] = op6;
        m_data[6u] = op7;
        m_data[7u] = op8;
        m_data[8u] = op9;
        m_data[9u] = op10;
        m_data[10u] = op11;
        m_data[11u] = op12;
        m_data[12u] = op13;
        m_data[13u] = op14;
        m_data[14u] = op15;
        m_data[15u] = op16;
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
    constexpr CrtpType& operator=(const CrtpType& rhs) noexcept
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
    constexpr CrtpType operator-() noexcept
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
    constexpr CrtpType operator+(const CrtpType& rhs) const noexcept
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
    constexpr CrtpType operator+(float rhs) const noexcept
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
    constexpr CrtpType& operator+=(const CrtpType& rhs) noexcept
    {
        *crtpThis() = *crtpThis() + rhs;
        return *crtpThis();
    }
    /// Addition into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    constexpr CrtpType& operator+=(float rhs) noexcept
    {
        *crtpThis() = *crtpThis() + rhs;
        return *crtpThis();
    }

    /// Subtraction operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr CrtpType operator-(const CrtpType& rhs) const noexcept
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
    constexpr CrtpType operator-(float rhs) const noexcept
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
    constexpr CrtpType& operator-=(const CrtpType& rhs) noexcept
    {
        *crtpThis() = *crtpThis() - rhs;
        return *crtpThis();
    }
    /// Subtraction into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    constexpr CrtpType& operator-=(float rhs) noexcept
    {
        *crtpThis() = *crtpThis() - rhs;
        return *crtpThis();
    }

    /// Multiplication operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr CrtpType operator*(const CrtpType& rhs) const noexcept
    {
        CrtpType ret(0.0f);
        for(unsigned ii = 0; (ii < N); ii += A)
        { 
            for(unsigned jj = 0; (jj < A); ++jj)
            {
                float& dst = ret[ii + jj];
                for(unsigned kk = 0; (kk < A); ++kk)
                {
                    dst += m_data[(kk * A) + jj] * rhs[ii + kk];
                }
            }
        }
        return ret;
    }
    /// Multiplication operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr CrtpType operator*(float rhs) const noexcept
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
    constexpr CrtpType& operator*=(const CrtpType& rhs) noexcept
    {
        *crtpThis() = *crtpThis() * rhs;
        return *crtpThis();
    }
    /// Multiplication into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    constexpr CrtpType& operator*=(float rhs) noexcept
    {
        *crtpThis() = *crtpThis() * rhs;
        return *crtpThis();
    }

    /// Division operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr CrtpType operator/(float rhs) const noexcept
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
    constexpr CrtpType& operator/=(float rhs) noexcept
    {
        *crtpThis() = *crtpThis() / rhs;
        return *crtpThis();
    }

    /// Equals operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Truth value.
    constexpr bool operator==(const CrtpType& rhs) const noexcept
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
    constexpr bool operator!=(const CrtpType& rhs) const noexcept
    {
        return !(*crtpThis() == rhs);
    }

public:
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
    constexpr friend CrtpType operator-(float lhs, const CrtpType& rhs) noexcept
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
    constexpr friend CrtpType operator*(float lhs, const CrtpType& rhs) noexcept
    {
        return rhs * lhs;
    }

    /// Division operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    constexpr friend CrtpType operator/(float lhs, const CrtpType& rhs) noexcept
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
        for(unsigned ii = 1; (ii < N); ii += A)
        {
            lhs << " ; " << rhs[ii];
        }
        return lhs << " ]";
    }
#endif

public:
    /// Mix between two matrices.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \param ratio Mixing ratio.
    constexpr friend CrtpType mix(const CrtpType& lhs, const CrtpType& rhs, float ratio) noexcept
    {
        return lhs + (rhs - lhs) * ratio;
    }

    /// Test if matrices are almost equal.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return True if almost equal, false otherwise.
    constexpr friend bool almost_equal(const CrtpType& lhs, const CrtpType& rhs) noexcept
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
    /// Test if optional matrices are almost equal.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return True if almost equal, false otherwise.
    constexpr friend bool almost_equal(const optional<CrtpType>& lhs, const optional<CrtpType>& rhs) noexcept
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
