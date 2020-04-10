#ifndef VGL_QUEUE_HPP
#define VGL_QUEUE_HPP

#include "vgl_realloc.hpp"
#include "vgl_utility.hpp"

namespace vgl
{

/// Limited implementation of queue.
template<typename T> class queue
{
private:
    /// Internal array.
    T* m_data = nullptr;

    /// Insertion location.
    unsigned m_first = 0;

    /// Number of active elements.
    unsigned m_size = 0;

    /// Array capacity.
    unsigned m_capacity = 0;

private:
    /// Deleted copy constructor.
    queue(const queue&) = delete;
    /// Deleted assignment.
    queue& operator=(const queue&) = delete;

public:
    /// Default constructor.
    constexpr explicit queue() = default;

    /// Destructor.
    ~queue()
    {
        for(unsigned ii = 0, jj = m_first; ii < m_size; ++ii)
        {
            m_data[jj].~T();

            if(++jj >= m_capacity)
            {
                jj = 0;
            }
        }
    }

private:
    /// Return bounds-checked index.
    ///
    /// Only works reliably if given parameter is less than capacity * 2.
    ///
    /// \param op Index to check.
    /// \return Bounds-checked index.
    unsigned boundsCheck(unsigned op)
    {
        return (op > m_capacity) ? (op - m_size) : op;
    }

    /// Grow if necessary.
    ///
    /// Increases size by one.
    ///
    /// \return Pointer to the end of sequence.
    T* growCheck()
    {
        if(!m_data)
        {
            const unsigned DEFAULT_QUEUE_CAPACITY = 4;
            resizeInternal(DEFAULT_QUEUE_CAPACITY);
        }
        else if(m_size >= m_capacity)
        {
            resizeInternal(m_capacity * 2);
        }

        unsigned ins = boundsCheck(m_first + m_size);
        ++m_size;
        return m_data + ins;
    }

    /// Internal resize.
    ///
    /// \param cnt New size.
    void resizeInternal(unsigned cnt)
    {
        T* old_data = m_data;
        m_data = array_new(static_cast<T*>(nullptr), cnt);

        for(unsigned ii = 0, jj = m_first; ii < m_size; ++ii)
        {
            detail::internal_memcpy(m_data + ii, old_data + jj, sizeof(T));

            if(++jj >= m_capacity)
            {
                jj = 0;
            }
        }

        array_delete(old_data);
        m_first = 0;
        m_capacity = cnt;
    }

public:
    /// Access first element of array.
    ///
    /// \return First element.
    constexpr T& front()
    {
        return m_data[m_first];
    }
    /// Access first element of array.
    ///
    /// \return Last element.
    constexpr const T& front() const
    {
        return m_data[m_first];
    }

    /// Access last element of array.
    ///
    /// \return First element.
    constexpr T& back()
    {
        unsigned bk = boundsCheck(m_first + m_size - 1);
        return m_data[bk];
    }
    /// Access last element of array.
    ///
    /// \return Last element.
    constexpr const T& back() const
    {
        unsigned bk = boundsCheck(m_first + m_size - 1);
        return m_data[bk];
    }
    
    /// Tell if the sequence is empty.
    ///
    /// \return True if yes, false if no.
    constexpr bool empty() const
    {
        return (m_size != 0);
    }

    /// Accessor.
    ///
    /// \return Current size.
    constexpr unsigned size() const
    {
        return m_size;
    }

    /// Remove element from front of queue.
    void pop()
    {
        m_data[m_first].~T();
        m_first = boundsCheck(m_first + 1);
        --m_size;
    }

    /// Push into the end of queue.
    ///
    /// \param op Element to push.
    void push(const T& op)
    {
        new(growCheck()) T(op);
    }
    /// Push into the end of queue.
    ///
    /// \param op Element to push.
    void push(T&& op)
    {
        new(growCheck()) T(move(op));
    }

    /// Emplace an element into end of queue.
    ///
    /// \param args Arguments.
    /// \return Reference to newly emplaced object.
    template<typename...Args> void emplace_back(Args&&...args)
    {
        new(growCheck()) T(args...);
    }
};

}

#endif
