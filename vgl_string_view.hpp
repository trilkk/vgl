#ifndef VGL_STRING_VIEW_HPP
#define VGL_STRING_VIEW_HPP

#include "vgl_string.hpp"

namespace vgl
{

namespace detail
{

/// Templated string_view replacement.
template<typename T> class basic_string_view : public string_data<const T>
{
private:
    /// Base class type.
    using base_type = string_data<const T>;

public:
    /// Constructor.
    constexpr explicit basic_string_view() noexcept = default;

    /// Constructor.
    ///
    /// Intentionally not explicit.
    ///
    /// \param op C string input.
    constexpr basic_string_view(const T* op) :
        base_type(op, internal_strlen(op))
    {
    }

    /// Constructor.
    ///
    /// Intentionally not explicit.
    ///
    /// \param op C string input.
    constexpr basic_string_view(const string_data<T>& op) noexcept :
        base_type(op.data(), op.length())
    {
    }

    /// Constructor.
    ///
    /// \param op C string input.
    constexpr basic_string_view(const basic_string_view<T>& op) noexcept :
        base_type(op.data(), op.length())
    {
    }

    /// Move constructor.
    ///
    /// \param op C string input.
    constexpr basic_string_view(basic_string_view<T>&& op) noexcept :
        base_type(op.m_data, op.m_length)
    {
    }

public:
    /// Assignment operator.
    ///
    /// \param op Input data.
    /// \return This object.
    basic_string_view& operator=(const basic_string_view<T>& op) noexcept
    {
        base_type::m_data = op.m_data;
        base_type::m_length = op.m_length;
        return *this;
    }

    /// Move operator.
    ///
    /// \param op Source object.
    /// \return This object.
    basic_string_view& operator=(basic_string_view<T>&& op) noexcept
    {
        base_type::m_data = op.m_data;
        base_type::m_length = op.m_length;
        return *this;
    }

    /// Equals operator.
    ///
    /// \param op Other string data.
    /// \return True if equal, false otherwise.
    bool operator==(const basic_string_view<T>& op) const noexcept
    {
        return internal_string_data_equals(*this, op);
    }

    /// Not equals operator.
    ///
    /// \param op Other string data.
    /// \return True if not equal, false otherwise.
    bool operator!=(const basic_string_view<T>& op) const noexcept
    {
        return !(*this == op);
    }

public:
#if defined(USE_LD)
    /// Stream output operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const basic_string_view<T>&& rhs)
    {
        return lhs << rhs.c_str();
    }
#endif
};

}

/// Equals operator for string and string view.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return True if equal, false otherwise.
template<typename T> bool operator==(const detail::basic_string<T>& lhs, const detail::basic_string_view<T>& rhs)
{
    return detail::internal_string_data_equals(lhs, rhs);
}

/// Not equals operator for string and string view.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return True if not equal, false otherwise.
template<typename T> bool operator!=(const detail::basic_string<T>& lhs, const detail::basic_string_view<T>& rhs)
{
    return !(lhs == rhs);
}

/// String view type definition.
using string_view = detail::basic_string_view<char>;

}

#endif
