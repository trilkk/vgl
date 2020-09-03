#ifndef VGL_OPTIONAL_HPP
#define VGL_OPTIONAL_HPP

#include "vgl_utility.hpp"

#include <cstddef>
#include <optional>
#include <type_traits>

namespace vgl
{

using std::nullopt_t;
using std::nullopt;

namespace detail
{

/// Trivial union member.
class optional_trivial_union_member
{
public:
    /// Constructor.
    constexpr explicit optional_trivial_union_member() noexcept
    {
    }

};

/// Container union for optional specialization that ensures correct alingment.
template<typename T> union optional_union
{
    /// Type data (not trivially destructible).
    T m_type_data;

    /// Trivially destructible alternative.
    optional_trivial_union_member m_trivial_data;


    /// Constructor.
    constexpr explicit optional_union() noexcept :
        m_trivial_data()
    {
    }
};

/// Base data class for optional.
///
/// Optional inherits either the trivially destructible or not trivially destructible optional depending on whether
/// the templated type is trivially destructible or not.
template<typename T, bool> class optional_data
{
protected:
    /// Trivially destructible data.
    optional_union<T> m_data;

    /// Flag for initialization.
    bool m_initialized;

protected:
    /// Default constructor.
    ///
    /// \param op Initial value.
    constexpr explicit optional_data(bool op) noexcept :
       m_initialized(op)
    {
    }

protected:
    /// Trivial destruct.
    constexpr void destruct()
    {
    }

    /// Trivial destruct and flag unused.
    constexpr void reset()
    {
        destruct();
        m_initialized = false;
    }
};

/// Specialization for nontrivially destructible optional data.
template<typename T> class optional_data<T, false>
{
protected:
    /// Nontrivially destructible data.
    optional_union<T> m_data;

    /// Flag for initialization.
    bool m_initialized;

protected:
    /// Default constructor.
    ///
    /// \param op Initial value.
    constexpr explicit optional_data(bool op) noexcept :
        m_initialized(op)
    {
    }

    /// Destructor.
    ~optional_data()
    {
        destruct();
    }

protected:
    /// Nontrivial destructi.
    constexpr void destruct()
    {
        if(m_initialized)
        {
            (*(&(m_data.m_type_data))).~T();
        }
    }

    /// Nontrivial destruct and flag uninitialized.
    constexpr void reset()
    {
        if(m_initialized)
        {
            (*(&(m_data.m_type_data))).~T();
            m_initialized = false;
        }
    }
};

}

/// Optional value.
///
/// Replacement for std::optional.
template<typename T> class optional : public detail::optional_data<T, std::is_trivially_destructible<T>::value>
{
private:
    /// Base class type.
    using base_type = detail::optional_data<T, std::is_trivially_destructible<T>::value>;

public:
    /// Constructor.
    constexpr explicit optional() noexcept :
        base_type(false)
    {
    }

    /// Constructor with copied value.
    ///
    /// \param op Value.
    constexpr optional(const T& op) :
        base_type(true)
    {
        new(getPtr()) T(op);
    }

    /// Constructor with moved value.
    ///
    /// \param op Value.
    constexpr optional(T&& op) :
        base_type(true)
    {
        new(getPtr()) T(move(op));
    }

    /// Copy constructor.
    ///
    /// \param op Value.
    constexpr optional(const optional<T>& op) :
        base_type(false)
    {
        if(op)
        {
            new(getPtr()) T(*op);
            base_type::m_initialized = true;
        }
    }

    /// Move constructor.
    ///
    /// \param op Value.
    constexpr optional(optional<T>&& op) :
        base_type(false)
    {
        if(op)
        {
            new(getPtr()) T(move(*op));
            op.reset();
            base_type::m_initialized = true;
        }
    }

    /// Initializer with nullopt.
    constexpr optional(const nullopt_t&) noexcept :
        base_type(false)
    {
    }

private:
    /// Perform access check.
    ///
    /// Throws an error if the optional value is not initialized.
    constexpr void accessCheck() const
    {
#if defined(USE_LD) && defined(DEBUG)
        if(!has_value())
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("optional value is uninitialized"));
        }
#endif
    }

    /// Get the pointer value top internal data.
    ///
    /// \return Pointer to internal data.
    constexpr T* getPtr()
    {
        return &(base_type::m_data.m_type_data);
    }
    /// Get the pointer value top internal data (const version).
    ///
    /// \return Pointer to internal data.
    constexpr const T* getPtr() const
    {
        return &(base_type::m_data.m_type_data);
    }

public:
    /// Tell if this optional value is initialized.
    ///
    /// \return True if yes, false if no.
    constexpr bool has_value() const
    {
        return base_type::m_initialized;
    }

    /// Destroys the contents of the optional.
    void reset()
    {
        base_type::reset();
    }

    /// Get the contained object.
    ///
    /// \return Contained object.
    constexpr T& value()
    {
        accessCheck();
        return *getPtr();
    }
    /// Get the contained object (const version).
    ///
    /// \return Contained object.
    constexpr const T& value() const
    {
        accessCheck();
        return *getPtr();
    }

public:
    /// Dereference operator.
    ///
    /// \return Contained object.
    constexpr T& operator*()
    {
        accessCheck();
        return value();
    }
    /// Dereference operator (const version).
    ///
    /// \return Contained object.
    constexpr const T& operator*() const
    {
        accessCheck();
        return value();
    }

    /// Member access operator.
    ///
    /// \return Contained object.
    constexpr T* operator->()
    {
        accessCheck();
        return getPtr();
    }
    /// Member access operator.
    ///
    /// \return Contained object.
    constexpr const T* operator->() const
    {
        accessCheck();
        return getPtr();
    }

    /// Boolean operator.
    ///
    /// \return True if this optional value is populated.
    constexpr operator bool() const noexcept
    {
        return has_value();
    }

    /// Equals operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Truth value.
    constexpr bool operator==(const optional<T>& rhs) const noexcept
    {
        if(has_value())
        {
            if(rhs)
            {
                return (value() == *rhs);
            }
            return false;
        }
        else if(rhs)
        {
            return false;
        }
        return true;
    }
    /// Not equals operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Truth value.
    constexpr bool operator!=(const optional<T>& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    /// Assignment operator.
    ///
    /// \param op Assigned object.
    constexpr optional<T>& operator=(const T& op)
    {
        base_type::destruct();
        new(getPtr()) T(op);
        base_type::m_initialized = true;
        return *this;
    }
    /// Move operator.
    ///
    /// \param op Assigned object.
    constexpr optional<T>& operator=(T&& op)
    {
        base_type::destruct();
        new(getPtr()) T(move(op));
        base_type::m_initialized = true;
        return *this;
    }

    /// Assignment operator.
    ///
    /// \param op Assigned object.
    constexpr optional<T>& operator=(const optional<T>& op)
    {
        reset();
        if(op)
        {
            new(getPtr()) T(*op);
            base_type::m_initialized = true;
        }
        return *this;
    }
    /// Move operator.
    ///
    /// Moves complete optional.
    ///
    /// \param op Assigned object.
    constexpr optional<T>& operator=(optional<T>&& op)
    {
        reset();
        if(op)
        {
            new(getPtr()) T(move(*op));
            op.reset();
            base_type::m_initialized = true;
        }
        return *this;
    }

    /// Assignment from nullopt.
    ///
    /// \return The optional after clearing it.
    constexpr optional<T>& operator=(const nullopt_t&)
    {
        reset();
        return *this;
    }
};

}

#endif
