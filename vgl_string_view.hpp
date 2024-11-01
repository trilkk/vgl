#ifndef VGL_STRING_VIEW_HPP
#define VGL_STRING_VIEW_HPP

#include "vgl_string.hpp"

namespace vgl
{

/// Templated string_view replacement.
class string_view : public detail::string_data<const char>
{
private:
    /// Base class type.
    using base_type = detail::string_data<const char>;

public:
    /// Constructor.
    constexpr explicit string_view() noexcept = default;

    /// Constructor.
    ///
    /// Intentionally not explicit.
    ///
    /// \param op C string input.
    constexpr string_view(const char* op) :
        base_type(op, detail::internal_strlen(op))
    {
    }

    /// Constructor.
    ///
    /// \param ptr C string input.
    /// \param len Length of input.
    constexpr explicit string_view(const char* ptr, unsigned len) :
        base_type(ptr, len)
    {
    }

    /// Constructor.
    ///
    /// Intentionally not explicit.
    ///
    /// \param op C string input.
    constexpr string_view(const string& op) noexcept :
        base_type(op.data(), op.length())
    {
    }

    /// Copy constructor.
    ///
    /// \param other Source object.
    constexpr string_view(const string_view& other) noexcept :
        base_type(other.data(), other.length())
    {
    }

    /// Move constructor.
    ///
    /// \param other Source object.
    constexpr string_view(string_view&& other) noexcept :
        base_type(other.m_data, other.m_length)
    {
    }

public:
    /// Assignment operator.
    ///
    /// \param op Input data.
    /// \return This object.
    constexpr string_view& operator=(const string_view& op) noexcept
    {
        base_type::m_data = op.m_data;
        base_type::m_length = op.m_length;
        return *this;
    }

    /// Move operator.
    ///
    /// \param op Source object.
    /// \return This object.
    constexpr string_view& operator=(string_view&& op) noexcept
    {
        base_type::m_data = op.m_data;
        base_type::m_length = op.m_length;
        return *this;
    }
};

}

#endif
