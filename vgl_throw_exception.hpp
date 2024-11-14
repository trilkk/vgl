#ifndef VGL_THROW_EXCEPTION_HPP
#define VGL_THROW_EXCEPTION_HPP

#if defined(USE_LD)

#include "vgl_string.hpp"

#include <exception>

#include <boost/throw_exception.hpp>

namespace vgl
{

/// Wrapper for runtime_error.
///
/// Main intent is to support and be easily constructable using replacement classes. Eating own dog food good.
class runtime_error : public std::exception
{
private:
    /// Internal error string.
    string m_data = "runtime_error: ";

public:
    /// Constructor.
    ///
    /// \param op String input.
    explicit runtime_error(const string& op)
    {
        m_data += op;
    }

    /// Constructor.
    ///
    /// \param op String input.
    explicit runtime_error(const char* op)
    {
        m_data += op;
    }

    /// Copy constructor.
    ///
    /// \param other Source object.
    runtime_error(const runtime_error& other) :
        m_data(other.m_data)
    {
    }

    /// Move constructor.
    ///
    /// \param other Source object.
    runtime_error(runtime_error&& other) :
        m_data(move(other.m_data))
    {
    }

public:

    /// \cond
    const char* what() const noexcept override
    {
        return m_data.c_str();
    }
    /// \endcond

public:
    /// Copy operator.
    ///
    /// \param other Source object.
    /// \return This object.
    runtime_error& operator=(const runtime_error& other)
    {
        m_data = other.m_data;
        return *this;
    }

    /// Move operator.
    ///
    /// \param other Source object.
    /// \return This object.
    runtime_error& operator=(runtime_error&& other)
    {
        m_data = move(other.m_data);
        return *this;
    }
};

}

/// Wrapper for BOOST_THROW_EXCEPTION macro.
///
/// \param op Exception to throw.
#define VGL_THROW_EXCEPTION(op) BOOST_THROW_EXCEPTION(op)

/// Constructs and throws a runtime_error.
///
/// \param op String input.
#define VGL_THROW_RUNTIME_ERROR(op) VGL_THROW_EXCEPTION(vgl::runtime_error(op))

#endif

#endif
