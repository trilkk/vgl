#ifndef VGL_ARRAY_HPP
#define VGL_ARRAY_HPP

#if defined(USE_LD)
#include <sstream>
#include <boost/throw_exception.hpp>
#endif

namespace vgl
{

/// Replacement for std::array.
template<typename T, unsigned N> class array
{
public:
    /// Convenience typedef.
    using value_type = T;

    /// Iterator type.
    using iterator = T*;
    /// Const iterator type.
    using const_iterator = const T*;

public:
    /// Array data.
    T m_data[N];

private:
    /// Boundary check.
    ///
    /// \param idx Index to check.
    constexpr void accessCheck(unsigned idx) const
    {
#if defined(USE_LD) && defined(DEBUG)
        if(idx >= N)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("accessing index " + std::to_string(idx) +
                        " from array of size " + std::to_string(N)));
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
            BOOST_THROW_EXCEPTION(std::runtime_error("accessing negative index " + std::to_string(idx) +
                        " from array of size " + std::to_string(N)));
        }
#endif
        accessCheck(static_cast<unsigned>(idx));
    }

public:
    /// Iterator to the beginning.
    ///
    /// \return Iterator.
    constexpr iterator begin()
    {
        return m_data;
    }
    /// Iterator to the beginning.
    ///
    /// \return Iterator.
    constexpr const_iterator cbegin() const
    {
        return m_data;
    }

    /// Iterator to the end.
    ///
    /// \return Iterator.
    constexpr iterator end()
    {
        return m_data + N;
    }
    /// Iterator to the end.
    ///
    /// \return Iterator.
    constexpr const_iterator cend() const
    {
        return m_data + N;
    }

    /// Accessor.
    ///
    /// \return First element.
    constexpr T& front()
    {
        return m_data[0];
    }
    /// Accessor.
    ///
    /// \return First element.
    constexpr const T& front() const
    {
        return m_data[0];
    }

    /// Accessor.
    ///
    /// \return Last element.
    constexpr T& back()
    {
        return m_data[N - 1];
    }
    /// Accessor.
    ///
    /// \return Last element.
    constexpr const T& back() const
    {
        return m_data[N - 1];
    }

    /// Accessor.
    ///
    /// \return Pointer to data.
    constexpr T* data()
    {
        return m_data;
    }
    /// Accessor.
    ///
    /// \return Pointer to data.
    constexpr const T* data() const
    {
        return m_data;
    }

    /// Get byte size.
    ///
    /// \return Current size in bytes.
    constexpr static unsigned getSizeBytes()
    {
        return N * sizeof(T);
    }

    /// Accessor.
    ///
    /// \return Array size.
    constexpr static unsigned size()
    {
        return N;
    }

public:
    /// Access operator.
    ///
    /// \return Element reference.
    constexpr T& operator[](unsigned idx)
    {
        accessCheck(idx);
        return m_data[idx];
    }
    /// Access operator.
    ///
    /// \return Element reference.
    constexpr const T& operator[](unsigned idx) const
    {
        accessCheck(idx);
        return m_data[idx];
    }
    /// Access operator.
    ///
    /// \return Element reference.
    constexpr T& operator[](int idx)
    {
        accessCheck(idx);
        return m_data[idx];
    }
    /// Access operator.
    ///
    /// \return Element reference.
    constexpr const T& operator[](int idx) const
    {
        accessCheck(idx);
        return m_data[idx];
    }
};

}

#endif
