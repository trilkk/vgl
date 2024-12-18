#ifndef VGL_STRING_HPP
#define VGL_STRING_HPP

#include "vgl_assert.hpp"
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
    /// Value type.
    using value_type = T;
    /// Iterator type.
    using iterator = value_type*;
    /// Const iterator type.
    using const_iterator = const value_type*;
    /// Type of this class.
    using this_type = string_data<value_type>;

public:
    /// No-position.
    static const unsigned npos = numeric_limits<unsigned>::max();

protected:
    /// Internal data.
    value_type* m_data = nullptr;

    /// String length.
    unsigned m_length = 0;

protected:
    /// Default constructor.
    constexpr explicit string_data() noexcept = default;

    /// Constructor.
    ///
    /// \param data Data input.
    /// \param length Length.
    constexpr explicit string_data(value_type* data, unsigned length) noexcept :
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
    constexpr iterator begin() noexcept
    {
        return m_data;
    }
    /// Const iterator to the beginning.
    ///
    /// \return Const iterator.
    constexpr const_iterator begin() const noexcept
    {
        return m_data;
    }

    /// Iterator to the end.
    ///
    /// \return Iterator.
    constexpr iterator end() noexcept
    {
        return m_data + m_length;
    }
    /// Const iterator to the end.
    ///
    /// \return Const iterator.
    constexpr const_iterator end() const noexcept
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
    constexpr const value_type* data() const noexcept
    {
        if(m_data)
        {
            return m_data;
        }
        return reinterpret_cast<const T*>("");
    }

#if defined(USE_LD)
    /// Finds the occurrance of another substring.
    ///
    /// \param rhs Right-hand side operand.
    /// \param pos Position to start search from.
    /// \return Found index or npos if not found.
    constexpr unsigned find(const this_type& rhs, unsigned pos = 0) const noexcept
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

    /// Tells if the string starts with given substring.
    ///
    /// \param op Substring to test.
    /// \return True if starts with given substring, false otherwise.
    constexpr bool starts_with(const value_type* op)
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
    constexpr bool matchAtPosition(const this_type& rhs, unsigned pos) const noexcept
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
#endif

public:
    /// Access operator.
    ///
    /// \return Element reference.
    constexpr value_type& operator[](unsigned idx)
    {
        accessCheck(idx);
        return m_data[idx];
    }
    /// Const access operator.
    ///
    /// \return Element reference.
    constexpr const value_type& operator[](unsigned idx) const
    {
        accessCheck(idx);
        return m_data[idx];
    }
    /// Access operator.
    ///
    /// \return Element reference.
    constexpr value_type& operator[](int idx)
    {
        accessCheck(idx);
        return m_data[idx];
    }
    /// Const access operator.
    ///
    /// \return Element reference.
    constexpr const value_type& operator[](int idx) const
    {
        accessCheck(idx);
        return m_data[idx];
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

#if defined(USE_LD)
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
#endif

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
    friend std::ostream& operator<<(std::ostream& lhs, const this_type& rhs)
    {
        for(const T& cc : rhs)
        {
            lhs << cc;
        }
        return lhs;
    }
#endif

public:
    VGL_ITERATOR_FUNCTIONS(this_type)
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
    explicit string(const string_data<const value_type>& op)
    {
        assign(op);
    }

#if defined(USE_LD)
    /// Constructor from std::string.
    ///
    /// Intentionally not explicit.
    ///
    /// \param op String data input.
    string(const std::string& op) :
        base_type()
    {
        assign(op.data(), static_cast<unsigned>(op.length()));
    }

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
        VGL_ASSERT(data || (len == 0));
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
            base_type::m_data[len] = static_cast<value_type>(0);
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
    string& assign(const string_data<const value_type>& op)
    {
        return assign(op.data(), op.length());
    }
    /// Assign data.
    ///
    /// \param op Input data.
    /// \return This object.
    string& assign(const base_type::this_type& op)
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
        base_type::m_data[new_size] = static_cast<value_type>(0);
        base_type::m_length = new_size;
    }
    /// Resizes the string.
    ///
    /// New characters are initialized to T().
    ///
    /// \param new_size New size.
    /// \param value Value for new elements.
    void resize(unsigned new_size, value_type value)
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
    /// Insert into the string.
    ///
    /// \param pos Insertion position.
    /// \param start_iter Start iterator to insert.
    /// \param end_iter End iterator to insert.
    template<class IteratorType> void insert(const_iterator pos, IteratorType start_iter, IteratorType end_iter)
    {
        unsigned new_length = length() + static_cast<unsigned>(end_iter - start_iter);
        value_type* new_data = array_new(static_cast<value_type*>(nullptr), new_length + 1);
        value_type* insertion_iter = new_data;

        for(const_iterator ii = cbegin(); (ii < pos); ++ii)
        {
            *insertion_iter = *ii;
            ++insertion_iter;
        }
        for(IteratorType ii = start_iter; (ii < end_iter); ++ii)
        {
            *insertion_iter = static_cast<value_type>(*ii);
            ++insertion_iter;
        }
        for(const_iterator ii = pos, ee = cend(); (ii < ee); ++ii)
        {
            *insertion_iter = *ii;
            ++insertion_iter;
        }
        *insertion_iter = static_cast<value_type>(0);

        array_delete(base_type::m_data);
        base_type::m_data = new_data;
        base_type::m_length = new_length;
    }

    /// Returns a substring of this string.
    ///
    /// \param pos Position to start from.
    /// \param count Number of entries or \e npos to copy until end of string.
    /// \return Copy of a part of this string.
    string substr(unsigned pos, unsigned count = npos)
    {
        VGL_ASSERT(pos < length());
        if (count == npos)
        {
            return string(base_type::m_data + pos, length() - pos);
        }
        VGL_ASSERT((pos + count) <= length());
        return string(base_type::m_data + pos, count);
    }

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
    template<typename T> string& operator=(const detail::string_data<T>& rhs)
    {
        return assign(rhs.data(), rhs.length());
    }

