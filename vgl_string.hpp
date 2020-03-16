#ifndef VGL_STRING_HPP
#define VGL_STRING_HPP

#include "vgl_realloc.hpp"

#include <cstring>

namespace vgl
{

namespace detail
{

/// Internal strlen.
///
/// \param op C string.
/// \return Length of string minus terminating null.
constexpr unsigned internal_strlen(const char* op)
{
    const char* ii = op;
    while(*ii)
    {
        ++ii;
    }
    return static_cast<unsigned>(ii - op);
}

/// Internal memcpy.
///
/// \param dst Destination pointer.
/// \param src Source pointer.
/// \param count Number of bytes to copy.
/// \return Destination pointer.
constexpr void* internal_memcpy(void* dst, const void* src, unsigned count)
{
    uint8_t* udst = static_cast<uint8_t*>(dst);
    const uint8_t* usrc = static_cast<const uint8_t*>(src);
    for(unsigned ii = 0; (ii < count); ++ii)
    {
        udst[ii] = usrc[ii];
    }
    return dst;
}

/// String replacement.
template<typename T> class basic_string
{
public:
    /// Iterator type.
    using iterator = T*;
    /// Const iterator type.
    using const_iterator = const T*;

private:
    /// Internal data.
    T* m_data = nullptr;

    /// String length.
    unsigned m_length = 0;

public:
    /// Constructor.
    explicit constexpr basic_string() = default;

    /// Constructor.
    ///
    /// \param op C string input.
    explicit basic_string(const T* op)
    {
        assign(op);
    }

    /// Copy constructor.
    ///
    /// \param op C string input.
    basic_string(const basic_string<T>& op)
    {
        assign(op);
    }

    /// Move constructor.
    ///
    /// \param op C string input.
    basic_string(basic_string<T>&& op) :
        m_data(op.m_data),
        m_length(op.m_length)
    {
        op.m_data = nullptr;
        op.m_length = 0;
    }

    /// Destructor.
    ~basic_string()
    {
        array_delete(m_data);
    }

public:
    /// Iterator to the beginning.
    ///
    /// \return Iterator.
    constexpr iterator begin() const
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
    constexpr iterator end() const
    {
        return m_data + m_length;
    }
    /// Iterator to the end.
    ///
    /// \return Iterator.
    constexpr const_iterator cend() const
    {
        return m_data + m_length;
    }

    /// Assign data.
    ///
    /// \param op Input data.
    /// \return This object.
    basic_string& assign(const T* op)
    {
        clear();
        if(op && *op)
        {
            m_length = internal_strlen(op);
            unsigned copy_length = m_length + 1;
            m_data = array_new(m_data, copy_length);
            internal_memcpy(m_data, op, copy_length);
        }
        return *this;
    }
    /// Assign data.
    ///
    /// \param op Input data.
    /// \return This object.
    basic_string& assign(const basic_string& op)
    {
        clear();
        if(!op.empty())
        {
            m_length = op.length();
            unsigned copy_length = m_length + 1;
            m_data = array_new(m_data, copy_length);
            internal_memcpy(m_data, op, copy_length);
        }
        return *this;
    }

    /// Clear the string data.
    void clear()
    {
        array_delete(m_data);
        m_data = nullptr;
        m_length = 0;
    }

    /// Is the string empty?
    ///
    /// \return True if empty, false otherwise.
    constexpr bool empty() const
    {
        return (m_length <= 0);
    }

    /// Accessor.
    ///
    /// \return String length (without terminating zero).
    constexpr unsigned length() const
    {
        return m_length;
    }

    /// Return C string representing the string.
    ///
    /// \return C string pointer.
    constexpr const T* c_str() const
    {
        if(m_data)
        {
            return m_data;
        }
        return reinterpret_cast<const T*>("");
    }

public:
    /// Assignment operator.
    ///
    /// \param op Input data.
    /// \return This object.
    basic_string& operator=(const char* op)
    {
        return assign(op);
    }
    /// Assignment operator.
    ///
    /// \param op Input data.
    /// \return This object.
    basic_string& operator=(const basic_string<T>& op)
    {
        return assign(op);
    }

    /// Move operator.
    ///
    /// \param op Source object.
    /// \return This object.
    basic_string& operator=(basic_string<T>&& op)
    {
        m_data = op.m_data;
        m_length = op.m_length;
        op.m_data = nullptr;
        op.m_length = 0;
    }

#if defined(USE_LD)
    /// Stream output operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const basic_string<T>& rhs)
    {
        return lhs << rhs.c_str();
    }
#endif
};

}

/// String type definition.
using string = detail::basic_string<char>;

}

#endif
