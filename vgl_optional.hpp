#ifndef VGL_OPTIONAL_HPP
#define VGL_OPTIONAL_HPP

#include <cstddef>
#include <optional>
#include <type_traits>

namespace vgl
{

using std::nullopt;

namespace detail
{

/// Base data class for optional.
///
/// Inherited by trivially or nontrivially destructible optional.
template<typename T> class optional_data
{
protected:
    /// Container for optional data.
    alignas(alignof(T)) uint8_t m_data[sizeof(T)];

    /// Flag for initialization.
    bool m_initialized;

protected:
    /// Default constructor.
    ///
    /// \param op Initial value.
    constexpr explicit optional_data(bool op) :
       m_initialized(op)
    {
    }
};

/// Destructible class for optional.
///
/// Optional inherits either the trivially destructible or not trivially destructible optional depending on whether
/// the templated type is trivially destructible or not.
template<typename T, bool> class optional_destructible : public optional_data<T>
{
protected:
    /// Default constructor.
    ///
    /// \param op Initial value.
    constexpr explicit optional_destructible(bool op) :
        optional_data<T>(op)
    {
    }

public:
    /// Trivial destruction function.
    constexpr void destruct()
    {
    }
};

/// Specialization for non-trivially destructible optional values.
template<typename T> class optional_destructible<T, false> : public optional_data<T>
{
protected:
    /// Default constructor.
    ///
    /// \param op Initial value.
    constexpr explicit optional_destructible(bool op) :
        optional_data<T>(op)
    {
    }

    /// Destructor.
    ~optional_destructible()
    {
        destruct();
    }

public:
    /// Nontrivial destruction function.
    constexpr void destruct()
    {
        if(optional_data<T>::m_initialized)
        {
            (*reinterpret_cast<T*>(optional_data<T>::m_data)).~T();
        }
    }
};

}

/// Optional value.
///
/// Replacement for std::optional.
template<typename T> class optional : detail::optional_destructible<T, std::is_trivially_destructible<T>::value>
{
private:
    /// Base class type.
    using base_type = detail::optional_data<T>;

    /// Parent class type.
    using parent_type = detail::optional_destructible<T, std::is_trivially_destructible<T>::value>;

public:
    /// Constructor.
    constexpr explicit optional() :
        parent_type(false)
    {
    }

    /// Constructor with copied value.
    ///
    /// \param op Value.
    constexpr optional(const T& op) :
        parent_type(true)
    {
        new(getPtr()) T(op);
    }

    /// Constructor with moved value.
    ///
    /// \param op Value.
    constexpr optional(T&& op) :
        parent_type(true)
    {
        new(getPtr()) T(std::move(op));
    }

    /// Copy constructor.
    ///
    /// \param op Value.
    constexpr optional(const optional<T>& op) :
        parent_type(false)
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
        parent_type(false)
    {
        if(op)
        {
            new(getPtr()) T(std::move(*op));
            op.destruct();
            base_type::m_initialized = true;
        }
    }

    /// Initializer with nullopt.
    constexpr optional(const std::nullopt_t&) :
        parent_type(false)
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
        return reinterpret_cast<T*>(base_type::m_data);
    }
    /// Get the pointer value top internal data (const version).
    ///
    /// \return Pointer to internal data.
    constexpr const T* getPtr() const
    {
        return reinterpret_cast<const T*>(base_type::m_data);
    }

public:
    /// Tell if this optional value is initialized.
    ///
    /// \return True if yes, false if no.
    constexpr bool has_value() const
    {
        return base_type::m_initialized;
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
    constexpr operator bool() const
    {
        return has_value();
    }

    /// Equals operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Truth value.
    constexpr bool operator==(const optional<T>& rhs) const
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
    constexpr bool operator!=(const optional<T>& rhs) const
    {
        return !(*this == rhs);
    }

    /// Assignment operator.
    ///
    /// \param op Assigned object.
    constexpr optional<T>& operator=(const T& op)
    {
        parent_type::destruct();
        new(getPtr()) T(op);
        base_type::m_initialized = true;
        return *this;
    }
    /// Move operator.
    ///
    /// \param op Assigned object.
    constexpr optional<T>& operator=(T&& op)
    {
        parent_type::destruct();
        new(getPtr()) T(std::move(op));
        base_type::m_initialized = true;
        return *this;
    }

    /// Assignment operator.
    ///
    /// \param op Assigned object.
    constexpr optional<T>& operator=(const optional<T>& op)
    {
        parent_type::destruct();
        base_type::m_initialized = false;
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
        base_type::destruct();
        base_type::m_initialized = false;
        if(op)
        {
            new(getPtr()) T(std::move(*op));
            base_type::m_initialized = true;
            op.destruct();
            op.base_type::m_initialized = false;
        }
        return *this;
    }

    /// Assignment from nullopt.
    ///
    /// \return The optional after clearing it.
    constexpr optional<T>& operator=(const std::nullopt_t&)
    {
        base_type::destruct();
        base_type::m_initialized = false;
        return *this;
    }
};

}

#endif
