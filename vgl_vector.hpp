#ifndef VGL_VECTOR_HPP
#define VGL_VECTOR_HPP

#include "vgl_algorithm.hpp"
#include "vgl_realloc.hpp"
#include "vgl_utility.hpp"

namespace vgl
{

/// Sequential memory array.
///
/// The name vector was a mistake, but can no longer be changed.
/// Do not confuse with actual vectors, i.e. vec2, vec3, vec4.
template<typename T> class vector
{
public:
    /// Convenience typedef.
    using value_type = T;

    /// Iterator type.
    using iterator = T*;
    /// Const iterator type.
    using const_iterator = const T*;

private:
    /// Array data.
    T *m_data = nullptr;

    /// Array size.
    unsigned m_size = 0;

    /// Array capacity.
    unsigned m_capacity = 0;

private:
    /// Deleted copy constructor.
    vector(const vector&) = delete;
    /// Deleted assignment.
    vector& operator=(const vector&) = delete;

public:
    /// Default constructor.
    constexpr explicit vector() noexcept = default;

    /// Constructor.
    ///
    /// \param op Starting capacity.
    explicit vector(unsigned op) :
        m_data(array_new<T>(nullptr, op)),
        m_size(op),
        m_capacity(op)
    {
        for(unsigned ii = 0; ii < op; ++ii)
        {
            new(&m_data[ii]) T();
        }            
    }

    /// Move constructor.
    ///
    /// \param op Source object.
    constexpr vector(vector<T>&& op) noexcept :
        m_data(op.m_data),
        m_size(op.m_size),
        m_capacity(op.m_capacity)
    {
        op.m_data = nullptr;
        op.m_size = 0;
        op.m_capacity = 0;
    }

    /// Destructor.
    ~vector()
    {
        destructInternal();
        array_delete(m_data);
    }

private:
    /// Boundary check.
    ///
    /// \param idx Index to check.
    constexpr void accessCheck(unsigned idx) const
    {
#if defined(USE_LD) && defined(DEBUG)
        if(idx >= m_size)
        {
            std::ostringstream sstr;
            sstr << "accessing index " << idx << " from vector of size " << m_size;
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
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
            std::ostringstream sstr;
            sstr << "accessing negative index " << idx << " from vector of size " << m_size;
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif
        accessCheck(static_cast<unsigned>(idx));
    }

    /// Clear internals.
    constexpr void destructInternal()
    {
        for(T &vv : *this)
        {
            vv.~T();
        }
    }

    /// Grow if necessary.
    ///
    /// Increases size by one.
    ///
    /// \return Pointer to the end of sequence.
    T* growCheck()
    {
        if(m_size >= m_capacity)
        {
            const unsigned DEFAULT_CAPACITY = 4;
            resizeInternal(max(m_capacity * 2u, DEFAULT_CAPACITY));
        }

        // Return 
        iterator ret = end();
        ++m_size;
        return ret;
    }

    /// Internal resize.
    ///
    /// \param cnt New size.
    void resizeInternal(unsigned cnt)
    {
        m_data = array_new(m_data, cnt);
        m_capacity = cnt;
    }

public:
    /// Iterator to the beginning.
    ///
    /// \return Iterator.
    constexpr iterator begin() const noexcept
    {
        return m_data;
    }
    /// Iterator to the beginning.
    ///
    /// \return Iterator.
    constexpr const_iterator cbegin() const noexcept
    {
        return m_data;
    }

    /// Iterator to the end.
    ///
    /// \return Iterator.
    constexpr iterator end() const noexcept
    {
        return m_data + m_size;
    }
    /// Iterator to the end.
    ///
    /// \return Iterator.
    constexpr const_iterator cend() const noexcept
    {
        return m_data + m_size;
    }

    /// Accessor.
    ///
    /// \return First element.
    constexpr T& front()
    {
        accessCheck(0u);
        return m_data[0u];
    }
    /// Accessor.
    ///
    /// \return First element.
    constexpr const T& front() const
    {
        accessCheck(0u);
        return m_data[0u];
    }

    /// Accessor.
    ///
    /// \return Last element.
    constexpr T& back()
    {
        unsigned idx = m_size - 1;
        accessCheck(idx);
        return m_data[idx];
    }
    /// Accessor.
    ///
    /// \return Last element.
    constexpr const T& back() const
    {
        unsigned idx = m_size - 1;
        accessCheck(idx);
        return m_data[idx];
    }

    /// Accessor.
    ///
    /// \return Current capacity.
    constexpr unsigned capacity() const noexcept
    {
        return m_capacity;
    }

    /// Accessor.
    ///
    /// \return Pointer to data.
    constexpr T* data() noexcept
    {
        return m_data;
    }
    /// Const accessor.
    ///
    /// \return Pointer to data.
    constexpr const T* data() const noexcept
    {
        return m_data;
    }

    /// Tell if the sequence is empty.
    ///
    /// \return True if yes, false if no.
    constexpr bool empty() const noexcept
    {
        return (m_size == 0);
    }

    /// Get byte size.
    ///
    /// \return Current size in bytes.
    constexpr unsigned getSizeBytes() const
    {
        return static_cast<unsigned>(m_size * sizeof(T));
    }

    /// Accessor.
    ///
    /// \return Current size.
    constexpr unsigned size() const
    {
        return m_size;
    }

    /// Clear the array.
    ///
    /// Will NOT resize the array, the capacity will remain.
    constexpr void clear()
    {
        destructInternal();
        m_size = 0;
    }

    /// Remove last element from the array.
    ///
    /// Calling this function on an empty array has undefined results.
    constexpr void pop_back()
    {
        back().~T();
        --m_size;
    }

    /// Copy an element into the end of array.
    ///
    /// \param op Element to add.
    void push_back(const T &op)
    {
        new(growCheck()) T(op);
    }
    /// Move an element into the end of array.
    ///
    /// \param op Element to add.
    void push_back(T &&op)
    {
        new(growCheck()) T(move(op));
    }

    /// Emplace an element into array.
    ///
    /// \param args Arguments.
    /// \return Reference to newly emplaced object.
    template<typename...Args> void emplace_back(Args&&...args)
    {
        new(growCheck()) T(args...);
    }

    /// Resize to given size.
    ///
    /// \param cnt Count.
    void resize(unsigned cnt)
    {
        if(m_size == cnt)
        {
            return;
        }

        // Ensure capacity.
        if(cnt > m_capacity)
        {
            resizeInternal(cnt);
        }

        for(unsigned ii = m_size; (ii < cnt); ++ii)
        {
            new(&m_data[ii]) T();
        }

        for(unsigned ii = m_size; (ii > cnt); --ii)
        {
            m_data[ii - 1].~T();
        }

        m_size = cnt;
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
    /// Const access operator.
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
    /// Const access operator.
    ///
    /// \return Element reference.
    constexpr const T& operator[](int idx) const
    {
        accessCheck(idx);
        return m_data[idx];
    }

    /// Equals operator.
    ///
    /// \param op Other vector.
    constexpr bool operator==(const vector<T>& op) const noexcept
    {
        if(size() != op.size())
        {
            return false;
        }

        for(const_iterator ii = cbegin(), jj = op.cbegin(), ee = cend(); (ii != ee); ++ii, ++jj)
        {
            if(*ii != *jj)
            {
                return false;
            }
        }

        return true;
    }
    /// Not equals operator.
    ///
    /// \param op Other vector.
    constexpr bool operator!=(const vector<T>& op) const noexcept
    {
        return !(*this == op);
    }

    /// Move operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This object.
    constexpr vector<T>& operator=(vector<T>&& rhs) noexcept
    {
        m_data = rhs.m_data;
        m_size = rhs.m_size;
        m_capacity = rhs.m_capacity;
        rhs.m_data = nullptr;
        rhs.m_size = 0;
        rhs.m_capacity = 0;
        return *this;
    }

    /// Truth value operator.
    ///
    /// \return True if sequence is not empty.
    constexpr operator bool() const noexcept
    {
        return (0 < m_size);
    }
};

}

#endif
