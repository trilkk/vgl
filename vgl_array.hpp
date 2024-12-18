#ifndef VGL_ARRAY_HPP
#define VGL_ARRAY_HPP

#if defined(USE_LD)
#include "vgl_throw_exception.hpp"
#endif

namespace vgl
{

/// Replacement for std::array.
template<typename T, unsigned N> class array
{
public:
    /// Value type.
    using value_type = T;
    /// Iterator type.
    using iterator = value_type*;
    /// Const iterator type.
    using const_iterator = const value_type*;
    /// Type of this class.
    using this_type = array<value_type, N>;

public:
    /// Array data.
    value_type m_data[N];

private:
    /// Boundary check.
    ///
    /// \param idx Index to check.
    constexpr void accessCheck(unsigned idx) const
    {
#if defined(USE_LD) && defined(DEBUG)
        if(idx >= N)
        {
            VGL_THROW_RUNTIME_ERROR("accessing index " + to_string(idx) + " from array of size " + to_string(N));
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
            VGL_THROW_RUNTIME_ERROR("accessing negative index " + to_string(idx) + " from array of size " +
                    to_string(N));
        }
#endif
        accessCheck(static_cast<unsigned>(idx));
    }

public:
    /// Iterator to the beginning.
    ///
    /// \return Iterator.
    constexpr iterator begin() noexcept
    {
        return m_data;
    }
    /// Const iterator to the beginning.
    ///
    /// \return Iterator.
    constexpr const_iterator begin() const noexcept
    {
        return m_data;
    }

    /// Iterator to the end.
    ///
    /// \return Iterator.
    constexpr iterator end() noexcept
    {
        return m_data + N;
    }
    /// Const iterator to the end.
    ///
    /// \return Const iterator.
    constexpr const_iterator end() const noexcept
    {
        return m_data + N;
    }

    /// Accessor.
    ///
    /// \return First element.
    constexpr T& front() noexcept
    {
        return m_data[0];
    }
    /// Accessor.
    ///
    /// \return First element.
    constexpr const T& front() const noexcept
    {
        return m_data[0];
    }

    /// Accessor.
    ///
    /// \return Last element.
    constexpr T& back() noexcept
    {
        return m_data[N - 1];
    }
    /// Accessor.
    ///
    /// \return Last element.
    constexpr const T& back() const noexcept
    {
        return m_data[N - 1];
    }

    /// Accessor.
    ///
    /// \return Pointer to data.
    constexpr T* data() noexcept
    {
        return m_data;
    }
    /// Accessor.
    ///
    /// \return Pointer to data.
    constexpr const T* data() const noexcept
    {
        return m_data;
    }

    /// Get byte size.
    ///
    /// \return Current size in bytes.
    constexpr static unsigned getSizeBytes() noexcept
    {
        return N * sizeof(T);
    }

    /// Accessor.
    ///
    /// \return Array size.
    constexpr static unsigned size() noexcept
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

public:
    VGL_ITERATOR_FUNCTIONS(this_type)
};

}

#endif
