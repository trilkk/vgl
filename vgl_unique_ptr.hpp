#ifndef VGL_UNIQUE_PTR_HPP
#define VGL_UNIQUE_PTR_HPP

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
    constexpr explicit unique_ptr(T *op) :
        m_ref(op)
    {
    }

    /// Move constructor.
    ///
    /// Transfers ownership.
    ///
    /// \param op Source.
    unique_ptr(unique_ptr<T> &&op) :
        m_ref(op.release())
    {
    }

    /// Constructor from nullptr.
    unique_ptr(const std::nullptr_t&) :
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

    /// Release ownership, return released pointer.
    ///
    /// Should not be called by user.
    T* release()
    {
        T* ret = m_ref;
        m_ref = NULL;
        return ret;
    }

public:
    /// Accessor.
    ///
    /// \return Referenced pointer.
    T* get() const
    {
        return m_ref;
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
    T* operator->() const
    {
        return m_ref;
    }

    /// Dereference operator.
    ///
    /// \return Dereferenced contents.
    T& operator*()
    {
        return *m_ref;
    }
    /// Dereference operator.
    ///
    /// \return Dereferenced contents.
    const T& operator*() const
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
    unique_ptr<T>& operator=(const std::nullptr_t&)
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
    unique_ptr<T>& operator=(unique_ptr<T> &&op)
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

}

#endif