#if defined(USE_LD)
    /// Addition operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Concatenated string.
    template<typename T> string operator+(const detail::string_data<T>& rhs) const
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
    template<typename T> string& operator+=(const detail::string_data<T>& rhs)
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
        const char ZERO[] =
        {
            '0',
        };
        return string(ZERO, 1);
    }

    const unsigned CHARW = int_character_width<sizeof(T)>();
    char data[CHARW];

    unsigned iter = CHARW;
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
    return string(data + iter, CHARW - iter);
}

/// Converts an unsigned integer to a string.
///
/// \param op Unsigned integer to convert.
/// \return String representation.
template<typename T> string unsigned_to_string(T op)
{
    if(op == 0)
    {
        return string("0");
    }

    const unsigned CHARW = unsigned_character_width<sizeof(T)>();
    char data[CHARW];

    unsigned iter = CHARW;
    do {
        --iter;
        data[iter] = static_cast<char>('0' + (op % 10));
        op /= 10;
    } while(op > 0);
    return string(data + iter, CHARW - iter);
}

/// Converts a pointer number to a string.
///
/// \param op Pointer to convert.
/// \return String representation.
template<typename T> string pointer_to_string(T op)
{
    auto intToHexChar = [](size_t val) -> char
    {
        if(val < 10)
        {
            return static_cast<char>('0' + val);
        }
        return static_cast<char>('A' + (val - 10));
    };

    const unsigned CHARW = static_cast<unsigned>(sizeof(T) * 2);
    char data[CHARW + 2];

    data[0] = '0';
    data[1] = 'x';

    unsigned iter = CHARW + 2;
    auto val = static_cast<size_t>(op);
    do {
        --iter;
        data[iter] = intToHexChar(val % 16);
        val /= 16;
    } while(iter > 2);
    return string(data, CHARW + 2);
}

/// Converts a floating-point number to a string.
///
/// Intentionally uses the standard library.
///
/// \param op Floating-point number to convert.
/// \return String representation.
template<typename T> string float_to_string(T op)
{
    return string(std::to_string(op));
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

/// String conversion from pointer.
///
/// \param op Value.
/// \return String representation.
inline string to_string(void* op)
{
    return detail::pointer_to_string(reinterpret_cast<size_t>(op));
}

/// String conversion from floating-point number.
///
/// \param op Value.
/// \return String representation.
inline string to_string(float op)
{
    return detail::float_to_string(op);
}

/// String conversion from floating-point number.
///
/// \param op Value.
/// \return String representation.
inline string to_string(double op)
{
    return detail::float_to_string(op);
}

/// String conversion from floating-point number.
///
/// \param op Value.
/// \return String representation.
inline string to_string(long double op)
{
    return detail::float_to_string(op);
}
#endif

}

#endif
