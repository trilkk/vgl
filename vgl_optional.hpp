#ifndef VGL_OPTIONAL_HPP
#define VGL_OPTIONAL_HPP

#include "vgl_type_traits.hpp"
#include "vgl_utility.hpp"

#include <cstddef>
#include <optional>

namespace vgl
{

using std::nullopt_t;
using std::nullopt;

namespace detail
{

/// Gets alignment suitable for an optional.
///
/// \return Alignment for an optional.
template<typename T> constexpr size_t optional_alignof()
{
    size_t min_align = alignof(uint32_t);
    size_t ret = alignof(T);
    return (ret < min_align) ? min_align : ret;
}

/// Internal data container for optional.
///
/// This class intentionally has no constructor or destructor.
///
/// Aligned to the alignment of the optional type to.
template<typename T> class alignas(optional_alignof<T>()) optional_internal_data
{
protected:
    /// Align to uint32_t at the very least.
    /// This removes an alignment warning on some platforms.
    static const size_t STORAGE_SIZE = (sizeof(T) / sizeof(uint32_t)) + ((sizeof(T) % sizeof(uint32_t)) ? 1 : 0);

protected:
    /// Internal data storage.
    uint32_t m_data[STORAGE_SIZE];

    /// Flag for initialization.
    bool m_initialized;

protected:
    /// Accessor.
    ///
    /// \return Pointer to internal data.
    constexpr T* getPtr() noexcept
    {
        return reinterpret_cast<T*>(m_data);
    }
    /// Accessor.
    ///
    /// \return Pointer to internal data.
    constexpr const T* getPtr() const noexcept
    {
        return reinterpret_cast<const T*>(m_data);
    }
};

/// Base data class for optional.
///
/// Optional inherits either the trivially destructible or not trivially destructible optional depending on whether
/// the templated type is trivially destructible or not.
template<typename T, bool> class optional_data : public optional_internal_data<T>
{
protected:
    /// Base type.
    using base_type = optional_internal_data<T>;

protected:
    /// Default constructor.
    ///
    /// \param op Initial value.
    constexpr explicit optional_data(bool op) noexcept
    {
        base_type::m_initialized = op;
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
        base_type::m_initialized = false;
    }
};

/// Specialization for nontrivially destructible optional data.
template<typename T> class optional_data<T, false> : public optional_internal_data<T>
{
protected:
    /// Base type.
    using base_type = optional_internal_data<T>;

protected:
    /// Default constructor.
    ///
    /// \param op Initial value.
    constexpr explicit optional_data(bool op) noexcept
    {
        base_type::m_initialized = op;
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
        if(base_type::m_initialized)
        {
            base_type::getPtr()->~T();
        }
    }

    /// Nontrivial destruct and flag uninitialized.
    constexpr void reset()
    {
        if(base_type::m_initialized)
        {
            base_type::getPtr()->~T();
            base_type::m_initialized = false;
        }
    }
};

}

/// Optional value.
///
/// Replacement for std::optional.
template<typename T> class optional : public detail::optional_data<T, is_trivially_destructible<T>::value>
{
private:
    /// Base class type.
    using parent_type = detail::optional_data<T, is_trivially_destructible<T>::value>;

    /// Internal data type.
    using base_type = detail::optional_internal_data<T>;

public:
    /// Constructor.
    constexpr explicit optional() noexcept :
        parent_type(false)
    {
    }

    /// Constructor with copied value.
    ///
    /// \param op Value.
    constexpr optional(const T& op) :
        parent_type(true)
    {
        new(base_type::getPtr()) T(op);
    }

    /// Constructor with moved value.
    ///
    /// \param op Value.
    constexpr optional(T&& op) :
        parent_type(true)
    {
        new(base_type::getPtr()) T(move(op));
    }

    /// Copy constructor.
    ///
    /// \param op Value.
    constexpr optional(const optional<T>& op) :
        parent_type(false)
    {
        if(op)
        {
            new(base_type::getPtr()) T(*op);
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
            new(base_type::getPtr()) T(move(*op));
            op.reset();
            base_type::m_initialized = true;
        }
    }

    /// Initializer with nullopt.
    constexpr optional(const nullopt_t&) noexcept :
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
        parent_type::reset();
    }

    /// Get the contained object.
    ///
    /// \return Contained object.
    constexpr T& value()
    {
        accessCheck();
        return *base_type::getPtr();
    }
    /// Get the contained object (const version).
    ///
    /// \return Contained object.
    constexpr const T& value() const
    {
        accessCheck();
        return *base_type::getPtr();
    }

    /// Constructs a member in-place.
    ///
    /// \param args Arguments.
    template<typename...Args> T& emplace(Args&&...args)
    {
        parent_type::destruct();
        new(base_type::getPtr()) T(args...);
        base_type::m_initialized = true;
        return *base_type::getPtr();
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
        return base_type::getPtr();
    }
    /// Member access operator.
    ///
    /// \return Contained object.
    constexpr const T* operator->() const
    {
        accessCheck();
        return base_type::getPtr();
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
        parent_type::destruct();
        new(base_type::getPtr()) T(op);
        base_type::m_initialized = true;
        return *this;
    }
    /// Move operator.
    ///
    /// \param op Assigned object.
    constexpr optional<T>& operator=(T&& op)
    {
        parent_type::destruct();
        new(base_type::getPtr()) T(move(op));
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
            new(base_type::getPtr()) T(*op);
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
            new(base_type::getPtr()) T(move(*op));
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
