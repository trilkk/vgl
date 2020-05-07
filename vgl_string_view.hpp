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
    /// Intentionally not explicit.
    ///
    /// \param op C string input.
    constexpr string_view(const string_data<char>& op) noexcept :
        base_type(op.data(), op.length())
    {
    }

    /// Constructor.
    ///
    /// \param op C string input.
    constexpr string_view(const string_view& op) noexcept :
        base_type(op.data(), op.length())
    {
    }

    /// Move constructor.
    ///
    /// \param op C string input.
    constexpr string_view(string_view&& op) noexcept :
        base_type(op.m_data, op.m_length)
    {
    }

public:
    /// Assignment operator.
    ///
    /// \param op Input data.
    /// \return This object.
    string_view& operator=(const string_view& op) noexcept
    {
        base_type::m_data = op.m_data;
        base_type::m_length = op.m_length;
        return *this;
    }

    /// Move operator.
    ///
    /// \param op Source object.
    /// \return This object.
    string_view& operator=(string_view&& op) noexcept
    {
        base_type::m_data = op.m_data;
        base_type::m_length = op.m_length;
        return *this;
    }

public:
#if defined(USE_LD)
    /// Stream output operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const string_view& rhs)
    {
        return lhs << rhs.c_str();
    }
#endif
};

}

#endif
