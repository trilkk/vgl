#ifndef VGL_MUTEX_HPP
#define VGL_MUTEX_HPP

#if defined(VGL_ENABLE_GTK)
#include "vgl_realloc.hpp"
#include "vgl_extern_gtk.hpp"
#else
#include "vgl_extern_sdl.hpp"
#endif

#if defined(USE_LD)
#include "vgl_throw_exception.hpp"
#include <ostream>
#endif

namespace vgl
{

/// Mutex class.
class Mutex
{
public:
    /// Internal mutex implementation.
    using mutex_type =
#if defined(VGL_ENABLE_GTK)
        GMutex
#else
        SDL_mutex
#endif
        ;

private:
    /// Actual mutex.
    mutex_type* m_mutex;

private:
    /// Deleted copy constructor.
    Mutex(const Mutex&) = delete;
    /// Deleted assignment.
    Mutex& operator=(const Mutex&) = delete;

public:
    /// Default constructor.
    explicit Mutex() :
#if defined(VGL_ENABLE_GTK)
        m_mutex(new mutex_type)
#else
        m_mutex(dnload_SDL_CreateMutex())
#endif
    {
#if defined(VGL_ENABLE_GTK)
        dnload_g_mutex_init(m_mutex);
#elif defined(USE_LD) && defined(DEBUG)
        if(!m_mutex)
        {
            VGL_THROW_RUNTIME_ERROR(string("Mutex::Mutex(): ") + SDL_GetError());
        }
#endif
    }

    /// Constructor.
    ///
    /// \param op Internal type to adapt.
    constexpr Mutex(mutex_type* op) noexcept :
        m_mutex(op)
    {
    }

    /// Destructor.
    ~Mutex()
    {
        destruct();
    }

    /// Move constructor.
    ///
    /// \param other Source object.
    constexpr Mutex(Mutex&& other) noexcept :
        Mutex(other.m_mutex)
    {
        other.m_mutex = nullptr;
    }

public:
    /// Accessor.
    ///
    /// \return Internal implementation.
    constexpr mutex_type* getMutexImpl() const noexcept
    {
        return m_mutex;
    }

    /// Lock.
    void acquire() const
    {
        internal_mutex_acquire(getMutexImpl());
    }

    /// Unlock.
    void release() const
    {
        internal_mutex_release(getMutexImpl());
    }

private:
    /// Internal destructor.
    void destruct()
    {
        if(m_mutex)
        {
#if defined(VGL_ENABLE_GTK)
            dnload_g_mutex_clear(m_mutex);
            delete m_mutex;
#else
            dnload_SDL_DestroyMutex(m_mutex);
#endif
        }
    }

public:
    /// Acquire mutex implementation.
    ///
    /// \param op Mutex implementation.
    static inline void internal_mutex_acquire(mutex_type* op)
    {
#if defined(VGL_ENABLE_GTK)
        dnload_g_mutex_lock(op);
#else
        int err = dnload_SDL_LockMutex(op);
#if defined(USE_LD) && defined(DEBUG)
        if(err)
        {
            VGL_THROW_RUNTIME_ERROR(string("internal_mutex_acquire(): ") + SDL_GetError());
        }
#else
        (void)err;
#endif
#endif
    }

    /// Release mutex implementation.
    ///
    /// \param op Mutex implementation.
    static inline void internal_mutex_release(mutex_type* op)
    {
#if defined(VGL_ENABLE_GTK)
        dnload_g_mutex_unlock(op);
#else
        int err = dnload_SDL_UnlockMutex(op);
#if defined(USE_LD) && defined(DEBUG)
        if(err)
        {
            VGL_THROW_RUNTIME_ERROR(string("internal_mutex_release(): ") + SDL_GetError());
        }
#else
        (void)err;
#endif
#endif
    }

public:
    /// Move operator.
    ///
    /// \param other Source object.
    /// \return This object.
    Mutex& operator=(Mutex&& other)
    {
        destruct();
        m_mutex = other.m_mutex;
        other.m_mutex = nullptr;
        return *this;
    }

public:
#if defined(USE_LD)
    /// Stream output operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const Mutex& rhs)
    {
        return lhs << "Mutex(" << rhs.getMutexImpl() << ")";
    }
#endif
};

}

#endif
