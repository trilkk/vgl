#ifndef VGL_UNIQUE_PTR_HPP
#define VGL_UNIQUE_PTR_HPP

#include "vgl_cstddef.hpp"
#include "vgl_realloc.hpp"

namespace vgl
{

/// Replacement for std::unique_ptr.
template<typename T> class unique_ptr
{
private:
    /// Object reference.
    T* m_ref;

private:
    /// Deleted copy constructor.
    unique_ptr(const unique_ptr<T>&) = delete;
    /// Deleted assignment.
    unique_ptr<T>& operator=(const unique_ptr<T>&) = delete;

public:
    /// Empty constructor.
    constexpr explicit unique_ptr() :
        m_ref(nullptr)
    {
    }

    /// Constructor.
    ///
    /// \param op Pointer to take control of.
    constexpr explicit unique_ptr(T* op) :
        m_ref(op)
    {
    }

    /// Move constructor.
    ///
    /// Transfers ownership.
    ///
    /// \param op Source.
    constexpr unique_ptr(unique_ptr<T>&& op) noexcept :
        m_ref(op.release())
    {
    }

    /// Constructor from nullptr.
    unique_ptr(const nullptr_t&) :
        m_ref(nullptr)
    {
    }

    /// Destructor.
    ~unique_ptr()
    {
        erase();
    }

private:
    /// Delete contents if they exist.
    ///
    /// Will not actually modify the content pointer.
    constexpr void erase() const
    {
        delete m_ref;
    }

public:
    /// Accessor.
    ///
    /// \return Referenced pointer.
    constexpr T* get() const noexcept
    {
        return m_ref;
    }

    /// Release ownership, return released pointer.
    ///
    /// Should not be called by user.
    constexpr T* release() noexcept
    {
        T* ret = m_ref;
        m_ref = NULL;
        return ret;
    }

    /// Replace contents.
    ///
    /// \param op New contents.
    void reset(T* op = NULL)
    {
        erase();
        m_ref = op;
    }

public:
    /// Pointer operator.
    ///
    /// \return Pointer.
    constexpr T* operator->() const noexcept
    {
        return m_ref;
    }

    /// Dereference operator.
    ///
    /// \return Dereferenced contents.
    constexpr T& operator*() noexcept
    {
        return *m_ref;
    }
    /// Dereference operator.
    ///
    /// \return Dereferenced contents.
    constexpr const T& operator*() const noexcept
    {
        return *m_ref;
    }

    /// Assignment operator.
    ///
    /// Takes ownership.
    ///
    /// \param op Source.
    /// \return This object.
    unique_ptr<T>& operator=(T *op)
    {
        reset(op);
        return *this;
    }

    /// Assignment from nullptr.
    ///
    /// Destroys the object.
    ///
    /// \return This object.
    unique_ptr<T>& operator=(const nullptr_t&)
    {
        reset();
        return *this;
    }

    /// Move operator.
    ///
    /// Transfers ownership.
    ///
    /// \param op Source.
    /// \return This object.
    unique_ptr<T>& operator=(unique_ptr<T>&& op)
    {
        reset(op.m_ref);
        op.m_ref = NULL;
        return *this;
    }

    /// Truth value operator.
    ///
    /// \return True if pointer is not empty.
    constexpr operator bool() const
    {
        return (m_ref != NULL);
    }
};

/// Creates an unique_ptr and returns it.
///
/// Note that this function may sometimes increase code size.
/// Only use it for trivial situations such as returning unique pointers.
///
/// \param args Arguments.
/// \return New unique_ptr of the chosen type.
template<typename T, typename...Args> inline unique_ptr<T> make_unique(Args&&...args)
{
    return unique_ptr<T>(new T(args...));
}

}

#endif
