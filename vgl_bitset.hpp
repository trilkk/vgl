#ifndef VGL_BITSET_HPP
#define VGL_BITSET_HPP

#include "vgl_type_traits.hpp"

namespace vgl
{

/// Implementation of bitset.
template<unsigned N> class bitset
{
    static_assert(N >= 1, "bitset must have positive size");
    static_assert(N <= 32, "maximum bitset bit size is 32");

public:
    /// Proxy class allowing writing to a bitset.
    class reference
    {
    private:
        /// Target bitset.
        bitset<N>& m_ref;

        /// Index.
        unsigned m_index;

    private:
        /// Deleted copy constructor.
        reference(const reference&) = delete;
        /// Deleted assignment.
        reference& operator=(const reference&) = delete;

    public:
        /// Constructor.
        ///
        /// \param src Source bitset.
        /// \param idx Index.
        constexpr explicit reference(bitset<N>& src, unsigned idx) noexcept :
            m_ref(src),
            m_index(idx)
        {
        }

    public:
        /// Flips the select bit.
        constexpr reference& flip()
        {
            m_ref.flip(m_index);
            return *this;
        }

    public:
        /// Assignment operator.
        ///
        /// \param op Bool value to write to the bitset.
        constexpr reference& operator=(bool op)
        {
            m_ref.set(m_index, op);
            return *this;
        }

        /// Truth value operator.
        ///
        /// \return True if bitst is not empty.
        constexpr operator bool() const
        {
            return m_ref.getInternal(m_index);
        }

    public:
#if defined(USE_LD)
        /// Stream output operator.
        ///
        /// \param lhs Left-hand-side operand.
        /// \param rhs Right-hand-side operand.
        /// \return Output stream.
        friend std::ostream& operator<<(std::ostream& lhs, const reference& rhs)
        {
            return lhs << static_cast<bool>(rhs);
        }
#endif
    };

private:
    /// Internal bit field data.
    uint32_t m_data = 0;

public:
    /// Default constructor.
    constexpr explicit bitset() = default;

    /// Constructor.
    ///
    /// \param op Initial value.
    constexpr explicit bitset(int op) :
        m_data(static_cast<uint32_t>(op))
    {
        assertData();
    }

    /// Constructor.
    ///
    /// \param op Initial value.
    constexpr explicit bitset(uint32_t op) :
        m_data(op)
    {
        assertData();
    }

private:
    /// Boundary check.
    ///
    /// \param idx Index to check.
    constexpr void accessCheck(unsigned idx) const
    {
#if defined(USE_LD) && defined(DEBUG)
        if(idx >= N)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("accessing bit index " + std::to_string(idx) +
                        " from " + std::to_string(N) + "-bit set"));
        }
#else
        (void)idx;
#endif
    }
    /// Boundary check.
    ///
    /// \param idx Index to check.
    constexpr void accessCheck(int idx) const
    {
#if defined(USE_LD) && defined(DEBUG)
        if(idx < 0)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("accessing negative bit index " +
                        std::to_string(idx) + " from " + std::to_string(N) + "-bit set"));
        }
#endif
        accessCheck(static_cast<unsigned>(idx));
    }

    /// Assert that the data only has bits in the valid range.
    constexpr void assertData() const
    {
#if defined(USE_LD) && defined(DEBUG)
        if(m_data & (!mask()))
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("bitset value " + std::to_string(m_data) +
                        " has bits outside " + std::to_string(N) + "-bit range"));
        }
#endif
    }

    /// Gets a bit value from the set.
    ///
    /// \param idx Bit index.
    constexpr bool getInternal(unsigned idx)
    {
        accessCheck(idx);
        return ((m_data & (static_cast<uint32_t>(1) << idx)) != 0);
    }
    /// Gets a bit value from the set.
    ///
    /// \param idx Bit index.
    constexpr bool getInternal(int idx)
    {
        accessCheck(idx);
        return ((m_data & (static_cast<uint32_t>(1) << idx)) != 0);
    }

public:
    /// Checks if all bits are set to true.
    ///
    /// \return False if any bit is not set.
    constexpr bool all() const
    {
        return (m_data == mask());
    }

    /// Checks if any bit is set to true.
    ///
    /// \return True if any bit is set.
    constexpr bool any() const
    {
        return (m_data != 0);
    }

    /// Checks if the bitset is empty.
    constexpr bool none() const
    {
        return (m_data == 0);
    }

    /// Set a bit.
    ///
    /// \param idx Index.
    /// \param value Value (default: true).
    constexpr bitset<N>& set(unsigned idx, bool value = true)
    {
        accessCheck(idx);
        if(value)
        {
            m_data |= static_cast<uint32_t>(1) << idx;
        }
        else
        {
            m_data &= !(static_cast<uint32_t>(1) << idx);
        }
        return *this;
    }

    /// Flip a bit.
    ///
    /// \param idx Index.
    constexpr bitset<N>& flip(unsigned idx)
    {
        m_data ^= static_cast<uint32_t>(1) << idx;
        return *this;
    }

public:
    /// Access operator.
    ///
    /// \return Element reference.
    constexpr reference operator[](unsigned idx)
    {
        accessCheck(idx);
        return reference(*this, idx);
    }
    /// Const access operator.
    ///
    /// \return Element reference.
    constexpr bool operator[](unsigned idx) const
    {
        return getInternal(idx);
    }
    /// Access operator.
    ///
    /// \return Element reference.
    constexpr reference operator[](int idx)
    {
        accessCheck(idx);
        return reference(*this, static_cast<unsigned>(idx));
    }
    /// Const access operator.
    ///
    /// \return Element reference.
    constexpr bool operator[](int idx) const
    {
        return getInternal(idx);
    }

    /// AND assignment operator.
    ///
    /// \param op Source object
    constexpr bitset<N>& operator&=(const bitset<N>& op)
    {
        m_data &= op.m_data;
        return *this;
    }

    /// OR into operator.
    ///
    /// \param op Source value.
    constexpr bitset<N>& operator|=(const bitset<N>& op) noexcept
    {
        m_data |= op.m_data;
        return *this;
    }

    /// XOR into operator.
    ///
    /// \param op Source value.
    constexpr bitset<N>& operator^=(const bitset<N>& op) noexcept
    {
        m_data ^= op.m_data;
        return *this;
    }

    /// Truth value operator.
    ///
    /// \return True if bitst is not empty.
    constexpr operator bool() const
    {
        return any();
    }

private:
    /// Mask for the allowed values.
    constexpr static uint32_t mask()
    {
        uint32_t ret = 0;
        for(unsigned ii = 0; (ii < N); ++ii)
        {
            ret |= (static_cast<uint32_t>(1) << ii);
        }
        return ret;
    }

    /// Gets the number of bits the bitset can hold.
    ///
    /// \return The template variable the bitset was constructed with.
    constexpr static unsigned size()
    {
        return N;
    }
};

}

#endif
