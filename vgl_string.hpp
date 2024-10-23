#ifndef VGL_STRING_HPP
#define VGL_STRING_HPP

#include "vgl_limits.hpp"
#include "vgl_realloc.hpp"

#if defined(USE_LD)
#include "vgl_utility.hpp"
#include <ostream>
#endif

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

/// Base class for strings and string_views.
template<typename T> class string_data
{
public:
    /// Iterator type.
    using iterator = T*;
    /// Const iterator type.
    using const_iterator = const T*;

public:
    /// No-position.
    static const unsigned npos = numeric_limits<unsigned>::max();

protected:
    /// Internal data.
    T* m_data = nullptr;

    /// String length.
    unsigned m_length = 0;

protected:
    /// Default constructor.
    constexpr explicit string_data() noexcept = default;

    /// Constructor.
    ///
    /// \param data Data input.
    /// \param length Length.
    constexpr explicit string_data(T* data, unsigned length) noexcept :
        m_data(data),
        m_length(length)
    {
    }

private:
    /// Boundary check.
    ///
    /// \param idx Index to check.
    constexpr void accessCheck(unsigned idx) const
    {
#if defined(USE_LD) && defined(DEBUG)
        if(idx >= m_length)
        {
            // Can't use vgl_throw_exception.hpp yet.
            BOOST_THROW_EXCEPTION(std::runtime_error("accessing index " + std::to_string(idx) +
                        " from string of length " + std::to_string(m_length)));
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
            // Can't use vgl_throw_exception.hpp yet.
            BOOST_THROW_EXCEPTION(std::runtime_error("accessing negative index " + std::to_string(idx) +
                        " from string of length " + std::to_string(m_length)));
        }
#endif
        accessCheck(static_cast<unsigned>(idx));
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
        return m_data + m_length;
    }
    /// Iterator to the end.
    ///
    /// \return Iterator.
    constexpr const_iterator cend() const noexcept
    {
        return m_data + m_length;
    }

    /// Is the string empty?
    ///
    /// \return True if empty, false otherwise.
    constexpr bool empty() const noexcept
    {
        return (m_length <= 0);
    }

    /// Finds the occurrance of another substring.
    ///
    /// \param rhs Right-hand side operand.
    /// \param pos Position to start search from.
    /// \return Found index or npos if not found.
    constexpr unsigned find(const string_data& rhs, unsigned pos = 0) const noexcept
    {
        if(rhs.length() > m_length)
        {
            return npos;
        }
        unsigned remaining = m_length - rhs.length();
        for(unsigned ii = pos; (ii <= remaining); ++ii)
        {
            if(matchAtPosition(rhs, ii))
            {
                return ii;
            }
        }
        return npos;
    }

    /// Accessor.
    ///
    /// \return String length (without terminating zero).
    constexpr unsigned length() const noexcept
    {
        return m_length;
    }

    /// Returns the internal data pointer.
    ///
    /// \return Data pointer.
    constexpr const T* data() const noexcept
    {
        if(m_data)
        {
            return m_data;
        }
        return reinterpret_cast<const T*>("");
    }

    /// Tells if the string starts with given substring.
    ///
    /// \param op Substring to test.
    /// \return True if starts with given substring, false otherwise.
    constexpr bool starts_with(const T* op)
    {
        for(unsigned ii = 0;;)
        {
            if(!(*op))
            {
                return true;
            }
            else if(ii >= m_length)
            {
                return false;
            }

            if(*op != m_data[ii])
            {
                return false;
            }
            ++op;
            ++ii;
        }
    }

private:
    /// Checks if given string matches at given position.
    ///
    /// Parameters must be valid.
    ///
    /// \param rhs Right-hand side operand.
    /// \param pos Position to search from.
    /// \return True if found, false if not.
    constexpr bool matchAtPosition(const string_data& rhs, unsigned pos) const noexcept
    {
        for(unsigned ii = 0; (ii < rhs.length()); ++ii)
        {
            if(m_data[ii + pos] != rhs.m_data[ii])
            {
                return false;
            }
        }
        return true;
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

    /// Less than operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return True if this is less than rhs, false otherwise.
    template<typename R> constexpr bool operator<(const string_data<R>& rhs) const noexcept
    {
        unsigned common_length = min(length(), rhs.length());
        for(unsigned ii = 0; (ii < common_length); ++ii)
        {
            auto ccl = m_data[ii];
            auto ccr = rhs.m_data[ii];
            if(ccl < ccr)
            {
                return true;
            }
            else if(ccl > ccr)
            {
                return false;
            }
        }
        return length() < rhs.length();
    }

    /// Equals operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return True if equal, false if not.
    template<typename R> constexpr bool operator==(const string_data<R>& rhs) const noexcept
    {
        if(m_length != rhs.length())
        {
            return false;
        }

        for(unsigned ii = 0; (ii < m_length); ++ii)
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
    /// \return True if equal, false if not.
    template<typename R> constexpr bool operator!=(const string_data<R>& rhs) const noexcept
    {
        return !(*this == rhs);
    }

public:
#if defined(USE_LD)
    /// Conversion to STL string operator.
    ///
    /// \return STL string.
    operator std::string() const
    {
        return std::string(data(), length());
    }
    /// Conversion to STL string_view operator.
    ///
    /// \return STL string.
    operator std::string_view() const
    {
        return std::string_view(data(), length());
    }

    /// Stream output operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const string_data<T>& rhs)
    {
        for(const T& cc : rhs)
        {
            lhs << cc;
        }
        return lhs;
    }
#endif
};

}

/// String replacement.
class string : public detail::string_data<char>
{
private:
    /// Base class type.
    using base_type = detail::string_data<char>;

public:
    /// Constructor.
    constexpr explicit string() noexcept = default;

    /// Constructor.
    ///
    /// Intentionally not explicit.
    ///
    /// \param op C string input.
    string(const char* op)
    {
        assign(op);
    }

    /// Constructor.
    ///
    /// \param data String data.
    /// \param count Number of elements to read, excluding terminating zero.
    explicit string(const char* data, unsigned count)
    {
        assign(data, count);
    }
 
    /// Constructor.
    ///
    /// \param op String data input.
    explicit string(const string_data<const char>& op)
    {
        assign(op);
    }

#if defined(USE_LD)
    /// Consructor from iterators.
    ///
    /// \param first First iterator to insert.
    /// \param last End of iteration.
    template<typename InputIt> string(InputIt first, InputIt last)
    {
        resize(static_cast<unsigned>(last - first));
        for(unsigned ii = 0; (first != last); ++ii, ++first)
        {
            base_type::m_data[ii] = *first;
        }
    }

    /// Copy constructor.
    ///
    /// \param op String data input.
    string(const string& other) :
        base_type()
    {
        assign(other);
    }

    /// Copy operator.
    ///
    /// \param other Source object.
    /// \return This object.
    string& operator=(const string& other)
    {
        return assign(other);
    }
#else
    /// Deleted copy constructor.
    string(const string&) = delete;
    /// Deleted copy operator.
    string& operator=(const string&) = delete;
#endif

    /// Move constructor.
    ///
    /// \param op C string input.
    constexpr string(string&& op) noexcept :
        base_type(op.m_data, op.m_length)
    {
        op.m_data = nullptr;
        op.m_length = 0;
    }

    /// Move operator.
    ///
    /// \param op Source object.
    /// \return This object.
    constexpr string& operator=(string&& op) noexcept
    {
        base_type::m_data = op.m_data;
        base_type::m_length = op.m_length;
        op.m_data = nullptr;
#if defined(USE_LD) && defined(DEBUG)
        op.m_length = 0;
#endif
        return *this;
    }

    /// Destructor.
    ~string()
    {
        array_delete(base_type::m_data);
    }

public:
    /// Assign data.
    ///
    /// \param data Input data.
    /// \param len Data length.
    /// \return This object.
    string& assign(const char* data, unsigned len)
    {
#if defined(USE_LD)
        if(length() == len)
        {
            detail::internal_memcpy(base_type::m_data, data, len);
            return *this;
        }
#endif
        base_type::m_length = len;
        if(len > 0)
        {
            base_type::m_data = array_new(base_type::m_data, len + 1);
            detail::internal_memcpy(base_type::m_data, data, len);
            base_type::m_data[len] = 0;
        }
        else
        {
            array_delete(base_type::m_data);
            base_type::m_data = nullptr;
        }
        return *this;
    }
    /// Assign data.
    ///
    /// \param op Input data.
    /// \return This object.
    string& assign(const char* op)
    {
        if(!op)
        {
            return assign(op, 0);
        }
        return assign(op, detail::internal_strlen(op));
    }
    /// Assign data.
    ///
    /// \param op Input data.
    /// \return This object.
    string& assign(const string_data<const char>& op)
    {
        return assign(op.data(), op.length());
    }
    /// Assign data.
    ///
    /// \param op Input data.
    /// \return This object.
    string& assign(const string_data<char>& op)
    {
        return assign(const_cast<const char*>(op.data()), op.length());
    }

    /// Return C string representing the string.
    ///
    /// \return C string pointer.
    constexpr const char* c_str() const noexcept
    {
        return data();
    }

    /// Clear the string data.
    void clear()
    {
        array_delete(base_type::m_data);
        base_type::m_data = nullptr;
        base_type::m_length = 0;
    }

    /// Resizes the string.
    ///
    /// New characters are not initialized.
    ///
    /// \param new_size New size.
    void resize(unsigned new_size)
    {
        base_type::m_data = array_new(base_type::m_data, new_size + 1);
        base_type::m_data[new_size] = static_cast<char>(0);
        base_type::m_length = new_size;
    }
    /// Resizes the string.
    ///
    /// New characters are initialized to T().
    ///
    /// \param new_size New size.
    /// \param value Value for new elements.
    void resize(unsigned new_size, char value)
    {
        unsigned old_size = length();

        resize(new_size);

        while(old_size < new_size)
        {
            base_type::m_data[old_size] = value;
            ++old_size;
        }
    }

#if defined(USE_LD)
    /// Swap with another object.
    ///
    /// \param other Object to swap with.
    void swap(string& other)
    {
        vgl::swap(base_type::m_data, other.base_type::m_data);
        vgl::swap(base_type::m_length, other.base_type::m_length);
    }
#endif

public:
    /// Assignment operator.
    ///
    /// \param op Input data.
    /// \return This object.
    string& operator=(const char* op)
    {
        return assign(op);
    }
    /// Assignment operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This object.
    string& operator=(const string_data<const char>& rhs)
    {
        return assign(rhs.data());
    }

#if defined(USE_LD)
    /// Addition operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Concatenated string.
    string operator+(const string& rhs) const
    {
        string ret;
        auto new_size = length() + rhs.length();
        if(new_size)
        {
            ret.resize(new_size);
            if(length())
            {
                detail::internal_memcpy(ret.base_type::m_data, data(), length());
            }
            if(ret.length())
            {
                detail::internal_memcpy(ret.base_type::m_data + length(), rhs.data(), rhs.length());
            }
        }
        return ret;
    }

    /// Addition operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Concatenated string.
    string operator+(const char* rhs) const
    {
        string ret;
        auto rhs_len = detail::internal_strlen(rhs);
        auto new_size = length() + rhs_len;
        if(new_size)
        {
            ret.resize(new_size);
            if(length())
            {
                detail::internal_memcpy(ret.base_type::m_data, data(), length());
            }
            if(rhs_len)
            {
                detail::internal_memcpy(ret.base_type::m_data + length(), rhs, rhs_len);
            }
        }
        return ret;
    }

    /// Addition into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This object.
    string& operator+=(const string& rhs)
    {
        if(rhs.length())
        {
            auto old_size = length();
            auto new_size = old_size + rhs.length();
            resize(new_size);
            detail::internal_memcpy(base_type::m_data + old_size, rhs.data(), rhs.length());
        }
        return *this;
    }

    /// Addition into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This object.
    string& operator+=(const char* rhs)
    {
        auto rhs_len = detail::internal_strlen(rhs);
        if(rhs_len)
        {
            auto old_size = length();
            auto new_size = old_size + rhs_len;
            resize(new_size);
            detail::internal_memcpy(base_type::m_data + old_size, rhs, rhs_len);
        }
        return *this;
    }

public:
    /// Addition operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Concatenated string.
    friend string operator+(const char* lhs, const string& rhs)
    {
        string ret;
        auto lhs_len = detail::internal_strlen(lhs);
        auto new_size = lhs_len + rhs.length();
        if(new_size)
        {
            ret.resize(new_size);
            if(lhs_len)
            {
                detail::internal_memcpy(ret.base_type::m_data, lhs, lhs_len);
            }
            if(rhs.length())
            {
                detail::internal_memcpy(ret.base_type::m_data + lhs_len, rhs.data(), rhs.length());
            }
        }
        return ret;
    }

    /// Stream output operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const string& rhs)
    {
        return lhs << rhs.c_str();
    }
#endif
};

#if defined(USE_LD)
namespace detail
{

/// Integer to maximum character count -transformation.
/// \return Maximum integer character count.
template<unsigned> constexpr unsigned int_character_width();
/// \cond
template<> constexpr unsigned int_character_width<1>()
{
    return 4;
}
template<> constexpr unsigned int_character_width<2>()
{
    return 6;
}
template<> constexpr unsigned int_character_width<4>()
{
    return 11;
}
template<> constexpr unsigned int_character_width<8>()
{
    return 20;
}
/// \endcond

/// Unsigned integer to maximum character count -transformation.
/// \return Maximum unsigned integer character count.
template<unsigned> constexpr unsigned unsigned_character_width();
/// \cond
template<> constexpr unsigned unsigned_character_width<1>()
{
    return 3;
}
template<> constexpr unsigned unsigned_character_width<2>()
{
    return 5;
}
template<> constexpr unsigned unsigned_character_width<4>()
{
    return 10;
}
template<> constexpr unsigned unsigned_character_width<8>()
{
    return 20;
}
/// \endcond

/// Converts an integer to a string.
/// \param op Integer to convert.
/// \return String representation.
template<typename T> string int_to_string(T op)
{
    if(op == 0)
    {
        return string("0");
    }

    const unsigned charw = int_character_width<sizeof(T)>();
    char data[charw + 1];

    unsigned iter = charw;
    data[charw] = 0;
    if(op < 0)
    {
        do {
            --iter;
            data[iter] = static_cast<char>('0' + (-(op % 10)));
            op /= 10;
        } while(op < 0);
        --iter;
        data[iter] = '-';
    }
    else
    {
        do {
            --iter;
            data[iter] = static_cast<char>('0' + (op % 10));
            op /= 10;
        } while(op > 0);
    }
    return string(data + iter);
}

/// Converts an unsigned integer to a string.
/// \param op Unsigned integer to convert.
/// \return String representation.
template<typename T> string unsigned_to_string(T op)
{
    if(op == 0)
    {
        return string("0");
    }

    const unsigned charw = unsigned_character_width<sizeof(T)>();
    char data[charw + 1];

    unsigned iter = charw;
    data[charw] = 0;
    do {
        --iter;
        data[iter] = static_cast<char>('0' + (op % 10));
        op /= 10;
    } while(op > 0);
    return string(data + iter);
}

}

/// String conversion from integer.
///
/// \param op Value.
/// \return String representation.
inline string to_string(int8_t op)
{
    return detail::int_to_string(op);
}

/// String conversion from unsigned integer.
///
/// \param op Value.
/// \return String representation.
inline string to_string(uint8_t op)
{
    return detail::unsigned_to_string(op);
}

/// String conversion from integer.
///
/// \param op Value.
/// \return String representation.
inline string to_string(int16_t op)
{
    return detail::int_to_string(op);
}

/// String conversion from unsigned integer.
///
/// \param op Value.
/// \return String representation.
inline string to_string(uint16_t op)
{
    return detail::unsigned_to_string(op);
}

/// String conversion from integer.
///
/// \param op Value.
/// \return String representation.
inline string to_string(int32_t op)
{
    return detail::int_to_string(op);
}

/// String conversion from unsigned integer.
///
/// \param op Value.
/// \return String representation.
inline string to_string(uint32_t op)
{
    return detail::unsigned_to_string(op);
}

/// String conversion from integer.
///
/// \param op Value.
/// \return String representation.
inline string to_string(int64_t op)
{
    return detail::int_to_string(op);
}

/// String conversion from unsigned integer.
///
/// \param op Value.
/// \return String representation.
inline string to_string(uint64_t op)
{
    return detail::unsigned_to_string(op);
}
#endif

}

#endif
