#ifndef VGL_STRING_HPP
#define VGL_STRING_HPP

#include "vgl_realloc.hpp"

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

protected:
    /// Internal data.
    T* m_data = nullptr;

    /// String length.
    unsigned m_length = 0;

protected:
    /// Default constructor.
    explicit constexpr string_data() noexcept = default;

    /// Constructor.
    ///
    /// \param data Data input.
    /// \param length Length.
    explicit constexpr string_data(T* data, unsigned length) noexcept :
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

    /// Returns the internal data pointer.
    ///
    /// Unlike with std::string, this is guaranteed to be null-terminated.
    ///
    /// \return Data pointer.
    constexpr const T* data() const
    {
        if(m_data)
        {
            return m_data;
        }
        return reinterpret_cast<const T*>("");
    }
    /// Return C string representing the string.
    ///
    /// \return C string pointer.
    constexpr const T* c_str() const
    {
        return data();
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
    constexpr bool operator!=(const string_data<T>& rhs) const noexcept
    {
        return !(*this == rhs);
    }

public:
#if defined(USE_LD)
    /// Stream output operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const string_data<T>& rhs)
    {
        return lhs << rhs.c_str();
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
    explicit constexpr string() = default;

    /// Constructor.
    ///
    /// \param op C string input.
    explicit string(const char* op)
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

    /// Copy constructor.
    ///
    /// \param op String data input.
    string(const string& op) :
        base_type()
    {
        assign(op);
    }

    /// Move constructor.
    ///
    /// \param op C string input.
    string(string&& op) :
        base_type(op.m_data, op.m_length)
    {
        op.m_data = nullptr;
        op.m_length = 0;
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
        clear();
        if(len > 0)
        {
            base_type::m_length = len;
            unsigned copy_length = len + 1;
            base_type::m_data = array_new(base_type::m_data, copy_length);
            detail::internal_memcpy(base_type::m_data, data, copy_length);
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
        return assign(reinterpret_cast<const char*>(op.data(), op.length()));
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
        unsigned old_size = base_type::m_length;

        resize(new_size);

        while(old_size < new_size)
        {
            base_type::m_data[old_size] = value;
            ++old_size;
        }
    }

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
    /// \param op Input data.
    /// \return This object.
    string& operator=(const string& op)
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

    /// Move operator.
    ///
    /// \param op Source object.
    /// \return This object.
    string& operator=(string&& op)
    {
        base_type::m_data = op.m_data;
        base_type::m_length = op.m_length;
        op.m_data = nullptr;
#if defined(USE_LD) && defined(DEBUG)
        op.m_length = 0;
#endif
        return *this;
    }
};

}

#endif
