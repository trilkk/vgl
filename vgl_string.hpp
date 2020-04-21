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
            std::ostringstream sstr;
            sstr << "accessing index " << idx << " from string of length " << m_length;
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
            sstr << "accessing negative index " << idx << " from string of length " << m_length;
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
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

/// Internal comparison operation for string data.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Left-hand-side operand.
template<typename L, typename R> bool internal_string_data_equals(const string_data<L>& lhs, const string_data<R>& rhs)
{
    if(lhs.length() != rhs.length())
    {
        return false;
    }

    typename string_data<L>::const_iterator ii = lhs.cbegin();
    const typename string_data<L>::const_iterator ee = lhs.cbegin();
    typename string_data<R>::const_iterator jj = rhs.cbegin();
    while(ii != ee)
    {
        if(*ii != *jj)
        {
            return false;
        }
        ++ii;
        ++jj;
    }

    return true;
}

/// Templated string replacement.
template<typename T> class basic_string : public string_data<T>
{
private:
    /// Base class type.
    using base_type = string_data<T>;

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

    /// Constructor.
    ///
    /// \param op String data input.
    explicit basic_string(const string_data<const T>& op)
    {
        assign(op);
    }

    /// Copy constructor.
    ///
    /// \param op String data input.
    basic_string(const basic_string<T>& op)
    {
        assign(op);
    }

    /// Move constructor.
    ///
    /// \param op C string input.
    basic_string(basic_string<T>&& op) :
        base_type(op.m_data, op.m_length)
    {
        op.m_data = nullptr;
        op.m_length = 0;
    }

    /// Destructor.
    ~basic_string()
    {
        array_delete(base_type::m_data);
    }

public:
    /// Assign data.
    ///
    /// \param op Input data.
    /// \return This object.
    basic_string& assign(const T* op)
    {
        clear();
        if(op && *op)
        {
            base_type::m_length = internal_strlen(op);
            unsigned copy_length = base_type::m_length + 1;
            base_type::m_data = array_new(base_type::m_data, copy_length);
            internal_memcpy(base_type::m_data, op, copy_length);
        }
        return *this;
    }
    /// Assign data.
    ///
    /// \param op Input data.
    /// \return This object.
    basic_string& assign(const string_data<const T>& op)
    {
        clear();
        if(!op.empty())
        {
            base_type::m_length = op.length();
            unsigned copy_length = base_type::m_length + 1;
            base_type::m_data = array_new(base_type::m_data, copy_length);
            internal_memcpy(base_type::m_data, op.data(), copy_length);
        }
        return *this;
    }
    /// Assign data.
    ///
    /// \param op Input data.
    /// \return This object.
    basic_string& assign(const string_data<T>& op)
    {
        return assign(*reinterpret_cast<const string_data<const T>*>(&op));
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
        base_type::m_data[new_size] = static_cast<T>(0);
        base_type::m_length = new_size;
    }
    /// Resizes the string.
    ///
    /// New characters are initialized to T().
    ///
    /// \param new_size New size.
    /// \param value Value for new elements.
    void resize(unsigned new_size, const T& value)
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
        base_type::m_data = op.m_data;
        base_type::m_length = op.m_length;
        op.m_data = nullptr;
        op.m_length = 0;
    }
};

}

/// String type definition.
using string = detail::basic_string<char>;

}

#endif
